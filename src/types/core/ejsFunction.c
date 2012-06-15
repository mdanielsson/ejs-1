/**
 *  ejsFunction.c - Function class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/******************************************************************************/
/*
 *  Create a function object.
 */
static EjsFunction *createFunction(Ejs *ejs, EjsType *type, int numSlots)
{
    EjsFunction     *fun;

    /*
     *  Note: Functions are not pooled, frames are.
     */
    fun = (EjsFunction*) ejsCreateObject(ejs, ejs->functionType, 0);
    if (fun == 0) {
        return 0;
    }
    fun->block.obj.var.isFunction = 1;
    fun->slotNum = -1;
    fun->nextSlot = -1;
    return fun;
}


/*
 *  Cast the operand to the specified type
 *
 *  intrinsic function cast(type: Type) : Object
 */
static EjsVar *castFunction(Ejs *ejs, EjsFunction *vp, EjsType *type)
{
    switch (type->id) {
    case ES_String:
        return (EjsVar*) ejsCreateString(ejs, "[function Function]");

    case ES_Number:
        return (EjsVar*) ejs->nanValue;

    case ES_Boolean:
        return (EjsVar*) ejs->trueValue;
            
    default:
        ejsThrowTypeError(ejs, "Can't cast type \"%s\"", type->qname.name);
        return 0;
    }
    return 0;
}


static EjsFunction *cloneFunctionVar(Ejs *ejs, EjsFunction *src, bool deep)
{
    EjsFunction     *dest;

    dest = (EjsFunction*) ejsCopyBlock(ejs, &src->block, deep);
    if (dest == 0) {
        return 0;
    }
    dest->properties = src->properties;
    dest->prototype = src->prototype;

    dest->body.code = src->body.code;
    dest->resultType = src->resultType;
    dest->thisObj = src->thisObj;
    dest->owner = src->owner;
    dest->slotNum = src->slotNum;
    dest->numArgs = src->numArgs;
    dest->numDefault = src->numDefault;
    dest->nextSlot = src->nextSlot;

    /*
     *  OPT
     */
    dest->getter = src->getter;
    dest->setter = src->setter;
    dest->staticMethod = src->staticMethod;
    dest->constructor = src->constructor;
    dest->hasReturn = src->hasReturn;
    dest->isInitializer = src->isInitializer;
    dest->literalGetter = src->literalGetter;
    dest->override = src->override;
    dest->rest = src->rest;
    dest->fullScope = src->fullScope;
    dest->nativeProc = src->nativeProc;
    dest->lang = src->lang;
    dest->isFrame = src->isFrame;
    return dest;
}


static void destroyFunction(Ejs *ejs, EjsFunction *fun)
{
    ejsFreeVar(ejs, (EjsVar*) fun, fun->isFrame ? ES_Frame : ES_Function);
}

void ejsMarkFunction(Ejs *ejs, EjsVar *parent, EjsFunction *fun)
{
    ejsMarkBlock(ejs, parent, (EjsBlock*) fun);

    if (fun->prototype) {
        ejsMarkVar(ejs, parent, (EjsVar*) fun->prototype);
    }
    if (fun->properties) {
        ejsMarkVar(ejs, parent, (EjsVar*) fun->properties);
    }
    if (fun->owner) {
        ejsMarkVar(ejs, parent, fun->owner);
    }
    if (fun->thisObj) {
        ejsMarkVar(ejs, parent, fun->thisObj);
    }
    if (fun->isFrame) {
        ejsMarkVar(ejs, parent, (EjsObj*) ((EjsFrame*) fun)->caller);
    }
}


static EjsVar *applyFunction(Ejs *ejs, EjsFunction *fun, int argc, EjsVar **argv)
{
    EjsArray        *args;
    EjsVar          *save, *result;
    
    mprAssert(argc == 2);
    args = (EjsArray*) argv[1];
    mprAssert(ejsIsArray(args));

    save = fun->thisObj;
    fun->thisObj = 0;

    result =  ejsRunFunction(ejs, fun, argv[0], args->length, args->data);

    fun->thisObj = save;
    return result;
}


static EjsVar *callFunctionMethod(Ejs *ejs, EjsFunction *fun, int argc, EjsVar **argv)
{
    return applyFunction(ejs, fun, argc, argv);
}


static EjsVar *getObj(Ejs *ejs, EjsFunction *fun)
{
    //  OPT. Compiler could set loading?
    if ((ejs->flags & EJS_FLAG_COMPILER) || fun->loading || fun->isFrame) {
        return (EjsVar*) fun;
    }
    if (fun->properties == 0) {
        /* 
         *  On-Demand creation of Function.* property storage 
         */  
        fun->properties = ejsCreateSimpleObject(ejs);
    }
    return (EjsVar*) fun->properties;
}


static EjsVar *getFunctionProperty(Ejs *ejs, EjsFunction *fun, int slotNum)
{
    if ((ejs->flags & EJS_FLAG_COMPILER) || fun->loading || fun->isFrame) {
        return (ejs->objectHelpers->getProperty)(ejs, (EjsVar*) fun, slotNum);
    } else if (fun->properties) {
        return (ejs->objectHelpers->getProperty)(ejs, (EjsVar*) fun->properties, slotNum);
    }
    return 0;
}


/*
 *  Return the number of properties in the object
 */
static int getFunctionPropertyCount(Ejs *ejs, EjsFunction *fun)
{
    if ((ejs->flags & EJS_FLAG_COMPILER) || fun->loading || fun->isFrame) {
        return (ejs->objectHelpers->getPropertyCount)(ejs, (EjsVar*) fun);
    } else if (fun->properties) {
        return (ejs->objectHelpers->getPropertyCount)(ejs, (EjsVar*) fun->properties);
    }
    return 0;
}


static EjsName getFunctionPropertyName(Ejs *ejs, EjsFunction *fun, int slotNum)
{
    EjsName     qname;

    if ((ejs->flags & EJS_FLAG_COMPILER) || fun->loading || fun->isFrame || fun->properties == 0) {
        return (ejs->objectHelpers->getPropertyName)(ejs, (EjsVar*) fun, slotNum);
    } else {
        return (ejs->objectHelpers->getPropertyName)(ejs, (EjsVar*) fun->properties, slotNum);
    }
    qname.name = 0;
    qname.space = 0;
    return qname;
}


/*
 *  Lookup a property with a namespace qualifier in an object and return the slot if found. Return EJS_ERR if not found.
 */
static int lookupFunctionProperty(struct Ejs *ejs, EjsFunction *fun, EjsName *qname)
{
    int     slotNum;

    if ((ejs->flags & EJS_FLAG_COMPILER) || fun->loading || fun->isFrame) {
        slotNum = (ejs->objectHelpers->lookupProperty)(ejs, (EjsVar*) fun, qname);
    } else if (fun->properties) {
        slotNum = (ejs->objectHelpers->lookupProperty)(ejs, (EjsVar*) fun->properties, qname);
    } else {
        slotNum = -1;
    }
    return slotNum;
}


/**
 *  Set the value of a property.
 *  @param slot If slot is -1, then allocate the next free slot
 *  @return Return the property slot if successful. Return < 0 otherwise.
 */
static int setFunctionProperty(Ejs *ejs, EjsFunction *fun, int slotNum, EjsVar *value)
{
    return (ejs->objectHelpers->setProperty)(ejs, getObj(ejs, fun), slotNum, value);
}


/*
 *  Set the name for a property. Objects maintain a hash lookup for property names. This is hash is created on demand 
 *  if there are more than N properties. If an object is not dynamic, it will use the types name hash. If dynamic, 
 *  then the types name hash will be copied when required. Callers must supply persistent names strings in qname. 
 */
static int setFunctionPropertyName(Ejs *ejs, EjsFunction *fun, int slotNum, EjsName *qname)
{
    return (ejs->objectHelpers->setPropertyName)(ejs, getObj(ejs, fun), slotNum, qname);
}


static int deleteFunctionProperty(Ejs *ejs, EjsFunction *fun, int slotNum)
{
    return (ejs->objectHelpers->deleteProperty)(ejs, getObj(ejs, fun), slotNum);
}

/*************************************************************************************************************/
/*
 *  Create a script function. This defines the method traits. It does not create a  method slot. ResultType may
 *  be null to indicate untyped. NOTE: untyped functions may return a result at their descretion.
 */

EjsFunction *ejsCreateFunction(Ejs *ejs, cuchar *byteCode, int codeLen, int numArgs, int numExceptions, EjsType *resultType, 
        int attributes, EjsConst *constants, EjsBlock *scopeChain, int lang)
{
    EjsFunction     *fun;
    EjsCode         *code;

    fun = (EjsFunction*) ejsCreateVar(ejs, ejs->functionType, 0);
    if (fun == 0) {
        return 0;
    }

    if (scopeChain) {
        fun->block.scopeChain = scopeChain;
    }
    fun->numArgs = numArgs;
    fun->resultType = resultType;
    fun->numArgs = numArgs;
    fun->lang = lang;

    /*
     *  When functions are in object literals, we dely setting .getter until the object is actually created.
     *  This enables reading the function without running the getter in the VM.
     */
    if (attributes & EJS_ATTR_LITERAL_GETTER) {
        fun->literalGetter = 1;

    } else if (attributes & EJS_ATTR_GETTER) {
        fun->getter = 1;
    }
    if (attributes & EJS_ATTR_SETTER) {
        fun->setter = 1;
    }
    if (attributes & EJS_ATTR_CONSTRUCTOR) {
        fun->constructor = 1;
    }
    if (attributes & EJS_ATTR_REST) {
        fun->rest = 1;
    }
    if (attributes & EJS_ATTR_INITIALIZER) {
        fun->isInitializer = 1;
    }
    if (attributes & EJS_ATTR_STATIC) {
        fun->staticMethod = 1;
    }
    if (attributes & EJS_ATTR_OVERRIDE) {
        fun->override = 1;
    }
    if (attributes & EJS_ATTR_NATIVE) {
        fun->nativeProc = 1;
    }
    if (attributes & EJS_ATTR_FULL_SCOPE) {
        fun->fullScope = 1;
    }
    if (attributes & EJS_ATTR_HAS_RETURN) {
        fun->hasReturn = 1;
    }
    code = &fun->body.code;
    code->codeLen = codeLen;
    code->byteCode = (uchar*) byteCode;
    code->numHandlers = numExceptions;
    code->constants = constants;
    return fun;
}


void ejsSetNextFunction(EjsFunction *fun, int nextSlot)
{
    fun->nextSlot = nextSlot;
}


void ejsSetFunctionLocation(EjsFunction *fun, EjsVar *obj, int slotNum)
{
    mprAssert(fun);
    mprAssert(obj);

    fun->owner = obj;
    fun->slotNum = slotNum;
}


EjsEx *ejsAddException(EjsFunction *fun, uint tryStart, uint tryEnd, EjsType *catchType, uint handlerStart,
        uint handlerEnd, int numBlocks, int numStack, int flags, int preferredIndex)
{
    EjsEx           *exception;
    EjsCode         *code;
    int             size;

    mprAssert(fun);

    code = &fun->body.code;

    exception = mprAllocObjZeroed(fun, EjsEx);
    if (exception == 0) {
        mprAssert(0);
        return 0;
    }

    exception->flags = flags;
    exception->tryStart = tryStart;
    exception->tryEnd = tryEnd;
    exception->catchType = catchType;
    exception->handlerStart = handlerStart;
    exception->handlerEnd = handlerEnd;
    exception->numBlocks = numBlocks;
    exception->numStack = numStack;

    if (preferredIndex < 0) {
        preferredIndex = code->numHandlers++;
    }

    if (preferredIndex >= code->sizeHandlers) {
        size = code->sizeHandlers + EJS_EX_INC;
        code->handlers = (EjsEx**) mprRealloc(fun, code->handlers, (int) (size * sizeof(EjsEx)));
        if (code->handlers == 0) {
            mprAssert(0);
            return 0;
        }
        memset(&code->handlers[code->sizeHandlers], 0, EJS_EX_INC * sizeof(EjsEx)); 
        code->sizeHandlers = size;
    }
    code->handlers[preferredIndex] = exception;
    return exception;
}


void ejsOffsetExceptions(EjsFunction *fun, int offset)
{
    EjsEx           *ex;
    int             i;

    mprAssert(fun);

    for (i = 0; i < fun->body.code.numHandlers; i++) {
        ex = fun->body.code.handlers[i];
        ex->tryStart += offset;
        ex->tryEnd += offset;
        ex->handlerStart += offset;
        ex->handlerEnd += offset;
    }
}


/*
 *  Set the byte code for a script function
 */
int ejsSetFunctionCode(EjsFunction *fun, uchar *byteCode, int len)
{
    mprAssert(fun);
    mprAssert(byteCode);
    mprAssert(len >= 0);

    byteCode = (uchar*) mprMemdup(fun, byteCode, len);
    if (byteCode == 0) {
        return EJS_ERR;
    }
    fun->body.code.codeLen = len;
    mprFree(fun->body.code.byteCode);
    fun->body.code.byteCode = (uchar*) byteCode;
    return 0;
}


EjsFunction *ejsCopyFunction(Ejs *ejs, EjsFunction *src)
{
    return cloneFunctionVar(ejs, src, 0);
}


/*
 *  Allocate a new variable. Size is set to the extra bytes for properties in addition to the type's instance size.
 */
static EjsFrame *allocFrame(Ejs *ejs, int numSlots)
{
    EjsObject       *obj;
    uint            size;

    mprAssert(ejs);

    size = (int) (numSlots * sizeof(EjsVar*) + sizeof(EjsFrame));

    if ((obj = (EjsObject*) mprAllocZeroed(ejsGetAllocCtx(ejs), size)) == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    obj->var.type = ejs->functionType;
    obj->var.master = (ejs->master == 0);
    obj->var.isObject = 1;
    obj->var.dynamic = 1;
    obj->var.isFunction = 1;

    obj->slots = (EjsVar**) &(((char*) obj)[sizeof(EjsFrame)]);
    obj->capacity = numSlots;
    obj->numProp = numSlots;
#if BLD_DEBUG
    ejsAddToGcStats(ejs, (EjsVar*) obj, ES_Frame);
#endif
    return (EjsFrame*) obj;
}


/*
 *  Fast allocation of a function activation frame
 */
EjsFrame *ejsCreateFrame(Ejs *ejs, EjsFunction *src)
{
    EjsFrame    *frame;
    int         numSlots;

    numSlots = max(src->block.obj.numProp, EJS_MIN_FRAME_SLOTS);

    if ((frame = (EjsFrame*) ejsAllocPooledVar(ejs, ES_Frame)) == 0) {
        frame = allocFrame(ejs, numSlots);
    }
    frame->function.block.obj.var.isObject = 1;
    frame->function.block.obj.var.dynamic = 1;
    frame->function.block.obj.var.isFunction = 1;
    frame->function.block.obj.var.type = ejs->functionType;
    frame->function.block.obj.numProp = src->block.obj.numProp;
    frame->function.block.obj.names = src->block.obj.names;
    frame->function.block.namespaces = src->block.namespaces;
    frame->function.block.scopeChain = src->block.scopeChain;
    frame->function.block.prev = src->block.prev;
    frame->function.block.traits = src->block.traits;

    frame->function.block.numTraits = src->block.numTraits;
    frame->function.block.sizeTraits = src->block.sizeTraits;
    frame->function.block.numInherited = src->block.numInherited;
    frame->function.block.hasScriptFunctions = src->block.hasScriptFunctions;
    frame->function.block.referenced = src->block.referenced;

    frame->function.numArgs = src->numArgs;
    frame->function.numDefault = src->numDefault;
    frame->function.nextSlot = src->nextSlot;
    frame->function.constructor = src->constructor;
    frame->function.getter = src->getter;
    frame->function.setter = src->setter;
    frame->function.staticMethod = src->staticMethod;
    frame->function.hasReturn = src->hasReturn;
    frame->function.isInitializer = src->isInitializer;
    frame->function.literalGetter = src->literalGetter;
    frame->function.override = src->override;
    frame->function.lang = src->lang;
    frame->function.fullScope = src->fullScope;
    frame->function.rest = src->rest;
    frame->function.loading = src->loading;
    frame->function.nativeProc = src->nativeProc;
    frame->function.isFrame = src->isFrame;

    frame->function.isFrame = 1;
    frame->function.prototype = 0;
    frame->function.properties = 0;
    frame->function.resultType = src->resultType;
    frame->function.slotNum = src->slotNum;
    frame->function.owner = src->owner;
    frame->function.body = src->body;
    frame->pc = src->body.code.byteCode;
    frame->argc = 0;

    if (src->block.obj.numProp > 0) {
        if (frame->function.block.obj.numProp > frame->function.block.obj.capacity) {
            ejsGrowObject(ejs, (EjsObject*) frame, numSlots);
        }
        memcpy(frame->function.block.obj.slots, src->block.obj.slots, src->block.obj.numProp * sizeof(EjsVar*));
        frame->function.block.obj.numProp = src->block.obj.numProp;
    }
    ejsSetDebugName(frame, ejsGetDebugName(src));
    mprAssert(frame->argc == 0);
    return frame;
}


void ejsCreateFunctionType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = ejsCreateCoreType(ejs, ejsName(&qname, EJS_INTRINSIC_NAMESPACE, "Function"), ejs->objectType, sizeof(EjsFunction),
        ES_Function, ES_Function_NUM_CLASS_PROP, ES_Function_NUM_INSTANCE_PROP, 
        EJS_ATTR_OBJECT | EJS_ATTR_NATIVE | EJS_ATTR_DYNAMIC_INSTANCE | EJS_ATTR_BLOCK_HELPERS);
    if (type) {
        ejs->functionType = type;
        ejsInitializeFunctionHelpers(type->helpers, 1);
    }
}


void ejsInitializeFunctionHelpers(EjsTypeHelpers *helpers, int all)
{
    helpers->createVar  = (EjsCreateVarHelper) createFunction;
    helpers->castVar    = (EjsCastVarHelper) castFunction;
    helpers->cloneVar   = (EjsCloneVarHelper) cloneFunctionVar;
    helpers->destroyVar = (EjsDestroyVarHelper) destroyFunction;
    helpers->markVar    = (EjsMarkVarHelper) ejsMarkFunction;

    /*
     *  These helpers are only used for accessing Function.prototype properties and Function.*
     *  They are not used for function actual parameters and locals.
     */
    if (all) {
        helpers->getProperty          = (EjsGetPropertyHelper) getFunctionProperty;
        helpers->getPropertyName      = (EjsGetPropertyNameHelper) getFunctionPropertyName;
        helpers->getPropertyCount     = (EjsGetPropertyCountHelper) getFunctionPropertyCount;
        helpers->lookupProperty       = (EjsLookupPropertyHelper) lookupFunctionProperty;
        helpers->setProperty          = (EjsSetPropertyHelper) setFunctionProperty;
        helpers->setPropertyName      = (EjsSetPropertyNameHelper) setFunctionPropertyName;
        helpers->deleteProperty       = (EjsDeletePropertyHelper) deleteFunctionProperty;
    }
}


void ejsConfigureFunctionType(Ejs *ejs)
{
    EjsType     *type;

    type = ejs->functionType;
    ejsBindMethod(ejs, type, ES_Function_apply, (EjsNativeFunction) applyFunction);
    ejsBindMethod(ejs, type, ES_Function_call, (EjsNativeFunction) callFunctionMethod);
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
