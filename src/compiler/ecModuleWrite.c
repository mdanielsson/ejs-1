/**
 *  ejsModuleWrite.c - Routines to encode and emit Ejscript byte code.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"
#include    "ecCompiler.h"

/****************************** Forward Declarations **************************/

static int  createBlockSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsBlock *vp);
static int  createClassSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsVar *vp);
static int  createDependencySection(EcCompiler *cp);
static int  createExceptionSection(EcCompiler *cp, EjsFunction *mp);
static int  createFunctionSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsFunction *fun);
static int  createGlobalProperties(EcCompiler *cp);
static int  createGlobalType(EcCompiler *cp, EjsType *klass);
static int  createPropertySection(EcCompiler *cp, EjsVar *block, int slotNum, EjsVar *vp);
static int  createSection(EcCompiler *cp, EjsVar *block, int slotNum);
static int  reserveRoom(EcCompiler *cp, int room);
static int  sum(cchar *name, int value);
static int  swapShort(EcCompiler *cp, int word);
static int  swapWord(EcCompiler *cp, int word);

#if BLD_FEATURE_EJS_DOC
static int  createDocSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsTrait *trait);
#endif

/*********************************** Code *************************************/
/*
 *  Write out the module file header
 */
int ecCreateModuleHeader(EcCompiler *cp)
{
    EjsModuleHdr    hdr;

    memset(&hdr, 0, sizeof(hdr));
    hdr.magic = swapShort(cp, EJS_MODULE_MAGIC);
    hdr.fileVersion = swapWord(cp, EJS_MODULE_VERSION);

    if (cp->empty) {
        hdr.flags |= EJS_MODULE_INTERP_EMPTY;
    }
    if (ecEncodeBlock(cp, (uchar*) &hdr, sizeof(hdr)) < 0) {
        return MPR_ERR_CANT_WRITE;
    }
    return 0;
}


/*
 *  Create a module section. This writes all classes, functions, variables and blocks contained by the module.
 */
int ecCreateModuleSection(EcCompiler *cp)
{
    Ejs         *ejs;
    EjsConst    *constants;
    EjsModule   *mp;
    EcState     *state;
    MprBuf      *buf;
    int         rc;

    state = cp->state;
    buf = state->code->buf;
    mp = state->currentModule;

    mprLog(cp, 5, "Create module section %s", mp->name);

    ejs = cp->ejs;
    constants = mp->constants;

    rc = 0;
    rc += ecEncodeByte(cp, EJS_SECT_MODULE);
    rc += ecEncodeString(cp, mp->name);
    rc += ecEncodeNumber(cp, mp->version);

    /*
     *  Remember this location for the module checksum. Reserve 4 bytes.
     */
    state->checksumOffset = mprGetBufEnd(buf) - buf->data;
    mprAdjustBufEnd(buf, 4);

    /*
     *  Write the constant pool and lock it against further updates.
     */
    mp->constants->locked = 1;
    rc += ecEncodeNumber(cp, constants->len);
    rc += ecEncodeBlock(cp, (uchar*) constants->pool, constants->len);

    if (createDependencySection(cp) < 0) {
        return EJS_ERR;
    }
    if (mp->hasInitializer) {
        if (createFunctionSection(cp, 0, -1, mp->initializer) < 0) {
            return EJS_ERR;
        }
    }
    if (createGlobalProperties(cp) < 0) {
        return EJS_ERR;
    }
    rc += ecEncodeByte(cp, EJS_SECT_MODULE_END);
    if (rc < 0) {
        return MPR_ERR_CANT_WRITE;
    }

    /*
     *  Update the checksum
     */
    mp->checksum += (sum(mp->name, 0) & EJS_ENCODE_MAX_WORD);
    ejsEncodeWord((uchar*) &buf->data[state->checksumOffset], mp->checksum);
    return 0;
}


static int createDependencySection(EcCompiler *cp)
{
    Ejs         *ejs;
    EjsModule   *module, *mp;
    int         rc, i, count, version;

    mp = cp->state->currentModule;
    mprAssert(mp);

    ejs = cp->ejs;

    /*
     *  If merging, don't need references to dependent modules as they are aggregated onto the output
     */
    if (mp->dependencies && !cp->merge) {
        count = mprGetListCount(mp->dependencies);
        for (i = 0; i < count; i++) {
            module = (EjsModule*) mprGetItem(mp->dependencies, i);

            if (module->compiling) {
                continue;
            }
            if (strcmp(mp->name, module->name) == 0) {
                /* A module can't depend on itself */
                continue;
            }

            rc = 0;
            rc += ecEncodeByte(cp, EJS_SECT_DEPENDENCY);
            rc += ecEncodeString(cp, module->name);
            rc += ecEncodeNumber(cp, module->checksum);

            if (!cp->bind) {
                rc += ecEncodeNumber(cp, module->minVersion);
                rc += ecEncodeNumber(cp, module->maxVersion);
            } else {
                version = EJS_MAKE_COMPAT_VERSION(module->version);
                rc += ecEncodeNumber(cp, version);
                rc += ecEncodeNumber(cp, version);
            }
            if (rc < 0) {
                return MPR_ERR_CANT_WRITE;
            }
            mp->checksum += sum(module->name, 0);
            mprLog(cp, 5, "    dependency section for %s from module %s", module->name, mp->name);
        }
    }
    return 0;
}


/*
 *  Emit all global classes, functions, variables and blocks.
 */
static int createGlobalProperties(EcCompiler *cp)
{
    Ejs             *ejs;
    EjsName         *prop;
    EjsModule       *mp;
    EjsVar          *vp;
    int             next, slotNum;

    ejs = cp->ejs;
    mp = cp->state->currentModule;

    if (mp->globalProperties == 0) {
        return 0;
    }

    for (next = 0; (prop = (EjsName*) mprGetNextItem(mp->globalProperties, &next)) != 0; ) {
        slotNum = ejsLookupProperty(ejs, ejs->global, prop);
        if (slotNum < 0) {
            mprError(ejs, "Code generation error. Can't find global property %s.", prop->name);
            return EJS_ERR;
        }
        vp = ejsGetProperty(ejs, ejs->global, slotNum);
        if (vp->visited) {
            continue;
        }
        if (ejsIsType(vp)) {
            if (createGlobalType(cp, (EjsType*) vp) < 0) {
                return EJS_ERR;
            }
        } else {
            if (createSection(cp, ejs->global, slotNum) < 0) {
                return EJS_ERR;
            }
        }
    }
    for (next = 0; (prop = (EjsName*) mprGetNextItem(mp->globalProperties, &next)) != 0; ) {
        slotNum = ejsLookupProperty(ejs, ejs->global, prop);
        vp = ejsGetProperty(ejs, ejs->global, slotNum);
        vp->visited = 0;
    }
    return 0;
}


/*
 *  Recursively emit a class and its base classes
 */
static int createGlobalType(EcCompiler *cp, EjsType *type)
{
    Ejs             *ejs;
    EjsModule       *mp;
    EjsType         *iface;
    int             slotNum, next;

    ejs = cp->ejs;
    mp = cp->state->currentModule;

    if (type->block.obj.var.visited || type->module != mp) {
        return 0;
    }
    type->block.obj.var.visited = 1;

    if (type->baseType && !type->baseType->block.obj.var.visited) {
        createGlobalType(cp, type->baseType);
    }

    if (type->implements) {
        for (next = 0; (iface = mprGetNextItem(type->implements, &next)) != 0; ) {
            createGlobalType(cp, iface);
        }
    }

    slotNum = ejsLookupProperty(ejs, ejs->global, &type->qname);
    mprAssert(slotNum >= 0);

    if (createSection(cp, ejs->global, slotNum) < 0) {
        return EJS_ERR;
    }
    return 0;
}


static int createSection(EcCompiler *cp, EjsVar *block, int slotNum)
{
    Ejs         *ejs;
    EjsTrait    *trait;
    EjsName     qname;
    EjsVar      *vp;

    ejs = cp->ejs;
    vp = ejsGetProperty(ejs, (EjsVar*) block, slotNum);
    qname = ejsGetPropertyName(ejs, block, slotNum);
    trait = ejsGetPropertyTrait(ejs, block, slotNum);

    /*
     *  hoistBlockVar will delete hoisted properties but will not (yet) compact to reclaim the slot.
     */
    if (slotNum < 0 || trait == 0 || vp == 0 || qname.name[0] == '\0') {
        return 0;
    }

    mprAssert(qname.name);

    if (ejsIsType(vp)) {
        return createClassSection(cp, block, slotNum, vp);

    } else if (ejsIsFunction(vp)) {
        return createFunctionSection(cp, block, slotNum, (EjsFunction*) vp);

    } else if (ejsIsBlock(vp)) {
        return createBlockSection(cp, block, slotNum, (EjsBlock*) vp);
    }
    return createPropertySection(cp, block, slotNum, vp);
}


/*
 *  Create a type section in the module file.
 */
static int createClassSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsVar *klass)
{
    Ejs             *ejs;
    EjsModule       *mp;
    EjsType         *type, *iface;
    EjsBlock        *instanceBlock;
    EjsTrait        *trait;
    EjsFunction     *fun;
    EjsName         qname, pname;
    int             next, i, rc, attributes, interfaceCount, instanceTraits;

    ejs = cp->ejs;
    mp = cp->state->currentModule;

    trait = ejsGetPropertyTrait(ejs, ejs->global, slotNum);
#if BLD_FEATURE_EJS_DOC
    createDocSection(cp, ejs->global, slotNum, trait);
#endif
    qname = ejsGetPropertyName(ejs, ejs->global, slotNum);
    mprAssert(qname.name);

    mprLog(cp, 5, "    type section %s for module %s", qname.name, mp->name);

    type = (EjsType*) ejsGetProperty(ejs, ejs->global, slotNum);
    mprAssert(type);
    mprAssert(ejsIsType(type));

    rc = 0;
    rc += ecEncodeByte(cp, EJS_SECT_CLASS);
    rc += ecEncodeString(cp, qname.name);
    rc += ecEncodeString(cp, qname.space);

    attributes = (trait) ? trait->attributes : 0;
    attributes &= ~(EJS_ATTR_BLOCK_HELPERS | EJS_ATTR_OBJECT_HELPERS | EJS_ATTR_SLOTS_NEED_FIXUP);

    if (type->hasStaticInitializer) {
        attributes |= EJS_ATTR_HAS_STATIC_INITIALIZER;
    }
    if (type->hasConstructor) {
        attributes |= EJS_ATTR_HAS_CONSTRUCTOR;
    }
    if (type->hasInitializer) {
        attributes |= EJS_ATTR_HAS_INITIALIZER;
    }
    if (type->callsSuper) {
        attributes |= EJS_ATTR_CALLS_SUPER;
    }
    if (type->block.obj.var.native) {
        attributes |= EJS_ATTR_NATIVE;

    } else if (type->implements) {
        for (next = 0; (iface = mprGetNextItem(type->implements, &next)) != 0; ) {
            if (!iface->isInterface) {
                attributes |= EJS_ATTR_SLOTS_NEED_FIXUP;
                break;
            }
        }
    }
    rc += ecEncodeNumber(cp, attributes);
    rc += ecEncodeNumber(cp, (cp->bind) ? slotNum: -1);

    mprAssert(type != type->baseType);
    rc += ecEncodeGlobal(cp, (EjsVar*) type->baseType, &type->baseType->qname);
    rc += ecEncodeNumber(cp, type->block.numTraits);

    instanceTraits = (type->instanceBlock) ? type->instanceBlock->numTraits: 0;
    rc += ecEncodeNumber(cp, instanceTraits);
    
    interfaceCount = (type->implements) ? mprGetListCount(type->implements) : 00;
    rc += ecEncodeNumber(cp, interfaceCount);

    if (type->implements) {
        for (next = 0; (iface = mprGetNextItem(type->implements, &next)) != 0; ) {
            rc += ecEncodeGlobal(cp, (EjsVar*) iface, &iface->qname);
        }
    }
    if (rc < 0) {
        return MPR_ERR_CANT_WRITE;
    }

    /*
     *  Loop over type traits
     */
    for (i = 0; i < type->block.numTraits; i++) {

        pname = ejsGetPropertyName(ejs, (EjsVar*) type, i);
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) type, i);
        if (trait == 0) {
            continue;
        }
        if (i < type->block.numInherited) {
            /*
             *  Skip inherited and implemented functions that are not overridden. We must emit overridden functions so
             *  the loader will create a unique function defintion for the overridden method.
             */
            fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, i);
            if (fun == 0 || !fun->block.obj.var.isFunction || !fun->override) {
                continue;
            }
            if (trait->attributes & EJS_ATTR_INHERITED) {
                continue;
            }
        }
        if (createSection(cp, (EjsVar*) type, i) < 0) {
            return rc;
        }
    }

    /*
     *  Loop over non-inherited instance properties. This skips implemented and inherited properties. They will be 
     *  copied by the loader when the module is loaded.
     */
    instanceBlock = type->instanceBlock;
    if (instanceBlock) {
        for (slotNum = instanceBlock->numInherited; slotNum < instanceBlock->numTraits; slotNum++) {
            pname = ejsGetPropertyName(ejs, (EjsVar*) instanceBlock, slotNum);
            if (createSection(cp, (EjsVar*) instanceBlock, slotNum) < 0) {
                return rc;
            }
        }
    }
    mp->checksum += sum(type->qname.name, slotNum + type->block.numTraits + instanceTraits + interfaceCount);

    if (ecEncodeByte(cp, EJS_SECT_CLASS_END) < 0) {
        return EJS_ERR;
    }
    return 0;
}


/*
 *  NOTE: static methods and methods are both stored in the typeTraits.
 *  The difference is in how the methods are called by the VM op codes.
 */
static int createFunctionSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsFunction *fun)
{
    Ejs             *ejs;
    EjsModule       *mp;
    EjsTrait        *trait;
    EjsName         qname;
    EjsCode         *code;
    EjsType         *resultType;
    int             rc, i, attributes;

    mprAssert(fun);

    rc = 0;
    mp = cp->state->currentModule;
    ejs = cp->ejs;
    block = fun->owner;
    slotNum = fun->slotNum;

    code = &fun->body.code;
    mprAssert(code);

    if (block && slotNum >= 0) {
        trait = ejsGetPropertyTrait(ejs, block, slotNum);
#if BLD_FEATURE_EJS_DOC
        createDocSection(cp, block, slotNum, trait);
#endif
        qname = ejsGetPropertyName(ejs, block, slotNum);
        attributes = trait->attributes;
        if (fun->isInitializer) {
            attributes |= EJS_ATTR_INITIALIZER;
        }

    } else {
        attributes = EJS_ATTR_INITIALIZER;
        qname.name = EJS_INITIALIZER_NAME;
        qname.space = EJS_INTRINSIC_NAMESPACE;
    }
    rc += ecEncodeByte(cp, EJS_SECT_FUNCTION);
    rc += ecEncodeString(cp, qname.name);
    rc += ecEncodeString(cp, qname.space);

    /*
     *  Map -1 to 0. Won't matter as it can't be a getter/setter
     */
    rc += ecEncodeNumber(cp, fun->nextSlot < 0 ? 0 : fun->nextSlot);

    if (fun->constructor) {
        attributes |= EJS_ATTR_CONSTRUCTOR;
    }
    if (fun->rest) {
        attributes |= EJS_ATTR_REST;
    }
    if (fun->fullScope) {
        attributes |= EJS_ATTR_FULL_SCOPE;
    }
    if (fun->hasReturn) {
        attributes |= EJS_ATTR_HAS_RETURN;
    }

    rc += ecEncodeNumber(cp, attributes);
    rc += ecEncodeByte(cp, fun->lang);

    resultType = fun->resultType;
    rc += ecEncodeGlobal(cp, (EjsVar*) resultType, (resultType) ? &resultType->qname : 0);

    rc += ecEncodeNumber(cp, (cp->bind || (block != ejs->global)) ? slotNum: -1);
    rc += ecEncodeNumber(cp, fun->numArgs);
    rc += ecEncodeNumber(cp, fun->block.obj.numProp - fun->numArgs);
    rc += ecEncodeNumber(cp, code->numHandlers);

    /*
     *  Output the code
     */
    rc += ecEncodeNumber(cp, code->codeLen);
    if (code->codeLen > 0) {
        rc += ecEncodeBlock(cp, code->byteCode, code->codeLen);
    }
    if (code->numHandlers > 0) {
        rc += createExceptionSection(cp, fun);
    }

    /*
     *  Recursively write args, locals and any nested functions and blocks.
     */
    attributes = 0;
    for (i = 0; i < fun->block.obj.numProp; i++) {
        createSection(cp, (EjsVar*) fun, i);
    }
    if (ecEncodeByte(cp, EJS_SECT_FUNCTION_END) < 0) {
        return EJS_ERR;
    }
    mp->checksum += sum(qname.name, slotNum + fun->numArgs + fun->block.obj.numProp - fun->numArgs + code->numHandlers);
    return rc;
}


/*
 *  NOTE: static methods and methods are both stored in the typeTraits.
 *  The difference is in how the methods are called by the VM op codes.
 */
static int createExceptionSection(EcCompiler *cp, EjsFunction *fun)
{
    Ejs         *ejs;
    EjsEx       *ex;
    EjsModule   *mp;
    int         rc, i;

    mprAssert(fun);

    rc = 0;
    mp = cp->state->currentModule;
    ejs = cp->ejs;

    rc += ecEncodeByte(cp, EJS_SECT_EXCEPTION);

    for (i = 0; i < fun->body.code.numHandlers; i++) {
        ex = fun->body.code.handlers[i];
        rc += ecEncodeByte(cp, ex->flags);
        rc += ecEncodeNumber(cp, ex->tryStart);
        rc += ecEncodeNumber(cp, ex->tryEnd);
        rc += ecEncodeNumber(cp, ex->handlerStart);
        rc += ecEncodeNumber(cp, ex->handlerEnd);
        rc += ecEncodeNumber(cp, ex->numBlocks);
        rc += ecEncodeNumber(cp, ex->numStack);
        rc += ecEncodeGlobal(cp, (EjsVar*) ex->catchType, ex->catchType ? &ex->catchType->qname : 0);
        // mp->checksum += sum(NULL, ex->tryStart + ex->tryEnd);
    }
    return rc;
}


static int createBlockSection(EcCompiler *cp, EjsVar *parent, int slotNum, EjsBlock *block)
{
    Ejs             *ejs;
    EjsModule       *mp;
    EjsName         qname;
    int             i, rc;

    ejs = cp->ejs;
    mp = cp->state->currentModule;

    if (ecEncodeByte(cp, EJS_SECT_BLOCK) < 0) {
        return MPR_ERR_CANT_WRITE;
    }

    qname = ejsGetPropertyName(ejs, parent, slotNum);
    if (ecEncodeString(cp, qname.name) < 0) {
        return MPR_ERR_CANT_WRITE;
    }
    rc = 0;
    rc += ecEncodeNumber(cp, (cp->bind || (block != ejs->globalBlock)) ? slotNum : -1);
    rc += ecEncodeNumber(cp, block->obj.numProp);
    if (rc < 0) {
        return MPR_ERR_CANT_WRITE;
    }

    /*
     *  Now emit all the properties
     */
    for (i = 0; i < block->obj.numProp; i++) {
        createSection(cp, (EjsVar*) block, i);
    }

    if (ecEncodeByte(cp, EJS_SECT_BLOCK_END) < 0) {
        return EJS_ERR;
    }
    // mp->checksum += sum(qname.name, block->obj.numProp + slotNum);
    return 0;
}


static int createPropertySection(EcCompiler *cp, EjsVar *block, int slotNum, EjsVar *vp)
{
    Ejs         *ejs;
    EjsTrait    *trait;
    EjsName     qname;
    EjsModule   *mp;
    int         rc, attributes;

    ejs = cp->ejs;
    mp = cp->state->currentModule;

    trait = ejsGetPropertyTrait(ejs, block, slotNum);
    qname = ejsGetPropertyName(ejs, block, slotNum);
    
    mprAssert(qname.name[0] != '\0');
    attributes = trait->attributes;

#if BLD_FEATURE_EJS_DOC
    createDocSection(cp, block, slotNum, trait);
#endif

    mprLog(cp, 5, "    global property section %s", qname.name);

    if (trait->type) {
        if (trait->type == ejs->namespaceType || (cp->empty && (strcmp(trait->type->qname.name, "Namespace") == 0))) {
            attributes |= EJS_ATTR_HAS_VALUE;
        }
    }
    rc = 0;
    rc += ecEncodeByte(cp, EJS_SECT_PROPERTY);
    rc += ecEncodeName(cp, &qname);

    rc += ecEncodeNumber(cp, attributes);
    rc += ecEncodeNumber(cp, (cp->bind || (block != ejs->global)) ? slotNum : -1);
    rc += ecEncodeGlobal(cp, (EjsVar*) trait->type, trait->type ? &trait->type->qname : 0);

    if (attributes & EJS_ATTR_HAS_VALUE) {
        if (vp && ejsIsNamespace(vp)) {
            rc += ecEncodeString(cp, ((EjsNamespace*) vp)->name);
        } else {
            rc += ecEncodeString(cp, 0);
        }
    }
    mp->checksum += sum(qname.name, slotNum);
    return rc;
}


#if BLD_FEATURE_EJS_DOC
static int createDocSection(EcCompiler *cp, EjsVar *block, int slotNum, EjsTrait *trait)
{
    Ejs         *ejs;
    EjsName     qname;
    EjsDoc      *doc;
    char        key[32];

    ejs = cp->ejs;
    
    if (trait == 0 || !(ejs->flags & EJS_FLAG_DOC)) {
        return 0;
    }

    if (ejs->doc == 0) {
        ejs->doc = mprCreateHash(ejs, EJS_DOC_HASH_SIZE);
    }
    if (slotNum < 0) {
        mprAssert(ejsIsBlock(block));
        slotNum = trait - ((EjsBlock*) block)->traits;
    }
    mprAssert(slotNum >= 0);
    mprSprintf(key, sizeof(key), "%Lx %d", PTOL(block), slotNum);
    doc = (EjsDoc*) mprLookupHash(ejs->doc, key);
    if (doc == 0) {
        return 0;
    }

    qname = ejsGetPropertyName(ejs, block, slotNum);
    mprAssert(qname.name);

    mprLog(cp, 5, "Create doc section for %s::%s", qname.space, qname.name);

    if (ecEncodeByte(cp, EJS_SECT_DOC) < 0) {
        return MPR_ERR_CANT_WRITE;
    }
    if (ecEncodeString(cp, doc->docString) < 0) {
        return MPR_ERR_CANT_WRITE;
    }
    return 0;
}
#endif /* BLD_FEATURE_EJS_DOC */


/****************************** Constant Management ***************************/
/*
 *  Add a constant to the constant pool. Grow if required and return the
 *  constant string offset into the pool.
 */
int ecAddConstant(EcCompiler *cp, cchar *str)
{
    int    offset;

    mprAssert(cp);

    if (str) {
        offset = ecAddModuleConstant(cp, cp->state->currentModule, str);
        if (offset < 0) {
            cp->fatalError = 1;
            mprAssert(offset > 0);
            return EJS_ERR;
        }

    } else {
        offset = 0;
    }
    return offset;
}


int ecAddNameConstant(EcCompiler *cp, EjsName *qname)
{
    if (ecAddConstant(cp, qname->name) < 0 || ecAddConstant(cp, qname->space) < 0) {
        return EJS_ERR;
    }
    return 0;
}


void ecAddFunctionConstants(EcCompiler *cp, EjsFunction *fun)
{
    if (fun->resultType) {
        ecAddNameConstant(cp, &fun->resultType->qname);
    }

#if BLD_FEATURE_EJS_DOC
    if (cp->ejs->flags & EJS_FLAG_DOC) {
        ecAddDocConstant(cp, 0, fun->owner, fun->slotNum);
    }
#endif

    ecAddBlockConstants(cp, (EjsBlock*) fun);
}


void ecAddBlockConstants(EcCompiler *cp, EjsBlock *block)
{
    Ejs         *ejs;
    EjsName     qname;
    EjsTrait    *trait;
    EjsVar      *vp;
    int         i;

    ejs = cp->ejs;

    for (i = 0; i < block->numTraits; i++) {
        qname = ejsGetPropertyName(ejs, (EjsVar*) block, i);
        ecAddNameConstant(cp, &qname);
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) block, i);
        if (trait && trait->type) {
            ecAddNameConstant(cp, &trait->type->qname);
        }
        vp = ejsGetProperty(ejs, (EjsVar*) block, i);
        if (ejsIsFunction(vp)) {
            ecAddFunctionConstants(cp, (EjsFunction*) vp);
        } else if (ejsIsBlock(vp)) {
            ecAddBlockConstants(cp, (EjsBlock*) vp);
        }
    }
}


#if BLD_FEATURE_EJS_DOC
/*
 *  Allow 2 methods to get the doc string: by trait and by block:slotNum
 */
int ecAddDocConstant(EcCompiler *cp, EjsTrait *trait, EjsVar *block, int slotNum)
{
    Ejs         *ejs;
    EjsDoc      *doc;
    char        key[32];

    ejs = cp->ejs;

    if (trait == 0 && slotNum >= 0) {
        trait = ejsGetPropertyTrait(cp->ejs, block, slotNum);
    }
    if (trait) {
        if (ejs->doc == 0) {
            ejs->doc = mprCreateHash(ejs, EJS_DOC_HASH_SIZE);
        }
        if (slotNum < 0) {
            mprAssert(ejsIsBlock(block));
            slotNum = trait - ((EjsBlock*) block)->traits;
        }
        mprAssert(slotNum >= 0);
        mprSprintf(key, sizeof(key), "%Lx %d", PTOL(block), slotNum);
        doc = (EjsDoc*) mprLookupHash(ejs->doc, key);
        if (doc && doc->docString) {
            if (ecAddConstant(cp, doc->docString) < 0) {
                return EJS_ERR;
            }
        }
    }
    return 0;
}
#endif


/*
 *  Add a constant and encode the offset.
 */
int ecAddModuleConstant(EcCompiler *cp, EjsModule *mp, cchar *str)
{
    Ejs         *ejs;
    EjsConst    *constants;
    MprHash     *sp;
    int         len, oldLen, size;

    mprAssert(mp);

    if (str == 0) {
        /* Just ignore null names */
        return 0;
    }

    ejs = cp->ejs;
    mprAssert(ejs);
    constants = mp->constants;

    /*
     *  Maintain a symbol table for quick location
     */
    sp = mprLookupHashEntry(constants->table, str);
    if (sp != 0) {
        return PTOI(sp->data);
    }

    if (constants->locked) {
        mprError(ejs, "Constant pool for module %s is locked. Can't add \"%s\", try another module name.", 
            mp->name, str);
        cp->fatalError = 1;
        return MPR_ERR_CANT_CREATE;
    }

    /*
     *  First string always starts at 1.
     */
    if (constants->len == 0) {
        constants->len = 1;
        constants->size = EC_BUFSIZE;
        constants->pool = (char*) mprAllocZeroed(constants, constants->size);
        if (constants->pool == 0) {
            cp->fatalError = 1;
            return MPR_ERR_CANT_CREATE;
        }
    }
    oldLen = constants->len;

    /*
     *  Add one for the null
     */
    len = (int) strlen(str) + 1;

    if ((oldLen + len) >= constants->size) {
        size = constants->size + len;
        size = (size + EC_BUFSIZE) / EC_BUFSIZE * EC_BUFSIZE;
        constants->pool = (char*) mprRealloc(constants, constants->pool, size);
        if (constants->pool == 0) {
            cp->fatalError = 1;
            return MPR_ERR_CANT_CREATE;
        }
        memset(&constants->pool[constants->size], 0, size - constants->size);
        constants->size = size;
    }

    mprStrcpy(&constants->pool[oldLen], len, str);
    constants->len += len;
    mprAddHash(constants->table, str, ITOP(oldLen));
    return oldLen;
}

/****************************** Value Emitters ********************************/
/*
 *  Emit an encoded string ored with flags. The name index is shifted by 2.
 */
static int encodeTypeName(EcCompiler *cp, cchar *name, int flags)
{
    int        offset;

    mprAssert(name && *name);

    offset = ecAddModuleConstant(cp, cp->state->currentModule, name);
    if (offset < 0) {
        cp->fatalError = 1;
        mprAssert(offset > 0);
        return EJS_ERR;
    }
    return ecEncodeNumber(cp, offset << 2 | flags);
}


/*
 *  Encode a global variable (usually a type). The encoding is untyped: 0, bound type: slot number, unbound or 
 *  unresolved type: name.
 */
int ecEncodeGlobal(EcCompiler *cp, EjsVar *obj, EjsName *qname)
{
    Ejs         *ejs;
    int         slotNum;

    ejs = cp->ejs;
    slotNum = -1;

    if (obj == 0) {
        ecEncodeNumber(cp, EJS_ENCODE_GLOBAL_NOREF);
        return 0;
    }

    /*
     *  If binding globals, we can encode the slot number of the type.
     */
    if (obj->builtin || cp->bind) {
        slotNum = ejsLookupProperty(ejs, ejs->global, qname);
        if (slotNum >= 0) {
            ecEncodeNumber(cp, (slotNum << 2) | EJS_ENCODE_GLOBAL_SLOT);
            return 0;
        }
    }

    /*
     *  So here we encode the type name and namespace name.
     */
    encodeTypeName(cp, qname->name, EJS_ENCODE_GLOBAL_NAME);
    ecEncodeString(cp, qname->space);
    return 0;
}


/**************************** Value Encoding Routines *************************/
/*
 *  Reserve a small amount of room sufficient for the next encoding
 */
static int reserveRoom(EcCompiler *cp, int room)
{
    EcCodeGen       *code;

    code = cp->state->code;
    mprAssert(code);

    if (mprGetBufSpace(code->buf) < room) {
        if (mprGrowBuf(code->buf, -1) < 0) {
            cp->fatalError = 1;
            cp->memError = 1;
            mprAssert(0);
            return MPR_ERR_NO_MEMORY;
        }
    }
    return 0;
}


/*
 *  Encode an Ejscript instruction operation code
 */
int ecEncodeOpcode(EcCompiler *cp, int code)
{
    mprAssert(code < 240);
    mprAssert(cp);

    cp->lastOpcode = code;
    return ecEncodeByte(cp, code);
}


/*
 *  Encode a <name><namespace> pair
 */
int ecEncodeName(EcCompiler *cp, EjsName *qname)
{
    int     rc;

    mprAssert(qname->name);

    rc = 0;
    rc += ecEncodeString(cp, qname->name);
    rc += ecEncodeString(cp, qname->space);
    return rc;

}


int ecEncodeString(EcCompiler *cp, cchar *str)
{
    int    offset;

    mprAssert(cp);

    if (str) {
        offset = ecAddModuleConstant(cp, cp->state->currentModule, str);
        if (offset < 0) {
            cp->error = 1;
            cp->fatalError = 1;
            return EJS_ERR;
        }

    } else {
        offset = 0;
    }
    return ecEncodeNumber(cp, offset);
}


int ecEncodeByte(EcCompiler *cp, int value)
{
    EcCodeGen   *code;
    uchar       *pc;

    mprAssert(cp);
    code = cp->state->code;

    if (reserveRoom(cp, sizeof(uchar)) < 0) {
        mprAssert(0);
        return EJS_ERR;
    }

    pc = (uchar*) mprGetBufEnd(code->buf);
    *pc++ = value;
    mprAdjustBufEnd(code->buf, sizeof(uchar));
    return 0;
}


int ecEncodeNumber(EcCompiler *cp, int64 number)
{
    MprBuf      *buf;
    int         len;

    mprAssert(cp);
    buf = cp->state->code->buf;
    if (reserveRoom(cp, sizeof(int64) + 2) < 0) {
        mprAssert(0);
        return EJS_ERR;
    }
    len = ejsEncodeNum((uchar*) mprGetBufEnd(buf), number);
    mprAdjustBufEnd(buf, len);
    return 0;
}


#if BLD_FEATURE_FLOATING_POINT
int ecEncodeDouble(EcCompiler *cp, double value)
{
    MprBuf      *buf;
    int         len;

    mprAssert(cp);
    buf = cp->state->code->buf;
    if (reserveRoom(cp, sizeof(double) + 4) < 0) {
        mprAssert(0);
        return EJS_ERR;
    }
    len = ejsEncodeDouble((uchar*) mprGetBufEnd(buf), value);
    mprAdjustBufEnd(buf, len);
    return 0;
}
#endif


/*
 *  Encode a 32-bit number. Always emit exactly 4 bytes.
 */
int ecEncodeWord(EcCompiler *cp, int number)
{
    MprBuf      *buf;
    int         len;

    mprAssert(cp);
    buf = cp->state->code->buf;

    if (reserveRoom(cp, sizeof(int) / sizeof(char)) < 0) {
        mprAssert(0);
        return EJS_ERR;
    }
    len = ejsEncodeWord((uchar*) mprGetBufEnd(buf), number);
    mprAssert(len == 4);
    mprAdjustBufEnd(buf, len);
    return 0;
}


int ecEncodeByteAtPos(EcCompiler *cp, uchar *pos, int value)
{
    return ejsEncodeByteAtPos(pos, value);
}


int ecEncodeWordAtPos(EcCompiler *cp, uchar *pos, int value)
{
    if (abs(value) > EJS_ENCODE_MAX_WORD) {
        mprError(cp, "Code generation error. Word %d exceeds maximum %d", value, EJS_ENCODE_MAX_WORD);
        return -1;
    }
    return ejsEncodeWordAtPos(pos, value);
}


int ecEncodeBlock(EcCompiler *cp, uchar *buf, int len)
{
    EcCodeGen   *code;

    code = cp->state->code;

    if (reserveRoom(cp, len) < 0) {
        mprAssert(0);
        return EJS_ERR;
    }
    if (mprPutBlockToBuf(code->buf, (char*) buf, len) != len) {
        cp->fatalError = 1;
        cp->memError = 1;
        mprAssert(0);
        return EJS_ERR;
    }
    return 0;
}


uint ecGetCodeOffset(EcCompiler *cp)
{
    EcCodeGen   *code;

    code = cp->state->code;
    return (uint) ((uchar*) mprGetBufEnd(code->buf) - (uchar*) mprGetBufStart(code->buf));
}


int ecGetCodeLen(EcCompiler *cp, uchar *mark)
{
    EcCodeGen   *code;

    code = cp->state->code;
    return (int) (((uchar*) mprGetBufEnd(code->buf)) - mark);
}


/*
 *  Copy the code at "pos" of length "size" the distance specified by "dist". Dist may be postitive or negative.
 */
void ecCopyCode(EcCompiler *cp, uchar *pos, int size, int dist)
{
    mprMemcpy((char*) &pos[dist], size, (char*) pos, size);
}


void ecAdjustCodeLength(EcCompiler *cp, int adj)
{
    EcCodeGen   *code;

    code = cp->state->code;
    mprAdjustBufEnd(code->buf, adj);
}


static int swapShort(EcCompiler *cp, int word)
{
    if (mprGetEndian(cp) == MPR_LITTLE_ENDIAN) {
        return word;
    }
    word = ((word & 0xFFFF) << 16) | ((word & 0xFFFF0000) >> 16);
    return ((word & 0xFF) << 8) | ((word & 0xFF00) >> 8);
}


static int swapWord(EcCompiler *cp, int word)
{
    if (mprGetEndian(cp) == MPR_LITTLE_ENDIAN) {
        return word;
    }
    return ((word & 0xFF000000) >> 24) | ((word & 0xFF0000) >> 8) | ((word & 0xFF00) << 8) | ((word & 0xFF) << 24);
}

/*
 *  Simple checksum of name and slots. Not meant to be rigorous.
 */
static int sum(cchar *name, int value)
{
    cchar    *cp;
    int     checksum;

    checksum = value;
    if (name) {
        for (cp = name; *cp; cp++) {
            checksum += *cp;
        }
    }
    return checksum;
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
