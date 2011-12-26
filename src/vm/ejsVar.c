/**
    ejsVar.c - Helper methods for the ejsVar interface.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/****************************** Forward Declarations **************************/

static EjsVar *getProperty(Ejs *ejs, EjsVar *vp, int slotNum);
static MprNumber parseNumber(Ejs *ejs, cchar *str);
static bool      parseBoolean(Ejs *ejs, cchar *s);

/*
    Get the type owning a property
 */
static inline EjsType *getOwningType(EjsVar *vp, int slotNum)
{
    EjsType     *type;

    type = vp->type;
    if (type->hasNativeBase) {
        if (vp->isType) {
            if (slotNum < type->block.numInherited) {
                do {
                    type = type->baseType;
                } while (slotNum < type->block.numInherited);
            }
        } else if (type->instanceBlock) {
            if (slotNum < type->instanceBlock->numInherited) {
                do {
                    type = type->baseType;
                } while (slotNum < type->instanceBlock->numInherited);
            }
        }
    }
    return type;
}

/************************************* Code ***********************************/
/**
    Cast the variable to a given target type.
    @return Returns a variable with the result of the cast or null if an exception is thrown.
 */
EjsVar *ejsCastVar(Ejs *ejs, EjsVar *vp, EjsType *targetType)
{
    mprAssert(ejs);
    mprAssert(targetType);

    if (vp == 0) {
        vp = ejs->undefinedValue;
    }
    if (vp->type == targetType) {
        return vp;
    }
    if (vp->type->helpers->castVar) {
        return (vp->type->helpers->castVar)(ejs, vp, targetType);
    }
    ejsThrowInternalError(ejs, "Cast helper not defined for type \"%s\"", vp->type->qname.name);
    return 0;
}


/*
    Create a new instance of a variable. Delegate to the type specific create.
 */
EjsVar *ejsCreateVar(Ejs *ejs, EjsType *type, int numSlots)
{
#if VXWORKS
    /*
     *  The VxWorks cc386 invoked linker crashes without this test. Ugh!
     */
    if (type == 0) {
        return 0;
    }
#endif
    mprAssert(type->helpers->createVar);
    return (type->helpers->createVar)(ejs, type, numSlots);
}


/**
    Copy a variable by copying all properties. If a property is a reference  type, just copy the reference.
    See ejsDeepClone for a complete recursive copy of all reference contents.
    @return Returns a variable or null if an exception is thrown.
 */
EjsVar *ejsCloneVar(Ejs *ejs, EjsVar *vp, bool deep)
{
    if (vp == 0) {
        return 0;
    }
    mprAssert(vp->type->helpers->cloneVar);
    return (vp->type->helpers->cloneVar)(ejs, vp, deep);
}


/*
    Define a property and its traits.
    @return Return the slot number allocated for the property.
 */
int ejsDefineProperty(Ejs *ejs, EjsVar *vp, int slotNum, EjsName *name, EjsType *propType, int attributes, EjsVar *value)
{
    EjsName     qname;

    mprAssert(name);
    mprAssert(name->name);
    mprAssert(name->space);
    
    if (ejs->flags & EJS_FLAG_COMPILER) {
        qname.name = mprStrdup(vp, name->name);
        qname.space = mprStrdup(vp, name->space);
        name = &qname;
    }
    mprAssert(vp->type->helpers->defineProperty);
    return (vp->type->helpers->defineProperty)(ejs, vp, slotNum, name, propType, attributes, value);
}


/**
    Delete a property in an object variable. The stack is unchanged.
    @return Returns a status code.
 */
int ejsDeleteProperty(Ejs *ejs, EjsVar *vp, int slotNum)
{
    EjsType     *type;

    mprAssert(slotNum >= 0);
    
    type = getOwningType(vp, slotNum);
    mprAssert(type->helpers->deleteProperty);
    return (type->helpers->deleteProperty)(ejs, vp, slotNum);
}


/**
    Delete a property in an object variable. The stack is unchanged.
    @return Returns a status code.
 */
int ejsDeletePropertyByName(Ejs *ejs, EjsVar *vp, EjsName *qname)
{
    EjsLookup   lookup;
    int         slotNum;

    mprAssert(qname);
    mprAssert(qname->name);
    mprAssert(qname->space);
    
    if (vp->type->helpers->deletePropertyByName) {
        return (vp->type->helpers->deletePropertyByName)(ejs, vp, qname);
    } else {
        slotNum = ejsLookupVar(ejs, vp, qname, &lookup);
        if (slotNum < 0) {
            ejsThrowReferenceError(ejs, "Property \"%s\" does not exist", qname->name);
            return 0;
        }
        return ejsDeleteProperty(ejs, vp, slotNum);
    }
}


void ejsDestroyVar(Ejs *ejs, EjsVar *vp)
{
    EjsType     *type;

    mprAssert(vp);

    type = vp->type;
    mprAssert(type->helpers->destroyVar);
    (type->helpers->destroyVar)(ejs, vp);
}


/**
    Get a property at a given slot in a variable.
    @return Returns the requested property varaible.
 */
EjsVar *ejsGetProperty(Ejs *ejs, EjsVar *vp, int slotNum)
{
    EjsType     *type;

    mprAssert(ejs);
    mprAssert(vp);
    mprAssert(slotNum >= 0);

    type = getOwningType(vp, slotNum);
    mprAssert(type->helpers->getProperty);
    return (type->helpers->getProperty)(ejs, vp, slotNum);
}


/*
    Get a property given a name.
 */
EjsVar *ejsGetPropertyByName(Ejs *ejs, EjsVar *vp, EjsName *name)
{
    int     slotNum;

    mprAssert(ejs);
    mprAssert(vp);
    mprAssert(name);

    /*
     *  WARNING: this is not implemented by most types
     */
    if (vp->type->helpers->getPropertyByName) {
        return (vp->type->helpers->getPropertyByName)(ejs, vp, name);
    }

    /*
     *  Fall back and use a two-step lookup and get
     */
    slotNum = ejsLookupProperty(ejs, vp, name);
    if (slotNum < 0) {
        return 0;
    }
    return ejsGetProperty(ejs, vp, slotNum);
}


/**
    Return the number of properties in the variable.
    @return Returns the number of properties.
 */
int ejsGetPropertyCount(Ejs *ejs, EjsVar *vp)
{
    mprAssert(vp->type->helpers->getPropertyCount);
    return (vp->type->helpers->getPropertyCount)(ejs, vp);
}


/**
    Return the name of a property indexed by slotNum.
    @return Returns the property name.
 */
EjsName ejsGetPropertyName(Ejs *ejs, EjsVar *vp, int slotNum)
{
    EjsType     *type;

    type = getOwningType(vp, slotNum);
    mprAssert(type->helpers->getPropertyName);
    return (type->helpers->getPropertyName)(ejs, vp, slotNum);
}


/**
    Return the trait for the indexed by slotNum.
    @return Returns the property name.
 */
EjsTrait *ejsGetPropertyTrait(Ejs *ejs, EjsVar *vp, int slotNum)
{
    EjsType     *type;

    type = getOwningType(vp, slotNum);
    mprAssert(type->helpers->getPropertyTrait);
    return (type->helpers->getPropertyTrait)(ejs, vp, slotNum);
}


/**
    Get a property slot. Lookup a property name and return the slot reference. If a namespace is supplied, the property
    must be defined with the same namespace.
    @return Returns the slot number or -1 if it does not exist.
 */
int ejsLookupProperty(Ejs *ejs, EjsVar *vp, EjsName *name)
{
    mprAssert(ejs);
    mprAssert(vp);
    mprAssert(name);
    mprAssert(name->name);
    mprAssert(name->space);

    mprAssert(vp->type->helpers->lookupProperty);
    return (vp->type->helpers->lookupProperty)(ejs, vp, name);
}


/*
    Invoke an operator.
    vp is left-hand-side
    @return Return a variable with the result or null if an exception is thrown.
 */
EjsVar *ejsInvokeOperator(Ejs *ejs, EjsVar *vp, int opCode, EjsVar *rhs)
{
    mprAssert(vp);

    mprAssert(vp->type->helpers->invokeOperator);
    return (vp->type->helpers->invokeOperator)(ejs, vp, opCode, rhs);
}


/*
    ejsMarkVar is in ejsGarbage.c
 */


/*
    Set a property and return the slot number. Incoming slot may be -1 to allocate a new slot.
 */
int ejsSetProperty(Ejs *ejs, EjsVar *vp, int slotNum, EjsVar *value)
{
    if (vp == 0) {
        ejsThrowReferenceError(ejs, "Object is null");
        return EJS_ERR;
    }
    mprAssert(vp->type->helpers->setProperty);
    return (vp->type->helpers->setProperty)(ejs, vp, slotNum, value);
}


/*
    Set a property given a name.
 */
int ejsSetPropertyByName(Ejs *ejs, EjsVar *vp, EjsName *qname, EjsVar *value)
{
    int     slotNum;

    mprAssert(ejs);
    mprAssert(vp);
    mprAssert(qname);

    /*
     *  WARNING: Not all types implement this
     */
    if (vp->type->helpers->setPropertyByName) {
        return (vp->type->helpers->setPropertyByName)(ejs, vp, qname, value);
    }

    /*
     *  Fall back and use a two-step lookup and get
     */
    slotNum = ejsLookupProperty(ejs, vp, qname);
    if (slotNum < 0) {
        slotNum = ejsSetProperty(ejs, vp, -1, value);
        if (slotNum < 0) {
            return EJS_ERR;
        }
        if (ejsSetPropertyName(ejs, vp, slotNum, qname) < 0) {
            return EJS_ERR;
        }
        return slotNum;
    }
    return ejsSetProperty(ejs, vp, slotNum, value);
}


/*
    Set the property name and return the slot number. Slot may be -1 to allocate a new slot.
 */
int ejsSetPropertyName(Ejs *ejs, EjsVar *vp, int slot, EjsName *qname)
{
    mprAssert(vp->type->helpers->setPropertyName);
    return (vp->type->helpers->setPropertyName)(ejs, vp, slot, qname);
}


int ejsSetPropertyTrait(Ejs *ejs, EjsVar *vp, int slot, EjsType *propType, int attributes)
{
    mprAssert(vp->type->helpers->setPropertyTrait);
    return (vp->type->helpers->setPropertyTrait)(ejs, vp, slot, propType, attributes);
}


/**
    Get a string representation of a variable.
    @return Returns a string variable or null if an exception is thrown.
 */
EjsString *ejsToString(Ejs *ejs, EjsVar *vp)
{
    EjsFunction     *fn;

    if (vp == 0) {
        return ejsCreateString(ejs, "undefined");
    } else if (ejsIsString(vp)) {
        return (EjsString*) vp;
    }

    /*
     *  Types can provide a toString method or a castVar helper
     */
    if (vp->type->helpers->getProperty != getProperty) { 
        fn = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) vp->type, ES_Object_toString);
        if (ejsIsFunction(fn)) {
            return (EjsString*) ejsRunFunction(ejs, fn, vp, 0, NULL);
        }
    }
    if (vp->type->helpers->castVar) {
        return (EjsString*) (vp->type->helpers->castVar)(ejs, vp, ejs->stringType);
    }
    ejsThrowInternalError(ejs, "CastVar helper not defined for type \"%s\"", vp->type->qname.name);
    return 0;
}


/**
    Get a numeric representation of a variable.
    @return Returns a number variable or null if an exception is thrown.
 */
EjsNumber *ejsToNumber(Ejs *ejs, EjsVar *vp)
{
    if (vp == 0 || ejsIsNumber(vp)) {
        return (EjsNumber*) vp;
    }
    if (vp->type->helpers->castVar) {
        return (EjsNumber*) (vp->type->helpers->castVar)(ejs, vp, ejs->numberType);
    }
    ejsThrowInternalError(ejs, "CastVar helper not defined for type \"%s\"", vp->type->qname.name);
    return 0;
}


/**
    Get a boolean representation of a variable.
    @return Returns a number variable or null if an exception is thrown.
 */
EjsBoolean *ejsToBoolean(Ejs *ejs, EjsVar *vp)
{
    if (vp == 0 || ejsIsBoolean(vp)) {
        return (EjsBoolean*) vp;
    }
    if (vp->type->helpers->castVar) {
        return (EjsBoolean*) (vp->type->helpers->castVar)(ejs, vp, ejs->booleanType);
    }
    ejsThrowInternalError(ejs, "CastVar helper not defined for type \"%s\"", vp->type->qname.name);
    return 0;
}


/**
    Get a serialized string representation of a variable using JSON encoding.
    @return Returns a string variable or null if an exception is thrown.
 */
EjsString *ejsToJson(Ejs *ejs, EjsVar *vp)
{
    EjsFunction     *fn;
    EjsString       *result;

    if (vp == 0) {
        return ejsCreateString(ejs, "undefined");
    }
    if (vp->jsonVisited) {
        return ejsCreateString(ejs, "this");
    }
    vp->jsonVisited = 1;

    /*
     *  Types can provide a toJSON method, a serializeVar helper. If neither are provided, toString is used as a fall-back.
     */
    fn = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) vp->type, ES_Object_toJSON);
    if (ejsIsFunction(fn)) {
        result = (EjsString*) ejsRunFunction(ejs, fn, vp, 0, NULL);
    } else {
        result = ejsToString(ejs, vp);
    }
    vp->jsonVisited = 0;
    return result;
}


/*
    Fully construct a new object. We create a new instance and call all required constructors.
 */
EjsVar *ejsCreateInstance(Ejs *ejs, EjsType *type, int argc, EjsVar **argv)
{
    EjsFunction     *fun;
    EjsVar          *vp;
    int             slotNum;

    mprAssert(type);

    vp = ejsCreateVar(ejs, type, 0);
    if (vp == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    if (type->hasConstructor) {
        slotNum = type->block.numInherited;
        fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, slotNum);
        if (fun == 0) {
            return 0;
        }
        if (!ejsIsFunction(fun)) {
            return 0;
        }
        vp->permanent = 1;
        ejsRunFunction(ejs, fun, vp, argc, argv);
        vp->permanent = 0;
    }
    return vp;
}


/*********************************** Default helpers *****************************/
/*
    Report reference errors
 */
static void reportError(Ejs *ejs, EjsVar *vp)
{
    if (ejsIsNull(vp)) {
        ejsThrowReferenceError(ejs, "Object reference is null");

    } else if (ejsIsUndefined(vp)) {
        ejsThrowReferenceError(ejs, "Reference is undefined");

    } else {
        ejsThrowReferenceError(ejs, "Undefined setProperty helper");
    }
}


static EjsVar *createVar(Ejs *ejs, EjsType *type, int size)
{
    EjsVar      *vp;

    if (size == 0 && (vp = ejsAllocPooledVar(ejs, type->id)) != NULL) {
        memset(vp, 0, type->instanceSize);
        vp->type = type;
        return vp;
    }
    return ejsAllocVar(ejs, type, size);
}


static EjsVar *castVar(Ejs *ejs, EjsVar *vp, EjsType *toType)
{
    EjsString   *result;
    char        *buf;

    buf = mprAsprintf(ejs, -1, "[object %s]", vp->type->qname.name);
    result = ejsCreateString(ejs, buf);
    mprFree(buf);
    return (EjsVar*) result;
}


/*
    Default clone is just to do a shallow copy since most types are immutable.
 */
static EjsVar *cloneVar(Ejs *ejs, EjsVar *vp, bool depth)
{
    return vp;
}


static int defineProperty(Ejs *ejs, EjsVar *vp, int slotNum, EjsName *name, EjsType *propType, int attributes, EjsVar *value)
{
    reportError(ejs, vp);
    return EJS_ERR;
}


static int deleteProperty(Ejs *ejs, EjsVar *vp, int slotNum)
{
    reportError(ejs, vp);
    return EJS_ERR;
}


static void destroyVar(Ejs *ejs, EjsVar *vp)
{
    ejsFreeVar(ejs, vp, -1);
}


static EjsVar *getProperty(Ejs *ejs, EjsVar *vp, int slotNum)
{
    reportError(ejs, vp);
    return 0;
}


static int getPropertyCount(Ejs *ejs, EjsVar *vp)
{
    return 0;
}


static EjsName getPropertyName(Ejs *ejs, EjsVar *vp, int slotNum)
{
    static EjsName  qname;

    reportError(ejs, vp);

    qname.name = 0;
    qname.space = 0;

    return qname;
}


static EjsTrait *getPropertyTrait(Ejs *ejs, EjsVar *vp, int slotNum)
{
    return ejsGetTrait((EjsBlock*) vp->type, slotNum);
}


static EjsVar *invokeOperator(Ejs *ejs, EjsVar *lhs, int opCode, EjsVar *rhs)
{
    switch (opCode) {

    case EJS_OP_BRANCH_EQ:
    case EJS_OP_BRANCH_STRICTLY_EQ:
        return (EjsVar*) ejsCreateBoolean(ejs, lhs == rhs);

    case EJS_OP_BRANCH_NE:
    case EJS_OP_BRANCH_STRICTLY_NE:
        return (EjsVar*) ejsCreateBoolean(ejs, !(lhs == rhs));

    default:
        /*
         *  Pass to the standard Object helpers to implement Object methods.
         */
        return (ejs->objectHelpers->invokeOperator)(ejs, lhs, opCode, rhs);
    }
    return 0;
}


static int lookupProperty(Ejs *ejs, EjsVar *vp, EjsName *name)
{
    /*
     *  Don't throw or report error if lookupProperty is not implemented
     */
    return EJS_ERR;
}


static void markVar(Ejs *ejs, EjsVar *parent, EjsVar *vp)
{
}


static int setProperty(Ejs *ejs, EjsVar *vp, int slotNum, EjsVar *value)
{
    reportError(ejs, vp);
    return EJS_ERR;
}


static int setPropertyName(Ejs *ejs, EjsVar *vp, int slotNum, EjsName *name)
{
    reportError(ejs, vp);
    return EJS_ERR;
}


static int setPropertyTrait(Ejs *ejs, EjsVar *vp, int slotNum, EjsType *propType, int attributes)
{
    reportError(ejs, vp);
    return EJS_ERR;
}


/*
    Native types are responsible for completing the rest of the helpers
 */
void ejsInitializeDefaultHelpers(EjsTypeHelpers *helpers)
{
    helpers->castVar            = castVar;
    helpers->createVar          = createVar;
    helpers->cloneVar           = cloneVar;
    helpers->defineProperty     = defineProperty;
    helpers->deleteProperty     = deleteProperty;
    helpers->destroyVar         = destroyVar;
    helpers->getProperty        = getProperty;
    helpers->getPropertyTrait   = getPropertyTrait;
    helpers->getPropertyCount   = getPropertyCount;
    helpers->getPropertyName    = getPropertyName;
    helpers->invokeOperator     = invokeOperator;
    helpers->lookupProperty     = lookupProperty;
    helpers->markVar            = markVar;
    helpers->setProperty        = setProperty;
    helpers->setPropertyName    = setPropertyName;
    helpers->setPropertyTrait   = setPropertyTrait;
}


EjsTypeHelpers *ejsGetDefaultHelpers(Ejs *ejs)
{
    return (EjsTypeHelpers*) mprMemdup(ejs, ejs->defaultHelpers, sizeof(EjsTypeHelpers));
}


EjsTypeHelpers *ejsGetObjectHelpers(Ejs *ejs)
{
    /*
     *  . Does every type need a copy -- NO. But the loader uses this and allows
     *  native types to selectively override. See samples/types/native.
     */
    return (EjsTypeHelpers*) mprMemdup(ejs, ejs->objectHelpers, sizeof(EjsTypeHelpers));
}


EjsTypeHelpers *ejsGetBlockHelpers(Ejs *ejs)
{
    return (EjsTypeHelpers*) mprMemdup(ejs, ejs->blockHelpers, sizeof(EjsTypeHelpers));
}


/************************************* Misc ***********************************/

EjsName *ejsAllocName(MprCtx ctx, cchar *name, cchar *space)
{
    EjsName     *np;

    np = mprAllocObj(ctx, EjsName);
    if (np) {
        np->name = mprStrdup(np, name);
        np->space = mprStrdup(np, space);
    }
    return np;
}


EjsName ejsCopyName(MprCtx ctx, EjsName *qname)
{
    EjsName     name;

    name.name = mprStrdup(ctx, qname->name);
    name.space = mprStrdup(ctx, qname->space);

    return name;
}


EjsName *ejsDupName(MprCtx ctx, EjsName *qname)
{
    return ejsAllocName(ctx, qname->name, qname->space);
}


EjsName *ejsName(EjsName *np, cchar *space, cchar *name)
{
    np->name = name;
    np->space = space;
    return np;
}


void ejsZeroSlots(Ejs *ejs, EjsVar **slots, int count)
{
    int     i;

    for (i = 0; i < count; i++) {
        slots[i] = ejs->nullValue;
    }
}


/*
    Parse a string based on formatting instructions and intelligently create a variable.
    Number formats:
        [(+|-)][0][OCTAL_DIGITS]
        [(+|-)][0][(x|X)][HEX_DIGITS]
        [(+|-)][DIGITS]
        [+|-][DIGITS][.][DIGITS][(e|E)[+|-]DIGITS]
 */
EjsVar *ejsParseVar(Ejs *ejs, cchar *buf, int preferredType)
{
    int         type;

    mprAssert(buf);

    type = preferredType;
    if (preferredType == ES_Void || preferredType < 0) {
        if ((*buf == '-' || *buf == '+') && isdigit((int) buf[1])) {
            type = ejs->numberType->id;

        } else if (!isdigit((int) *buf) && *buf != '.') {
            if (strcmp(buf, "true") == 0) {
                return (EjsVar*) ejs->trueValue;

            } else if (strcmp(buf, "false") == 0) {
                return (EjsVar*) ejs->falseValue;
            }
            type = ES_String;

        } else {
            type = ES_Number;
        }
    }

    switch (type) {
    case ES_Object:
    case ES_Void:
    case ES_Null:
    default:
        break;

    case ES_Number:
        return (EjsVar*) ejsCreateNumber(ejs, parseNumber(ejs, buf));

    case ES_Boolean:
        return (EjsVar*) ejsCreateBoolean(ejs, parseBoolean(ejs, buf));

    case ES_String:
        if (strcmp(buf, "null") == 0) {
            return (EjsVar*) ejsCreateNull(ejs);

        } else if (strcmp(buf, "undefined") == 0) {
            return (EjsVar*) ejsCreateUndefined(ejs);
        }
        return (EjsVar*) ejsCreateString(ejs, buf);
    }
    return (EjsVar*) ejsCreateUndefined(ejs);
}


/*
    Convert the variable to a number type. Only works for primitive types.
 */
static bool parseBoolean(Ejs *ejs, cchar *s)
{
    if (s == 0 || *s == '\0') {
        return 0;
    }
    if (strcmp(s, "false") == 0 || strcmp(s, "FALSE") == 0) {
        return 0;
    }
    return 1;
}


/*
    Convert the string buffer to a Number.
 */
static MprNumber parseNumber(Ejs *ejs, cchar *str)
{
    int64       num;
    int         radix, c, negative;

    mprAssert(str);

    num = 0;
    negative = 0;

    if (*str == '-') {
        str++;
        negative = 1;
    } else if (*str == '+') {
        str++;
    }

#if BLD_FEATURE_FLOATING_POINT
    if (*str != '.' && !isdigit((int) *str)) {
        return ejs->nanValue->value;
    }

    /*
        Floating format: [DIGITS].[DIGITS][(e|E)[+|-]DIGITS]
     */
    if (!(*str == '0' && tolower((int) str[1]) == 'x')) {
        MprNumber   n;
        cchar       *cp;
        for (cp = str; *cp; cp++) {
            if (*cp == '.' || tolower((int) *cp) == 'e') {
                n = atof(str);
                if (negative) {
                    n = 0.0 - n;
                }
                return n;
            }
        }
    }
#else
    if (*str != '.' && !isdigit(*str)) {
        return 0;
    }
#endif

    /*
        Parse an integer. Observe hex and octal prefixes (0x, 0).
     */
    if (*str != '0') {
        /*
         *  Normal numbers (Radix 10)
         */
        while (isdigit((int) *str)) {
            num = (*str - '0') + (num * 10);
            str++;
        }
    } else {
        str++;
        if (tolower((int) *str) == 'x') {
            str++;
            radix = 16;
            while (*str) {
                c = tolower((int) *str);
                if (isdigit(c)) {
                    num = (c - '0') + (num * radix);
                } else if (c >= 'a' && c <= 'f') {
                    num = (c - 'a' + 10) + (num * radix);
                } else {
                    break;
                }
                str++;
            }

        } else{
            radix = 8;
            while (*str) {
                c = tolower((int) *str);
                if (isdigit(c) && c < '8') {
                    num = (c - '0') + (num * radix);
                } else {
                    break;
                }
                str++;
            }
        }
    }

    if (negative) {
        return (MprNumber) (0 - num);
    }
    return (MprNumber) num;
}


int _ejsIs(struct EjsVar *vp, int slot)
{
    EjsType     *tp;

    if (vp == 0) {
        return 0;
    }
    if (vp->type->id == slot) {
        return 1;
    }
    for (tp = ((EjsVar*) vp)->type->baseType; tp; tp = tp->baseType) {
        if (tp->id == slot) {
            return 1;
        }
    }
    return 0;
}

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.TXT distributed with
    this software for full details.

    This software is open source; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version. See the GNU General Public License for more
    details at: http://www.embedthis.com/downloads/gplLicense.html

    This program is distributed WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This GPL license does NOT permit incorporating this software into
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses
    for this software and support services are available from Embedthis
    Software at http://www.embedthis.com

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
