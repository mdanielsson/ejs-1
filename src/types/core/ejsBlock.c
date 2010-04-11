/**
 *  ejsBlock.c - Lexical block
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/*********************************** Forwards *********************************/

static int growTraits(EjsBlock *block, int numTraits);
static int insertGrowTraits(EjsBlock *block, int count, int offset);

/*********************************** Helpers **********************************/

EjsBlock *ejsCreateBlock(Ejs *ejs, int size)
{
    EjsBlock        *block;

    block = (EjsBlock*) ejsCreateObject(ejs, ejs->blockType, size);
    if (block == 0) {
        return 0;
    }
    ejsInitList(&block->namespaces);
    return block;
}


/*
 *  Define a new property and set its name, type, attributes and property value.
 */
static int defineBlockProperty(Ejs *ejs, EjsBlock *block, int slotNum, EjsName *qname, EjsType *propType, int attributes, 
    EjsVar *val)
{
    EjsFunction     *fun;
    EjsType         *type;

    mprAssert(ejs);
    mprAssert(slotNum >= -1);
    mprAssert(ejsIsObject(block));
    mprAssert(qname);

    if (val == 0) {
        val = ejs->nullValue;
    }

    if (slotNum < 0) {
        slotNum = ejsGetPropertyCount(ejs, (EjsVar*) block);
    }
    if (ejsSetProperty(ejs, (EjsVar*) block, slotNum, val) < 0) {
        return EJS_ERR;
    }
    if (ejsSetPropertyName(ejs, (EjsVar*) block, slotNum, qname) < 0) {
        return EJS_ERR;
    }
    if (ejsSetPropertyTrait(ejs, (EjsVar*) block, slotNum, propType, attributes) < 0) {
        return EJS_ERR;
    }
    if (ejsIsFunction(val)) {
        fun = ((EjsFunction*) val);
        if (attributes & EJS_ATTR_CONSTRUCTOR) {
            fun->constructor = 1;
        }
        ejsSetFunctionLocation(fun, (EjsVar*) block, slotNum);
        if (fun->getter || fun->setter) {
             block->obj.var.hasGetterSetter = 1;
        }
        if (!ejsIsNativeFunction(fun)) {
            block->hasScriptFunctions = 1;
        }
        if (fun->staticMethod && ejsIsType(block)) {
            type = (EjsType*) block;
            if (!type->isInterface) {
                /*
                 *  For static methods, find the right base class and set thisObj to speed up later invocations.
                 */
                fun->thisObj = (EjsVar*) block;
            } else {
                mprAssert(0);
            }
        }
    }
    return slotNum;
}


/*
 *  Get the property Trait
 */
static EjsTrait *getBlockPropertyTrait(Ejs *ejs, EjsBlock *block, int slotNum)
{
    return ejsGetTrait(block, slotNum);
}


void ejsMarkBlock(Ejs *ejs, EjsVar *parent, EjsBlock *block)
{
    EjsVar          *item;
    EjsBlock        *b;
    int             next;

    ejsMarkObject(ejs, parent, (EjsObject*) block);
    if (block->prevException) {
        ejsMarkVar(ejs, (EjsVar*) block, (EjsVar*) block->prevException);
    }
    if (block->namespaces.length > 0) {
        for (next = 0; ((item = (EjsVar*) ejsGetNextItem(&block->namespaces, &next)) != 0); ) {
            ejsMarkVar(ejs, (EjsVar*) block, item);
        }
    }
    for (b = block->scopeChain; b; b = b->scopeChain) {
        ejsMarkVar(ejs, (EjsVar*) block, (EjsVar*) b);
    }
    for (b = block->prev; b; b = b->prev) {
        ejsMarkVar(ejs, (EjsVar*) block, (EjsVar*) b);
    }
}


/*
 *  Set the property Trait
 */
static int setBlockPropertyTrait(Ejs *ejs, EjsBlock *block, int slotNum, EjsType *type, int attributes)
{
    return ejsSetTrait(block, slotNum, type, attributes);
}


/*******************************************************************************************/
/*
 *  Grow the block traits, slots and names. This will update numTraits and numProp.
 */
int ejsGrowBlock(Ejs *ejs, EjsBlock *block, int size)
{
    if (size == 0) {
        return 0;
    }
    if (ejsGrowObject(ejs, (EjsObject*) block, size) < 0) {
        return EJS_ERR;
    }
    if (growTraits(block, size) < 0) {
        return EJS_ERR;
    }
    return 0;
}


/*
 *  Grow the block traits, slots and names by inserting before all existing properties. This will update numTraits and 
 *  numProp.
 */
int ejsInsertGrowBlock(Ejs *ejs, EjsBlock *block, int count, int offset)
{
    EjsFunction     *fun;
    int             i;

    if (count <= 0) {
        return 0;
    }
    if (ejsInsertGrowObject(ejs, (EjsObject*) block, count, offset) < 0) {
        return EJS_ERR;
    }
    if (insertGrowTraits(block, count, offset) < 0) {
        return EJS_ERR;
    }

    /*
     *  Fixup the slot numbers of all the methods.
     */
    for (i = offset + count; i < block->numTraits; i++) {
        fun = (EjsFunction*) block->obj.slots[i];
        if (fun == 0 || !ejsIsFunction(fun)) {
            continue;
        }
        fun->slotNum += count;
        if (fun->nextSlot >= 0) {
            fun->nextSlot += count;
        }
        mprAssert(fun->slotNum == i);
        mprAssert(fun->nextSlot < block->numTraits);
    }
    return 0;
}


/*
 *  Allocate space for traits. Caller will initialize the actual traits.
 */
static int growTraits(EjsBlock *block, int numTraits)
{
    int         count;

    mprAssert(block);
    mprAssert(numTraits >= 0);

    if (numTraits == 0) {
        return 0;
    }
    if (numTraits > block->sizeTraits) {
        count = EJS_PROP_ROUNDUP(numTraits);
        block->traits = (EjsTrait*) mprRealloc(block, block->traits, sizeof(EjsTrait) * count);
        if (block->traits == 0) {
            return EJS_ERR;
        }
        memset(&block->traits[block->sizeTraits], 0, (count - block->sizeTraits) * sizeof(EjsTrait));
        block->sizeTraits = count;
    }
    if (numTraits > block->numTraits) {
        block->numTraits = numTraits;
    }
    mprAssert(block->numTraits <= block->sizeTraits);
    return 0;
}


static int insertGrowTraits(EjsBlock *block, int count, int offset)
{
    int         mark, i;

    mprAssert(block);
    mprAssert(count >= 0);

    if (count == 0) {
        return 0;
    }

    /*
     *  This call will change both numTraits and sizeTraits
     */
    growTraits(block, block->numTraits + count);

    mark = offset + count ;
    for (i = block->numTraits - 1; i >= mark; i--) {
        block->traits[i] = block->traits[i - mark];
    }
    for (; i >= offset; i--) {
        block->traits[i].attributes = 0;
        block->traits[i].type = 0;
    }
    mprAssert(block->numTraits <= block->sizeTraits);
    return 0;
}


/*
 *  Add a new trait to the trait array.
 */
int ejsSetTrait(EjsBlock *block, int slotNum, EjsType *type, int attributes)
{
    mprAssert(block);
    mprAssert(slotNum >= 0);

    if (slotNum < 0 || slotNum >= block->obj.capacity) {
        return EJS_ERR;
    }

    mprAssert(block->numTraits <= block->sizeTraits);

    if (block->sizeTraits <= slotNum) {
        growTraits(block, slotNum + 1);
    } else if (block->numTraits <= slotNum) {
        block->numTraits = slotNum + 1;
    }
    
    block->traits[slotNum].type = type;
    block->traits[slotNum].attributes = attributes;
    
    mprAssert(block->numTraits <= block->sizeTraits);
    mprAssert(slotNum < block->sizeTraits);

    return slotNum;
}


void ejsSetTraitType(EjsTrait *trait, EjsType *type)
{
    mprAssert(trait);
    mprAssert(type == 0 || ejsIsType(type));

    trait->type = type;
}


void ejsSetTraitAttributes(EjsTrait *trait, int attributes)
{
    mprAssert(trait);

    trait->attributes = attributes;
}


/*
 *  Remove the designated slot. If compact is true, then copy slots down.
 */
static int removeTrait(EjsBlock *block, int slotNum, int compact)
{
    int         i;

    mprAssert(block);
    mprAssert(block->numTraits <= block->sizeTraits);
    
    if (slotNum < 0 || slotNum >= block->numTraits) {
        return EJS_ERR;
    }

    if (compact) {
        /*
         *  Copy traits down starting at
         */
        for (i = slotNum + 1; i < block->numTraits; i++) {
            block->traits[i - 1] = block->traits[i];
        }
        block->numTraits--;
        i--;

    } else {
        i = slotNum;
    }

    mprAssert(i >= 0);
    mprAssert(i < block->sizeTraits);

    block->traits[i].attributes = 0;
    block->traits[i].type = 0;

    if ((i - 1) == block->numTraits) {
        block->numTraits--;
    }
    mprAssert(slotNum < block->sizeTraits);
    mprAssert(block->numTraits <= block->sizeTraits);
    return 0;
}


/*
 *  Copy inherited type slots and traits. Don't copy overridden properties and clear property names for static properites
 */
int ejsInheritTraits(Ejs *ejs, EjsBlock *block, EjsBlock *baseBlock, int count, int offset, bool implementing)
{
    EjsNames        *names;
    EjsTrait        *trait;
    EjsFunction     *existingFun, *fun;
    EjsVar          *vp;
    EjsObject       *obj;
    int             i, start;

    mprAssert(block);
    
    if (baseBlock == 0 || count <= 0) {
        return 0;
    }
    block->numInherited += count;
    
    mprAssert(block->numInherited <= block->numTraits);
    mprAssert(block->numTraits <= block->sizeTraits);

    obj = &block->obj;
    names = obj->names;
    if (names == 0) {
        ejsGrowObjectNames(obj, obj->numProp);
        names = obj->names;
    }

    start = baseBlock->numTraits - count;
    for (i = start; i < baseBlock->numTraits; i++, offset++) {
        trait = &block->traits[i];

        if (obj->var.isInstanceBlock) {
            /*
             *  Instance properties
             */
            obj->slots[offset] = baseBlock->obj.slots[i];
            block->traits[offset] = baseBlock->traits[i];
            names->entries[offset] = baseBlock->obj.names->entries[i];

        } else {
            existingFun = (EjsFunction*) block->obj.slots[offset];
            if (existingFun && ejsIsFunction(existingFun) && existingFun->override) {
                continue;
            }

            /*
             *  Copy implemented properties (including static and instance functions) and inherited instance functions.
             */
            vp = baseBlock->obj.slots[i];
            if (implementing || (vp && ejsIsFunction(vp) && !((EjsFunction*) vp)->staticMethod)) {
                obj->slots[offset] = vp;
                block->traits[offset] = baseBlock->traits[i];
                names->entries[offset] = baseBlock->obj.names->entries[i];
                if (implementing && ejsIsFunction(vp) && ((EjsFunction*) vp)->staticMethod) {
                    /*
                     *  defineBlockProperty sets this as an optimization. Must unset for static functions that are 
                     *  implemented by other types.
                     */
                    ((EjsFunction*) vp)->thisObj = 0;
                }
            }
            if (vp && ejsIsFunction(vp)) {
                fun = (EjsFunction*) vp;
                if (fun->override) {
                    trait->attributes |= EJS_ATTR_INHERITED;
                }
            }
        }
    }

    if (block->numTraits < block->numInherited) {
        block->numTraits = block->numInherited;
        mprAssert(block->numTraits <= block->sizeTraits);
    }
    ejsRebuildHash(ejs, obj);
    return 0;
}


/*
 *  Get a trait by slot number. (Note: use ejsGetPropertyTrait for access to a property's trait)
 */
EjsTrait *ejsGetTrait(EjsBlock *block, int slotNum)
{
    mprAssert(block);
    mprAssert(slotNum >= 0);

    if (slotNum < 0 || slotNum >= block->numTraits) {
        return 0;
    }
    mprAssert(slotNum < block->numTraits);
    return &block->traits[slotNum];
}


int ejsGetTraitAttributes(EjsBlock *block, int slotNum)
{
    mprAssert(block);
    mprAssert(slotNum >= 0);

    if (slotNum < 0 || slotNum >= block->numTraits) {
        mprAssert(0);
        return 0;
    }
    mprAssert(slotNum < block->numTraits);
    return block->traits[slotNum].attributes;
}


EjsType *ejsGetTraitType(EjsBlock *block, int slotNum)
{
    mprAssert(block);
    mprAssert(slotNum >= 0);

    if (slotNum < 0 || slotNum >= block->numTraits) {
        mprAssert(0);
        return 0;
    }
    mprAssert(slotNum < block->numTraits);
    return block->traits[slotNum].type;
}


int ejsGetNumTraits(EjsBlock *block)
{
    mprAssert(block);

    return block->numTraits;
}


int ejsGetNumInheritedTraits(EjsBlock *block)
{
    mprAssert(block);
    return block->numInherited;
}


/*
 *  Remove a property from a block. This erases the property and its traits.
 */
int ejsRemoveProperty(Ejs *ejs, EjsBlock *block, int slotNum)
{
    EjsFunction     *fun;
    EjsVar          *vp;
    int             i;

    mprAssert(ejs);
    mprAssert(block);
    
    /*
     *  Copy type slots and traits down to remove the slot
     */
    removeTrait(block, slotNum, 1);
    ejsRemoveSlot(ejs, (EjsObject*) block, slotNum, 1);

    mprAssert(block->numTraits <= block->obj.numProp);

    /*
     *  Fixup the slot numbers of all the methods
     */
    for (i = slotNum; i < block->obj.numProp; i++) {
        vp = block->obj.slots[i];
        if (vp == 0) {
            continue;
        }
        if (ejsIsFunction(vp)) {
            fun = (EjsFunction*) vp;
            fun->slotNum--;
            mprAssert(fun->slotNum == i);
            if (fun->nextSlot >= 0) {
                fun->nextSlot--;
                mprAssert(fun->slotNum < block->obj.numProp);
            }
        }
    }

    return 0;
}


EjsBlock *ejsCopyBlock(Ejs *ejs, EjsBlock *src, bool deep)
{
    EjsBlock    *dest;

    dest = (EjsBlock*) ejsCopyObject(ejs, (EjsObject*) src, deep);

    dest->numTraits = src->numTraits;
    dest->sizeTraits = src->sizeTraits;
    dest->traits = src->traits;
    dest->dynamicInstance = src->dynamicInstance;
    dest->numInherited = src->numInherited;
    dest->nobind = src->nobind;
    dest->scopeChain = src->scopeChain;
    mprAssert(dest->numTraits <= dest->sizeTraits);
    dest->namespaces = src->namespaces;
    return dest;
}

/********************************* Namespaces *******************************/

void ejsResetBlockNamespaces(Ejs *ejs, EjsBlock *block)
{
    ejsClearList(&block->namespaces);
}


int ejsGetNamespaceCount(EjsBlock *block)
{
    mprAssert(block);

    return ejsGetListCount(&block->namespaces);
}


void ejsPopBlockNamespaces(EjsBlock *block, int count)
{
    mprAssert(block);
    mprAssert(block->namespaces.length >= count);

    block->namespaces.length = count;
}


int ejsAddNamespaceToBlock(Ejs *ejs, EjsBlock *block, EjsNamespace *nsp)
{
    EjsFunction     *fun;
    EjsNamespace    *namespace;
    EjsList         *list;
    int             next;

    mprAssert(block);

    if (nsp == 0) {
        ejsThrowTypeError(ejs, "Not a namespace");
        return EJS_ERR;
    }
    fun = (EjsFunction*) block;
    list = &block->namespaces;

    if (ejsIsFunction(fun)) {
        if (fun->isInitializer && fun->owner) {
            block = block->scopeChain;
            list = &block->namespaces;
            /*
             *  If defining a namespace at the class level (outside functions) use the class itself.
             *  Initializers only run once so this should only happen once.
             */
            for (next = 0; (namespace = ejsGetNextItem(list, &next)) != 0; ) {
                if (strcmp(namespace->name, nsp->name) == 0) {
                    /* Already there */
                    return 0;
                }
            }
            if (block->obj.var.master && ejs->master) {
                nsp = ejsCreateNamespace(ejs->master, mprStrdup(ejs->master, nsp->name), mprStrdup(ejs->master, nsp->uri));
            }
        }
    }
    ejsAddItemToSharedList(block, list, nsp);
    return 0;
}


/*
 *  Inherit namespaces from base types. Only inherit protected.
 */
void ejsInheritBaseClassNamespaces(Ejs *ejs, EjsType *type, EjsType *baseType)
{
    EjsNamespace    *nsp;
    EjsBlock        *block;
    EjsList         *baseNamespaces, oldNamespaces;
    int             next;

    block = &type->block;
    oldNamespaces = block->namespaces;
    ejsInitList(&block->namespaces);
    baseNamespaces = &baseType->block.namespaces;

    if (baseNamespaces) {
        for (next = 0; ((nsp = (EjsNamespace*) ejsGetNextItem(baseNamespaces, &next)) != 0); ) {
            if (strstr(nsp->name, ",protected")) {
                ejsAddItem(block, &block->namespaces, nsp);
            }
        }
    }

    if (oldNamespaces.length > 0) {
        for (next = 0; ((nsp = (EjsNamespace*) ejsGetNextItem(&oldNamespaces, &next)) != 0); ) {
            ejsAddItem(block, &block->namespaces, nsp);
        }
    }
}


/*************************************** Factory ***********************************/

void ejsCreateBlockType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = ejsCreateCoreType(ejs, ejsName(&qname, EJS_INTRINSIC_NAMESPACE, "Block"), ejs->objectType, 
        sizeof(EjsType), ES_Block, ES_Block_NUM_CLASS_PROP, ES_Block_NUM_INSTANCE_PROP, 
        EJS_ATTR_DYNAMIC_INSTANCE | EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_BLOCK_HELPERS);
    type->skipScope = 1;
    ejs->blockType = type;
}


void ejsConfigureBlockType(Ejs *ejs)
{
}


void ejsInitializeBlockHelpers(EjsTypeHelpers *helpers)
{
    helpers->cloneVar               = (EjsCloneVarHelper) ejsCopyBlock;
    helpers->defineProperty         = (EjsDefinePropertyHelper) defineBlockProperty;
    helpers->getPropertyTrait       = (EjsGetPropertyTraitHelper) getBlockPropertyTrait;
    helpers->markVar                = (EjsMarkVarHelper) ejsMarkBlock;
    helpers->setPropertyTrait       = (EjsSetPropertyTraitHelper) setBlockPropertyTrait;
}


/*
 *  @copy   default
 *
 *  Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
 *
 *  This software is distributed under commercial and open source licenses.
 *  You may use the GPL open source license described below or you may acquire
 *  a commercial license from Embedthis Software. You agree to be fully bound
 *  by the terms of either license. Consult the LICENSE.TXT distributed with
 *  this software for full details.
 *
 *  This software is open source; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version. See the GNU General Public License for more
 *  details at: http://www.embedthis.com/downloads/gplLicense.html
 *
 *  This program is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  This GPL license does NOT permit incorporating this software into
 *  proprietary programs. If you are unable to comply with the GPL, you must
 *  acquire a commercial license to use this software. Commercial licenses
 *  for this software and support services are available from Embedthis
 *  Software at http://www.embedthis.com
 *
 *  @end
 */
