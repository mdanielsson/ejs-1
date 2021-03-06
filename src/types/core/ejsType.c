/**
 *  ejsType.c - Type class
 *
 *  The type class is the base class for all types (classes) in the system.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/***************************** Forward Declarations ***************************/

static EjsType *createBootstrapType(Ejs *ejs, int numSlots);
static EjsType *createType(Ejs *ejs, EjsName *qname, EjsModule *up, EjsType *baseType, int instanceSize, int numSlots, 
        int attributes, void *typeData);
static EjsBlock *createInstanceBlock(Ejs *ejs, cchar *name, EjsBlock *baseBlock, int numSlots, int attributes);
static void fixInstanceSize(Ejs *ejs, EjsType *type);

/******************************************************************************/
/*
 *  Copy a type. 
 *
 *  intrinsic function copy(type: Object): Object
 */
static EjsType *cloneTypeVar(Ejs *ejs, EjsType *src, bool deep)
{
    EjsType     *dest;

    if (! ejsIsType(src)) {
        ejsThrowTypeError(ejs, "Expecting a Type object");
        return 0;
    }

    dest = (EjsType*) (ejs->blockHelpers->cloneVar)(ejs, (EjsVar*) src, deep);
    if (dest == 0) {
        return dest;
    }

    dest->qname = src->qname;
    dest->baseType = src->baseType;
    dest->instanceBlock = src->instanceBlock;
    dest->instanceSize = src->instanceSize;
    dest->helpers = src->helpers;
    dest->module = src->module;
    dest->typeData = src->typeData;
    dest->id = src->id;

    /*
     *  OPT
     */
    dest->subTypeCount = src->subTypeCount;
    dest->callsSuper = src->callsSuper;
    dest->final = src->final;
    dest->fixupDone = src->fixupDone;
    dest->hasBaseConstructors = src->hasBaseConstructors;
    dest->hasBaseInitializers = src->hasBaseInitializers;
    dest->hasBaseStaticInitializers = src->hasBaseStaticInitializers;
    dest->hasConstructor = src->hasConstructor;
    dest->hasInitializer = src->hasInitializer;
    dest->hasStaticInitializer = src->hasStaticInitializer;
    dest->initialized = src->initialized;
    dest->isInterface = src->isInterface;
    dest->objectBased = src->objectBased;
    dest->needFixup = src->needFixup;
    dest->numericIndicies = src->numericIndicies;
    dest->skipScope = src->skipScope;

    /* Don't copy pool. The cloned copy will have its own pool */

    return dest;
}


/*
 *  Create a new Type object. numSlots is the number of property slots to pre-allocate.
 *  This is hand-crafted to create types as small as possible.
 */
static EjsType *createTypeVar(Ejs *ejs, EjsType *typeType, int numSlots)
{
    EjsType         *type;
    EjsObject       *obj;
    EjsVar          *vp;
    EjsHashEntry    *entries;
    char            *start;
    int             typeSize, allocSlots, hashSize, i, dynamic;

    mprAssert(ejs);

    /*
     *  If the compiler is building itself (empty mode), then the types themselves must be dynamic. Otherwise, the type
     *  is fixed and will contain the names hash and traits in one memory block. 
     *  NOTE: don't confuse this with dynamic objects.
     */
    hashSize = 0;

    if ((ejs->flags & (EJS_FLAG_EMPTY | EJS_FLAG_COMPILER | EJS_FLAG_DYNAMIC))) {
        dynamic = 1;
        typeSize = sizeof(EjsType);
        allocSlots = 0;

    } else {
        dynamic = 0;
        allocSlots = numSlots;
        typeSize = sizeof(EjsType);
        typeSize += (int) sizeof(EjsNames);
        typeSize += ((int) sizeof(EjsHashEntry) * numSlots);
        if (numSlots > EJS_HASH_MIN_PROP) {
            hashSize = ejsGetHashSize(numSlots);
            typeSize += (hashSize * (int) sizeof(int*));
        }
        typeSize += (int) sizeof(EjsTrait) * numSlots;
        typeSize += (int) sizeof(EjsVar*) * numSlots;
    }

    if ((vp = (EjsVar*) mprAllocZeroed(ejsGetAllocCtx(ejs), typeSize)) == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    obj = (EjsObject*) vp;
    type = (EjsType*) vp;
    ejsSetDebugName(vp, "type");

    vp->type = type;
    vp->master = (ejs->master == 0);
    vp->isObject = 1;
    vp->type = typeType;
    vp->isType = 1;
    vp->dynamic = dynamic;
    obj->capacity = allocSlots;
    type->subTypeCount = typeType->subTypeCount + 1;
    ejsInitList(&type->block.namespaces);

    if (dynamic) {
        if (numSlots > 0 && ejsGrowBlock(ejs, (EjsBlock*) type, numSlots) < 0) {
            mprAssert(0);
            return 0;
        }
        type->block.numTraits = numSlots;
        mprAssert(type->block.numTraits <= type->block.sizeTraits);

    } else {
        /*
         *  This is for a fixed type. This is the normal case when not compiling. Layout is:
         *
         *  Type                Factor
         *      EjsNames
         *      EjsHashEntry    * numSlots (These are the property names and hash linkage)
         *      Hash buckets    ejsGetHashSize(numslots)
         *      Hash entries    (int*) * numSlots
         *      Traits          * numSlots
         *      Slots           * numSlots
         */
        start = (char*) type + sizeof(EjsType);

        obj->names = (EjsNames*) start;
        obj->names->sizeEntries = numSlots;
        start += sizeof(EjsNames);

        entries = obj->names->entries = (EjsHashEntry*) start;
        for (i = 0; i < numSlots; i++) {
            entries[i].nextSlot = -1;
            entries[i].qname.name = "";
            entries[i].qname.space = "";
        }
        start += (sizeof(EjsHashEntry) * numSlots);

        if (hashSize > 0) {
            obj->names->buckets = (int*) start;
            obj->names->sizeBuckets = hashSize;
            memset(obj->names->buckets, -1, hashSize * sizeof(int));
            start += sizeof(int) * hashSize;
        }

        type->block.traits = (EjsTrait*) start;
        type->block.sizeTraits = numSlots;
        type->block.numTraits = numSlots;
        start += sizeof(EjsTrait) * numSlots;

        obj->slots = (EjsVar**) start;
        obj->numProp = numSlots;
        start += (numSlots * sizeof(EjsVar*));

        mprAssert((start - (char*) type) <= typeSize);
    }
#if BLD_DEBUG
    ejsAddToGcStats(ejs, vp, ES_Type);
#endif
    return type;
}


/*
 *  Create a bootstrap type variable. This is used for the Object, Block and Type types.
 */
static EjsType *createBootstrapType(Ejs *ejs, int numSlots)
{
    EjsType     *type, bootstrap;
    EjsBlock    bootstrapInstanceBlock;

    mprAssert(ejs);

    memset(&bootstrap, 0, sizeof(bootstrap));
    memset(&bootstrapInstanceBlock, 0, sizeof(bootstrapInstanceBlock));

    bootstrap.instanceSize = sizeof(EjsType);
    bootstrap.subTypeCount = 0;
    bootstrap.instanceBlock = &bootstrapInstanceBlock;

    type = (EjsType*) createTypeVar(ejs, &bootstrap, numSlots);
    if (type == 0) {
        return 0;
    }
    /*
     *  This will be hand-crafted later
     */
    type->block.obj.var.type = 0;
    return type;
}


void markType(Ejs *ejs, EjsVar *parent, EjsType *type)
{
    ejsMarkBlock(ejs, parent, (EjsBlock*) type);

    if (type->instanceBlock) {
        ejsMarkVar(ejs, (EjsVar*) type, (EjsVar*) type->instanceBlock);
    }
}


static int setTypeProperty(Ejs *ejs, EjsType *type, int slotNum, EjsVar *value)
{
    if (slotNum < 0 && !type->block.obj.var.dynamic) {
        ejsThrowTypeError(ejs, "Object is not dynamic");
        return EJS_ERR;
    }
    return (ejs->blockHelpers->setProperty)(ejs, (EjsVar*) type, slotNum, value);
}


/******************************** Native Type API *****************************/
/*
 *  Create a core built-in type. This is used by core native type code to either create a type or to get a type
 *  that has been made by loading ejs.mod. Handles the EMPTY case when building the compiler itself.
 */
EjsType *ejsCreateCoreType(Ejs *ejs, EjsName *qname, EjsType *baseType, int instanceSize, int slotNum, int numTypeProp,
    int numInstanceProp, int attributes)
{
    EjsType     *type;

    type = ejsCreateType(ejs, qname, 0, baseType, instanceSize, slotNum, numTypeProp, numInstanceProp, attributes, 0);
    if (type == 0) {
        ejs->hasError = 1;
        return 0;
    }
    
    /*
     *  The coreTypes hash allows the loader to match the essential core type objects to those being loaded from a mod file.
     */
    mprAddHash(ejs->coreTypes, qname->name, type);
    return type;
}


EjsBlock *ejsCreateTypeInstanceBlock(Ejs *ejs, EjsType *type, int numInstanceProp)
{
    EjsType     *baseType;
    EjsBlock    *block;
    char        *instanceName;
    int         attributes;

    instanceName = mprStrcat(type, -1, type->qname.name, "InstanceType", NULL);

    attributes = 0;
    if (type->block.obj.var.native) {
       attributes |= EJS_ATTR_NATIVE;
    }
    baseType = type->baseType;
    block = createInstanceBlock(ejs, instanceName, (baseType) ? baseType->instanceBlock: 0, numInstanceProp, attributes);
    if (block == 0) {
        return 0;
    }
    type->instanceBlock = block;
    block->nobind = type->block.nobind;
    block->dynamicInstance = type->block.dynamicInstance;
    return block;
}


EjsType *ejsCreatePrototype(Ejs *ejs, EjsFunction *fun, int *prototypeSlot)
{
    EjsName     qname;
    EjsType     *type;
    int         slotNum;

    qname = ejsGetPropertyName(ejs, fun->owner, fun->slotNum);
    mprAssert(qname.name);

    slotNum = ejsGetPropertyCount(ejs, ejs->global);

    ejs->flags |= EJS_FLAG_DYNAMIC;
    type = ejsCreateType(ejs, &qname, NULL, ejs->objectType, ejs->objectType->instanceSize,
        slotNum, ES_Object_NUM_CLASS_PROP, ES_Object_NUM_INSTANCE_PROP, 
        EJS_ATTR_OBJECT | EJS_ATTR_DYNAMIC_INSTANCE | EJS_ATTR_OBJECT_HELPERS | EJS_ATTR_HAS_CONSTRUCTOR, NULL);
    ejs->flags &= ~EJS_FLAG_DYNAMIC;

    /*
     *  Install function as the constructor (always first class after object)
     */
    fun->constructor = 1;
    fun->thisObj = 0;
    ejsSetPropertyByName(ejs, (EjsVar*) type, &qname, (EjsVar*) fun);

    /*
     *  Setup the type as the Function.prototype
     */
    fun->prototype = type;
    if (fun->properties == 0) {
        fun->properties = ejsCreateSimpleObject(ejs);
    }
    *prototypeSlot = ejsSetPropertyByName(ejs, (EjsVar*) fun->properties, ejsName(&qname, "", "prototype"), (EjsVar*) type);

#if KEEP
    /*
     *  Also install as type.prototype
     */
    ejsSetPropertyByName(ejs, (EjsVar*) type, ejsName(&qname, "", "prototype"), (EjsVar*) type);
#endif
    return type;
}


/*
 *  Create a new type and initialize. BaseType is the super class for instances of the type being created. The
 *  returned EjsType will be an instance of EjsType. numTypeProp and  numInstanceProp should be set to the number
 *  of non-inherited properties.
 */
EjsType *ejsCreateType(Ejs *ejs, EjsName *qname, EjsModule *up, EjsType *baseType, int instanceSize,
                       int slotNum, int numTypeProp, int numInstanceProp, int attributes, void *typeData)
{
    EjsType     *type;
    int         needInstanceBlock;
    
    mprAssert(ejs);
    mprAssert(slotNum >= 0);
    
    needInstanceBlock = numInstanceProp;
    
    if ((ejs->flags & EJS_FLAG_EMPTY) && !ejs->initialized && attributes & EJS_ATTR_NATIVE) {
        /*
         *  If an empty interpreter, must not set a high number of properties based on the last slot generation.
         *  Property counts may be lower or zero this time round.
         */
        numTypeProp = 0;
        numInstanceProp = 0;
    }
    type = createType(ejs, qname, up, baseType, instanceSize, numTypeProp, attributes, typeData);
    if (type == 0) {
        return 0;
    }
    type->id = slotNum;
    ejsSetDebugName(type, type->qname.name);

    if (needInstanceBlock) {
        type->instanceBlock = ejsCreateTypeInstanceBlock(ejs, type, numInstanceProp);
    }
    return type;
}


/*
 *  Create a type object and initialize.
 */
static EjsType *createType(Ejs *ejs, EjsName *qname, EjsModule *up, EjsType *baseType, int instanceSize, int numSlots, 
        int attributes, void *typeData)
{
    EjsType     *type;

    mprAssert(ejs);
    mprAssert(instanceSize > 0);
    
    /*
     *  Create the type. For Object and Type, the value of ejs->typeType will be null. So bootstrap these first two types. 
     */
    if (ejs->typeType == 0) {
        type = (EjsType*) createBootstrapType(ejs, numSlots);

    } else {
        type = (EjsType*) createTypeVar(ejs, ejs->typeType, numSlots);
    }
    if (type == 0) {
        return 0;
    }

    if (baseType) {
        mprAssert(!(attributes & EJS_ATTR_SLOTS_NEED_FIXUP));

        if (baseType->hasConstructor || baseType->hasBaseConstructors) {
            type->hasBaseConstructors = 1;
        }
        if (baseType->hasInitializer || baseType->hasBaseInitializers) {
            type->hasBaseInitializers = 1;
        }
        type->baseType = baseType;
    }

    type->qname.name = qname->name;
    type->qname.space = qname->space;
    type->module = up;
    type->typeData = typeData;
    type->baseType = baseType;
    
    type->block.obj.var.native = (attributes & EJS_ATTR_NATIVE) ? 1 : 0;
    
    type->instanceSize = instanceSize;
    if (baseType) {
        fixInstanceSize(ejs, type);
    }

    /*
     *  OPT - should be able to just read in the attributes without having to stuff some in var and some in type.
     *  Should eliminate all the specific fields and just use BIT MASKS.
     */
    if (attributes & EJS_ATTR_SLOTS_NEED_FIXUP) {
        type->needFixup = 1;
    }
    if (attributes & EJS_ATTR_INTERFACE) {
        type->isInterface = 1;
    }
    if (attributes & EJS_ATTR_FINAL) {
        type->final = 1;
    }
    if (attributes & EJS_ATTR_OBJECT) {
        type->objectBased = 1;
    }
    if (attributes & EJS_ATTR_DYNAMIC_INSTANCE) {
        type->block.dynamicInstance = 1;
    }
    if (attributes & EJS_ATTR_HAS_CONSTRUCTOR) {
        /*
         *  This means the type certainly has a constructor method.
         */
        type->hasConstructor = 1;
    }
    if (attributes & EJS_ATTR_HAS_INITIALIZER) {
        type->hasInitializer = 1;
    }
    if (attributes & EJS_ATTR_HAS_STATIC_INITIALIZER) {
        type->hasStaticInitializer = 1;
    }
    if (attributes & EJS_ATTR_CALLS_SUPER) {
        type->callsSuper = 1;
    }
    if (attributes & EJS_ATTR_NO_BIND) {
        type->block.nobind = 1;
    }
    if (attributes & EJS_ATTR_BLOCK_HELPERS) {
        type->helpers = ejsGetBlockHelpers(ejs);
    } else if (attributes & EJS_ATTR_OBJECT_HELPERS) {
        type->helpers = ejsGetObjectHelpers(ejs);
    } else {
        type->helpers = ejsGetDefaultHelpers(ejs);
    }
    if (ejsGrowBlock(ejs, &type->block, numSlots) < 0) {
        return 0;
    }
    if (baseType && ejsInheritTraits(ejs, (EjsBlock*) type, (EjsBlock*) baseType, baseType->block.numTraits, 0, 0) < 0) {
        return 0;
    }
    return type;
}


/*
 *  Create a type instance block and initialize.
 */
static EjsBlock *createInstanceBlock(Ejs *ejs, cchar *name, EjsBlock *baseBlock, int numSlots, int attributes)
{
    EjsBlock    *block;

    mprAssert(ejs);
    
    /*
     *  Types and instance blocks are always eternal
     */
    block = ejsCreateBlock(ejs, numSlots);
    ejsSetDebugName(block, name);
    
    if (block == 0) {
        return 0;
    }

    /*
     *  OPT - should be able to just read in the attributes without having to stuff some in var and some in type.
     *  Should eliminate all the specific fields and just use BIT MASKS.
     */
    block->obj.var.native = (attributes & EJS_ATTR_NATIVE) ? 1 : 0;
    block->obj.var.isInstanceBlock = 1;
    
    if (numSlots > 0) {
        if (ejsGrowBlock(ejs, block, numSlots) < 0) {
            return 0;
        }
        if (baseBlock && ejsInheritTraits(ejs, (EjsBlock*) block, baseBlock, baseBlock->numTraits, 0, 0) < 0) {
            return 0;
        }
    }
    return block;
}


EjsType *ejsGetType(Ejs *ejs, int slotNum)
{
    EjsType     *type;

    if (slotNum < 0 || slotNum >= ejs->globalBlock->obj.numProp) {
        return 0;
    }
    type = (EjsType*) ejsGetProperty(ejs, ejs->global, slotNum);
    if (type == 0 || !ejsIsType(type)) {
        return 0;
    }
    return type;
}


static void fixInstanceSize(Ejs *ejs, EjsType *type)
{
    EjsType     *tp;

    type->hasNativeBase = 0;
    for (tp = type->baseType; tp && tp != ejs->objectType; tp = tp->baseType) {
        if (tp->instanceSize > type->instanceSize) {
            type->instanceSize = tp->instanceSize;
        }
        if (tp->block.obj.var.native) {
            type->hasNativeBase = 1;
        }
    }

    if (type->hasNativeBase && !type->block.obj.var.native) {
        /*
         *  Can't bind scripted class access if there is a native base class as the native class needs native helpers
         *  to access the properties.
         */
        type->block.nobind = 1;
        mprLog(ejs, 5, "NOTE: Type %s has a non-native base and won't be bound", type->qname.name);
    }
}


/*
 *  Fixup a type. This is used by the compiler and loader when it must first define a type without knowing the properties of 
 *  base classes. Consequently, it must fixup the type and its counts of inherited properties. It must also copy 
 *  inherited slots and traits. It is also used by the loader to fixup forward class references.
 */
int ejsFixupClass(Ejs *ejs, EjsType *type, EjsType *baseType, MprList *implements, int makeRoom)
{
    mprAssert(ejs);
    mprAssert(type);
    mprAssert(type != baseType);

    type->needFixup = 0;
    type->fixupDone = 1;
    type->baseType = baseType;
    
    if (baseType) {
        if (baseType->hasConstructor || baseType->hasBaseConstructors) {
            type->hasBaseConstructors = 1;
        }
        if (baseType->hasInitializer || baseType->hasBaseInitializers) {
            type->hasBaseInitializers = 1;
        }
        if (baseType != ejs->objectType && baseType->block.dynamicInstance) {
            type->block.dynamicInstance = 1;
        }
        type->subTypeCount = baseType->subTypeCount + 1;
    }
    fixInstanceSize(ejs, type);
    return ejsFixupBlock(ejs, (EjsBlock*) type, (EjsBlock*) baseType, implements, makeRoom);
}


/*
 *  Fixup a block. This is used by the compiler and loader when it must first define a type without knowing the properties 
 *  of base classes. Consequently, it must fixup the type and its counts of inherited properties. It must also copy 
 *  inherited slots and traits. It is also used by the loader to fixup forward class references.
 */
int ejsFixupBlock(Ejs *ejs, EjsBlock *block, EjsBlock *baseBlock, MprList *implements, int makeRoom)
{
    EjsType         *ifaceType;
    EjsBlock        *iface;
    EjsNamespace    *nsp;
    bool            isInstanceBlock;
    int             next, offset, count, nextNsp;

    mprAssert(ejs);
    mprAssert(block);
    mprAssert(block != baseBlock);

    isInstanceBlock = block->obj.var.isInstanceBlock;
    
    if (makeRoom) {
        /*
         *  Count the number of inherited traits and insert
         */
        count = (baseBlock) ? baseBlock->numTraits: 0;
        if (implements) {
            for (next = 0; ((iface = mprGetNextItem(implements, &next)) != 0); ) {
                iface = (isInstanceBlock) ? ((EjsType*) iface)->instanceBlock: iface;
                if (iface) {
                    ifaceType = (EjsType*) iface;
                    if (!ifaceType->isInterface) {
                        /*
                         *  Only inherit properties from implemented classes
                         */
                        count += iface->numTraits - iface->numInherited;
                    }
                }
            }
        }
        if (ejsInsertGrowBlock(ejs, block, count, 0) < 0) {
            return EJS_ERR;
        }
    }

    /*
     *  Copy the inherited traits from the base block and all implemented interfaces
     */
    offset = 0;
    if (baseBlock) {
        if (ejsInheritTraits(ejs, block, baseBlock, baseBlock->numTraits, offset, 0) < 0) {
            return EJS_ERR;
        }
        offset += baseBlock->numTraits;
    }
    
    if (implements) {
        for (next = 0; ((iface = mprGetNextItem(implements, &next)) != 0); ) {
            /*
             *  Only insert the first level of inherited traits
             */
            iface = (isInstanceBlock) ? ((EjsType*) iface)->instanceBlock: iface;
            if (iface) {
                ifaceType = (EjsType*) iface;
                if (!ifaceType->isInterface) {
                    count = iface->numTraits - iface->numInherited;
                    ejsInheritTraits(ejs, block, iface, count, offset, 1);
                    offset += iface->numTraits;
                }
                for (nextNsp = 0; (nsp = (EjsNamespace*) ejsGetNextItem(&iface->namespaces, &nextNsp)) != 0; ) {
                    ejsAddNamespaceToBlock(ejs, block, nsp);
                }
            }
        }
    }
    return 0;
}


/*
 *  Set the native method function for a function property
 */
int ejsBindMethod(Ejs *ejs, EjsType *type, int slotNum, EjsNativeFunction nativeProc)
{
    return ejsBindFunction(ejs, &type->block, slotNum, nativeProc);
}


/*
 *  Set the native method function for a function property
 */
int ejsBindFunction(Ejs *ejs, EjsBlock *block, int slotNum, EjsNativeFunction nativeProc)
{
    EjsFunction     *fun;
    EjsName         qname;

    fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) block, slotNum);

    if (fun == 0 || !ejsIsFunction(fun)) {
        mprAssert(fun);
        ejs->hasError = 1;
        mprError(ejs, "Attempt to bind non-existant function for slot %d in block/type \"%s\"", slotNum, 
            ejsGetDebugName(block));
        return EJS_ERR;
    }

    if (fun->body.code.codeLen != 0) {
        qname = ejsGetPropertyName(ejs, fun->owner, fun->slotNum);
        mprError(ejs, "Setting a native method on a non-native function \"%s\" in block/type \"%s\"", qname.name, 
            ejsGetDebugName(block));
        ejs->hasError = 1;
    }
    fun->body.proc = nativeProc;
    fun->nativeProc = 1;
    return 0;
}


/*
 *  Define a global public function. Returns a positive slot number, otherwise a negative MPR error.
 */
int ejsDefineGlobalFunction(Ejs *ejs, cchar *name, EjsNativeFunction fn)
{
    EjsFunction *fun;
    EjsName     qname;

    if ((fun = ejsCreateFunction(ejs, NULL, -1, 0, 0, ejs->objectType, 0, NULL, NULL, 0)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    fun->body.proc = fn;
    fun->nativeProc = 1;
    ejsName(&qname, EJS_PUBLIC_NAMESPACE, name);
    return ejsSetPropertyByName(ejs, ejs->global, &qname, (EjsVar*) fun);
}


/*
 *  Define a property. If then property type is null, then use the value's type if supplied. If no value, then set to the 
 *  void type. If OVERRIDE is not set in attributes, then the slotNum is offset above the base class slots.
 */
int ejsDefineInstanceProperty(Ejs *ejs, EjsType *type, int slotNum, EjsName *name, EjsType *propType, int attributes, 
        EjsVar *value)
{
    return ejsDefineProperty(ejs, (EjsVar*) type->instanceBlock, slotNum, name, propType, attributes, value);
}


/*
 *  Return true if target is an instance of type or a sub class of it.
 */
bool ejsIsA(Ejs *ejs, EjsVar *target, EjsType *type)
{
    EjsType     *tp;

    mprAssert(type);

    if (!ejsIsType(type)) {
        return 0;
    }
    if (target == 0) {
        return 0;
    }
    tp = ejsIsType(target) ? (EjsType*) target : target->type;
    return ejsIsTypeSubType(ejs, tp, type);
}


/*
 *  Return true if "target" is a "type", subclass of "type" or implements "type".
 */
bool ejsIsTypeSubType(Ejs *ejs, EjsType *target, EjsType *type)
{
    EjsType     *tp, *iface;
    int         next;

    mprAssert(target);
    mprAssert(type);
    
    if (!ejsIsType(target) || !ejsIsType(type)) {
        return 0;
    }

    /*
     *  See if target is a subtype of type
     */
    for (tp = target; tp; tp = tp->baseType) {
        /*
         *  Test ID also to allow cloned interpreters to match where the IDs are equal
         */
        if (tp == type || tp->id == type->id) {
            return 1;
        }
    }
    
    /*
     *  See if target implements type
     */
    if (target->implements) {
        for (next = 0; (iface = mprGetNextItem(target->implements, &next)) != 0; ) {
            if (iface == type) {
                return 1;
            }
        }
    }

    return 0;
}


/*
 *  Get the attributes of the type property at slotNum.
 *
 */
int ejsGetTypePropertyAttributes(Ejs *ejs, EjsVar *vp, int slotNum)
{
    EjsType     *type;

    if (!ejsIsType(vp)) {
        mprAssert(ejsIsType(vp));
        return EJS_ERR;
    }
    type = (EjsType*) vp;
    return ejsGetTraitAttributes((EjsBlock*) type, slotNum);
}


/*
 *  This call is currently only used to update the type namespace after resolving a run-time namespace.
 */
void ejsSetTypeName(Ejs *ejs, EjsType *type, EjsName *qname)
{
    type->qname.name = qname->name;
    type->qname.space = qname->space;
    ejsSetDebugName(type, qname->name);
    if (type->instanceBlock) {
        ejsSetDebugName(type->instanceBlock, qname->name);
    }
}


/*
 *  Define namespaces for a class. Inherit the protected and internal namespaces from all base classes.
 */
void ejsDefineTypeNamespaces(Ejs *ejs, EjsType *type)
{
    EjsNamespace        *nsp;

    if (type->baseType) {
        /*
         *  Inherit the base class's protected and internal namespaces
         */
        ejsInheritBaseClassNamespaces(ejs, type, type->baseType);
    }
    nsp = ejsDefineReservedNamespace(ejs, (EjsBlock*) type, &type->qname, EJS_PROTECTED_NAMESPACE);
    nsp->flags |= EJS_NSP_PROTECTED;
    nsp = ejsDefineReservedNamespace(ejs, (EjsBlock*) type, &type->qname, EJS_PRIVATE_NAMESPACE);
    nsp->flags |= EJS_NSP_PRIVATE;
}


void ejsTypeNeedsFixup(Ejs *ejs, EjsType *type)
{
    mprAssert(type);

    type->needFixup = 1;
    type->baseType = 0;
}


/*
 *  Return the total memory size used by a type
 */
static int ejsGetBlockSize(Ejs *ejs, EjsBlock *block)
{
    int     size, numProp;

    numProp = ejsGetPropertyCount(ejs, (EjsVar*) block);

    size = (int) (sizeof(EjsType) + sizeof(EjsTypeHelpers) + (numProp * sizeof(EjsVar*)));
    if (block->obj.names) {
        size += (int) (sizeof(EjsNames) + (block->obj.names->sizeEntries * sizeof(EjsHashEntry)));
        size += (int) ((block->obj.names->sizeBuckets * sizeof(int*)));
    }
    size += (int) (ejsGetNumTraits(block) * sizeof(EjsTrait));
    return size;
}


/*
 *  Return the total memory size used by a type
 */
int ejsGetTypeSize(Ejs *ejs, EjsType *type)
{
    int     size;

    size = ejsGetBlockSize(ejs, (EjsBlock*) type);
    if (type->instanceBlock) {
        size += ejsGetBlockSize(ejs, type->instanceBlock);
    }
    return size;
}


/*********************************** Factory **********************************/

void ejsCreateTypeType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;
    int         flags;

    flags = EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_BLOCK_HELPERS;
    if (ejs->flags & EJS_FLAG_EMPTY) {
        flags |= EJS_ATTR_DYNAMIC_INSTANCE;
    }

    type = ejsCreateCoreType(ejs, ejsName(&qname, EJS_INTRINSIC_NAMESPACE, "Type"), ejs->objectType, sizeof(EjsType), 
        ES_Type, ES_Type_NUM_CLASS_PROP, ES_Type_NUM_INSTANCE_PROP, flags);
    ejs->typeType = type;

    /*
     *  Override the createVar helper when creating types.
     */
    type->helpers->cloneVar     = (EjsCloneVarHelper) cloneTypeVar;
    type->helpers->createVar    = (EjsCreateVarHelper) createTypeVar;
    type->helpers->setProperty  = (EjsSetPropertyHelper) setTypeProperty;
    type->helpers->markVar      = (EjsMarkVarHelper) markType;

    /*
     *  WARNING: read closely. This can be confusing. Fixup the helpers for the object type. We need to find
     *  helpers via objectType->var.type->helpers. So we set it to the Type type. We keep objectType->baseType == 0
     *  because Object has no base type. Similarly for the Type type.
     */
    ejs->objectType->block.obj.var.type = ejs->typeType;
    ejs->typeType->block.obj.var.type = ejs->objectType;
}


void ejsConfigureTypeType(Ejs *ejs)
{
}


/*
 *  @copy   default
 *
 *  Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
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
 *  Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
