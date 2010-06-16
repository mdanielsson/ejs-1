/**
 *  ejsObject.c - Object class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/****************************** Forward Declarations **************************/

static EjsName  getObjectPropertyName(Ejs *ejs, EjsObject *obj, int slotNum);
static int      growSlots(Ejs *ejs, EjsObject *obj, int size);
static int      hashProperty(EjsObject *obj, int slotNum, EjsName *qname);
static int      lookupObjectProperty(struct Ejs *ejs, EjsObject *obj, EjsName *qname);
static int      makeHash(EjsObject *obj);
static inline int cmpQname(EjsName *a, EjsName *b);
static void     removeHashEntry(EjsObject  *obj, EjsName *qname);
static EjsVar   *objectToString(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv);

#define CMP_QNAME(a,b) cmpQname(a, b)

#if KEEP
static inline int cmpName(EjsName *a, EjsName *b);
#define CMP_NAME(a,b) cmpName(a, b)
#endif

/************************************* Code ***********************************/
/*
 *  Cast the operand to a primitive type
 *
 *  intrinsic function cast(type: Type) : Object
 */
static EjsVar *castObject(Ejs *ejs, EjsObject *obj, EjsType *type)
{
    EjsString   *result;
    
    mprAssert(ejsIsType(type));

    switch (type->id) {
    case ES_Boolean:
        return (EjsVar*) ejsCreateBoolean(ejs, 1);

    case ES_Number:
        result = ejsToString(ejs, (EjsVar*) obj);
        if (result == 0) {
            ejsThrowMemoryError(ejs);
            return 0;
        }
        return ejsParseVar(ejs, ejsGetString(result), ES_Number);

    case ES_String:
        result = ejsCreateStringAndFree(ejs, mprStrcat(ejs, -1, "[object ", obj->var.type->qname.name, "]", NULL));
        return (EjsVar*) result;

    default:
        if (ejsIsA(ejs, (EjsVar*) obj, type)) {
            return (EjsVar*) obj;
        }
        ejsThrowTypeError(ejs, "Can't cast to this type");
        return 0;
    }
}


/*
 *  Create an object which is an instance of a given type. This is used by all scripted types to create objects. NOTE: 
 *  we only initialize the Object base class. It is up to the  caller to complete the initialization for all other base 
 *  classes by calling the appropriate constructors. capacity is the number of property slots to pre-allocate. Slots are 
 *  allocated and the property hash is configured.  If dynamic is true, then the property slots are allocated separately
 *  and can grow. 
 */
EjsObject *ejsCreateObject(Ejs *ejs, EjsType *type, int numExtraSlots)
{
    EjsObject   *obj;
    EjsBlock    *instanceBlock;
    EjsType     *tp;
    int         numSlots, roundSlots, size, hasNativeType;

    mprAssert(type);
    mprAssert(numExtraSlots >= 0);

    instanceBlock = type->instanceBlock;

    numSlots = numExtraSlots;
    if (instanceBlock) {
        numSlots += instanceBlock->obj.numProp;
    }

    /*
     *  Check if a script type is extending a native type. If so, we must not use integrated slots
     */
    hasNativeType = 0;
    if (!type->block.obj.var.native) {
        for (tp = type->baseType; tp && tp != ejs->objectType; tp = tp->baseType) {
            hasNativeType += tp->block.obj.var.native;
        }
    }

    roundSlots = 0;
    if (type->dontPool || hasNativeType || (obj = (EjsObject*) ejsAllocPooledVar(ejs, type->id)) == 0) {
        roundSlots = max(numSlots, EJS_MIN_OBJ_SLOTS);
        if (hasNativeType) {
            if ((obj = (EjsObject*) ejsAllocVar(ejs, type, 0)) == 0) {
                return 0;
            }

        } else {
            if ((obj = (EjsObject*) ejsAllocVar(ejs, type, roundSlots * sizeof(EjsVar*))) == 0) {
                return 0;
            }
            /*
             *  Always begin with objects using integrated slots
             */
            obj->slots = (EjsVar**) &(((char*) obj)[type->instanceSize]);
            obj->capacity = roundSlots;
        }

    } else {
        size = type->instanceSize - sizeof(EjsObject);
    }
    obj->var.type = type;
    obj->var.isObject = 1;
    obj->var.dynamic = type->block.dynamicInstance;
    obj->var.hidden = 0;
    ejsSetDebugName(obj, type->qname.name);

    if (numSlots > 0) {
        if (numSlots > obj->capacity) {
            ejsGrowObject(ejs, obj, numSlots);
        }
        ejsZeroSlots(ejs, obj->slots, numSlots);
    }
    obj->numProp = numSlots;
    obj->names = (instanceBlock) ? instanceBlock->obj.names : 0;
    return obj;
}


EjsObject *ejsCreateSimpleObject(Ejs *ejs)
{
    return ejsCreateObject(ejs, ejs->objectType, 0);
}


EjsObject *ejsCopyObject(Ejs *ejs, EjsObject *src, bool deep)
{
    EjsObject   *dest;
    int         numProp, i;

    numProp = src->numProp;

    dest = ejsCreateObject(ejs, src->var.type, numProp);
    if (dest == 0) {
        return 0;
    }
    
    dest->var.builtin = src->var.builtin;
    dest->var.dynamic = src->var.dynamic;
    dest->var.hasGetterSetter = src->var.hasGetterSetter;
    dest->var.hasNativeBase = src->var.hasNativeBase;
    dest->var.hidden = src->var.hidden;
    dest->var.isFunction = src->var.isFunction;
    dest->var.isObject = src->var.isObject;
    dest->var.isInstanceBlock = src->var.isInstanceBlock;
    dest->var.isType = src->var.isType;
    dest->var.native = src->var.native;
    dest->var.noPool = src->var.noPool;
    dest->var.permanent = src->var.permanent;
    dest->var.primitive = src->var.primitive;
    dest->var.survived = src->var.survived;
    ejsSetDebugName(dest, mprGetName(src));

    if (numProp <= 0) {
        return dest;
    }

    for (i = 0; i < numProp; i++) {
        if (deep) {
            dest->slots[i] = ejsCloneVar(ejs, src->slots[i], deep);
        } else {
            dest->slots[i] = src->slots[i];
        }
    }

    if (dest->names == NULL && ejsGrowObjectNames(dest, numProp) < 0) {
        return 0;
    }
    for (i = 0; i < numProp && src->names; i++) {
        dest->names->entries[i].qname.space = mprStrdup(dest, src->names->entries[i].qname.space);
        dest->names->entries[i].qname.name = mprStrdup(dest, src->names->entries[i].qname.name);
        dest->names->entries[i].nextSlot = src->names->entries[i].nextSlot;
    }
    if (makeHash(dest) < 0) {
        return 0;
    }
    return dest;
}


/*
 *  Define a new property.
 */
static int defineObjectProperty(Ejs *ejs, EjsBlock *block, int slotNum, EjsName *qname, EjsType *propType, int attributes, 
    EjsVar *value)
{

    if (ejsIsBlock(block)) {
        return (ejs->blockHelpers->defineProperty)(ejs, (EjsVar*) block, slotNum, qname, propType, attributes, value);

    } else {
        ejsThrowInternalError(ejs, "Helper not defined for non-block object");
        return 0;
    }
}


/*
 *  Delete an instance property. To delete class properties, use the type as the obj.
 */
static int deleteObjectProperty(Ejs *ejs, EjsObject *obj, int slotNum)
{
    EjsName     qname;

    mprAssert(obj);
    mprAssert(obj->var.type);
    mprAssert(slotNum >= 0);

    if (!obj->var.dynamic && !(ejs->flags & EJS_FLAG_COMPILER)) {
        ejsThrowTypeError(ejs, "Can't delete properties in a non-dynamic object");
        return EJS_ERR;
    }
    if (slotNum < 0 || slotNum >= obj->numProp) {
        ejsThrowReferenceError(ejs, "Invalid property slot to delete");
        return EJS_ERR;
    }
    qname = getObjectPropertyName(ejs, obj, slotNum);
    if (qname.name == 0) {
        return EJS_ERR;
    }
    removeHashEntry(obj, &qname);
    obj->slots[slotNum] = ejs->undefinedValue;
    return 0;
}


/*
 *  Delete an instance property by name
 */
static int deleteObjectPropertyByName(Ejs *ejs, EjsObject *obj, EjsName *qname)
{
    int     slotNum;

    slotNum = lookupObjectProperty(ejs, obj, qname);
    if (slotNum < 0) {
        ejsThrowReferenceError(ejs, "Property does not exist");
        return EJS_ERR;
    } else {
        return deleteObjectProperty(ejs, obj, slotNum);
    }
}


static EjsVar *getObjectProperty(Ejs *ejs, EjsObject *obj, int slotNum)
{
    mprAssert(obj);
    mprAssert(obj->slots);
    mprAssert(slotNum >= 0);

    if (slotNum < 0 || slotNum >= obj->numProp) {
        ejsThrowReferenceError(ejs, "Property at slot \"%d\" is not found", slotNum);
        return 0;
    }
    return obj->slots[slotNum];
}


/*
 *  Return the number of properties in the object
 */
static int getObjectPropertyCount(Ejs *ejs, EjsObject *obj)
{
    mprAssert(obj);
    mprAssert(ejsIsObject(obj));

    return obj->numProp;
}


static EjsName getObjectPropertyName(Ejs *ejs, EjsObject *obj, int slotNum)
{
    EjsName     qname;

    mprAssert(obj);
    mprAssert(ejsIsObject(obj) || ejsIsArray(obj));
    mprAssert(obj->slots);
    mprAssert(slotNum >= 0);
    mprAssert(slotNum < obj->numProp);

    if (slotNum < 0 || slotNum >= obj->numProp || obj->names == 0) {
        qname.name = 0;
        qname.space = 0;
        return qname;
    }
    return obj->names->entries[slotNum].qname;
}


/*
 *  Cast the operands depending on the operation code
 */
EjsVar *ejsCoerceOperands(Ejs *ejs, EjsVar *lhs, int opcode, EjsVar *rhs)
{
    switch (opcode) {

    /*
     *  Binary operators
     */
    case EJS_OP_ADD:
        return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);

    case EJS_OP_AND: case EJS_OP_DIV: case EJS_OP_MUL: case EJS_OP_OR: case EJS_OP_REM:
    case EJS_OP_SHL: case EJS_OP_SHR: case EJS_OP_SUB: case EJS_OP_USHR: case EJS_OP_XOR:
        return ejsInvokeOperator(ejs, (EjsVar*) ejs->zeroValue, opcode, rhs);

    case EJS_OP_COMPARE_EQ:  case EJS_OP_COMPARE_NE:
        if (ejsIsNull(rhs) || ejsIsUndefined(rhs)) {
            return (EjsVar*) ((opcode == EJS_OP_COMPARE_EQ) ? ejs->falseValue: ejs->trueValue);
        } else if (ejsIsNumber(rhs)) {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, rhs);
        }
        return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);

    case EJS_OP_COMPARE_LE: case EJS_OP_COMPARE_LT:
    case EJS_OP_COMPARE_GE: case EJS_OP_COMPARE_GT:
        if (ejsIsNumber(rhs)) {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, rhs);
        }
        return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);

    case EJS_OP_COMPARE_STRICTLY_NE:
    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NOT_ZERO:
    case EJS_OP_COMPARE_NULL:
        return (EjsVar*) ejs->trueValue;

    case EJS_OP_COMPARE_STRICTLY_EQ:
    case EJS_OP_COMPARE_FALSE:
    case EJS_OP_COMPARE_TRUE:
    case EJS_OP_COMPARE_ZERO:
        return (EjsVar*) ejs->falseValue;

    /*
     *  Unary operators
     */
    case EJS_OP_LOGICAL_NOT: case EJS_OP_NOT:
        return 0;

    default:
        ejsThrowTypeError(ejs, "Opcode %d not valid for type %s", opcode, lhs->type->qname.name);
        return ejs->undefinedValue;
    }
    return 0;
}


EjsVar *ejsObjectOperator(Ejs *ejs, EjsVar *lhs, int opcode, EjsVar *rhs)
{
    EjsVar      *result;

    if (rhs == 0 || lhs->type != rhs->type) {
        if ((result = ejsCoerceOperands(ejs, lhs, opcode, rhs)) != 0) {
            return result;
        }
    }

    /*
     *  Types now match
     */
    switch (opcode) {

    case EJS_OP_COMPARE_EQ: case EJS_OP_COMPARE_STRICTLY_EQ:
    case EJS_OP_COMPARE_LE: case EJS_OP_COMPARE_GE:
        return (EjsVar*) ejsCreateBoolean(ejs, (lhs == rhs));

    case EJS_OP_COMPARE_NE: case EJS_OP_COMPARE_STRICTLY_NE:
    case EJS_OP_COMPARE_LT: case EJS_OP_COMPARE_GT:
        return (EjsVar*) ejsCreateBoolean(ejs, !(lhs == rhs));

    /*
     *  Unary operators
     */
    case EJS_OP_COMPARE_NOT_ZERO:
        return (EjsVar*) ejs->trueValue;

    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
    case EJS_OP_COMPARE_FALSE:
    case EJS_OP_COMPARE_TRUE:
    case EJS_OP_COMPARE_ZERO:
        return (EjsVar*) ejs->falseValue;

    case EJS_OP_LOGICAL_NOT: case EJS_OP_NOT: case EJS_OP_NEG:
        return (EjsVar*) ejs->oneValue;

    /*
     *  Binary operators
     */
    case EJS_OP_ADD: case EJS_OP_AND: case EJS_OP_DIV: case EJS_OP_MUL:
    case EJS_OP_REM: case EJS_OP_OR: case EJS_OP_SHL: case EJS_OP_SHR:
    case EJS_OP_SUB: case EJS_OP_USHR: case EJS_OP_XOR:
        return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, (EjsVar*) ejsToNumber(ejs, rhs));

    default:
        ejsThrowTypeError(ejs, "Opcode %d not implemented for type %s", opcode, lhs->type->qname.name);
        return 0;
    }
    mprAssert(0);
}


/*
 *  Lookup a property with a namespace qualifier in an object and return the slot if found. Return EJS_ERR if not found.
 */
static int lookupObjectProperty(struct Ejs *ejs, EjsObject *obj, EjsName *qname)
{
    EjsNames    *names;
    EjsName     *propName;
    int         slotNum, index;

    mprAssert(qname);
    mprAssert(qname->name);
    mprAssert(qname->space);

    names = obj->names;

    if (names == 0) {
        return EJS_ERR;
    }

    if (names->buckets == 0) {
        /*
         *  No hash. Just do a linear search
         */
        for (slotNum = 0; slotNum < obj->numProp; slotNum++) {
            propName = &names->entries[slotNum].qname;
            if (CMP_QNAME(propName, qname)) {
                return slotNum;
            }
        }
        return EJS_ERR;
    }
    
    /*
     *  Find the property in the hash chain if it exists. Note the hash does not include the namespace portion.
     *  We assume that names rarely clash with different namespaces. We do this so variable lookup and do a one
     *  hash probe and find matching names. Lookup will then pick the right namespace.
     */
    index = ejsComputeHashCode(names, qname);

    for (slotNum = names->buckets[index]; slotNum >= 0;  slotNum = names->entries[slotNum].nextSlot) {
        propName = &names->entries[slotNum].qname;
        /*
         *  Compare the name including the namespace portion
         */
        if (CMP_QNAME(propName, qname)) {
            return slotNum;
        }
    }
    return EJS_ERR;
}


#if KEEP
/****** WARNING - this usurps the native types. Should remove this function or push back into Object */
/*
 *  Lookup a qualified property name and count the number of name portion matches. This routine is used to quickly lookup a 
 *  qualified name AND determine if there are other names with different namespaces but having the same name portion.
 *  Returns EJS_ERR if more than one matching property is found (ie. two properties of the same name but with different 
 *  namespaces). This should be rare! Otherwise, return the slot number of the unique matching property.
 *
 *  This is a special lookup routine for fast varible lookup in the scope chain. Used by ejsLookupVar and ejsLookupScope.
 *  WARNING: updates qname->space
 */
int ejsLookupSingleProperty(Ejs *ejs, EjsObject *obj, EjsName *qname)
{
    EjsNames    *names;
    EjsName     *propName;
    int         i, slotNum, index, count;

    mprAssert(qname);
    mprAssert(qname->name);
    mprAssert(qname->space);
    mprAssert(qname->space[0] == '\0');

    names = obj->names;
    slotNum = -1;
    count = 0;

    if (names) {
        if (names->buckets == 0) {
            /*
             *  No hash. Just do a linear search. Examine all properties.
             */
            for (i = 0; i < obj->numProp; i++) {
                propName = &names->entries[i].qname;
                if (CMP_NAME(propName, qname)) {
                    count++;
                    slotNum = i;
                }
            }

        } else {
            /*
             *  Find the property in the hash chain if it exists. Note the hash does NOT include the namespace portion.
             *  We assume that names rarely clash with different namespaces. We do this so variable lookup and a single hash 
             *  probe will find matching names.
             */
            index = ejsComputeHashCode(names, qname);

            for (i = names->buckets[index]; i >= 0;  i = names->entries[i].nextSlot) {
                propName = &names->entries[i].qname;
                if (CMP_NAME(propName, qname)) {
                    slotNum = i;
                    count++;
                }
            }
        }
        if (count == 1) {
            if (mprLookupHash(ejs->standardSpaces, names->entries[slotNum].qname.space)) {
                qname->space = names->entries[slotNum].qname.space;
            } else {
                slotNum = -2;
            }
        }
    }

    return (count <= 1) ? slotNum : -2;
}
#endif

/*
 *  Mark the object properties for the garbage collector
 */
void ejsMarkObject(Ejs *ejs, EjsVar *parent, EjsObject *obj)
{
    EjsType     *type;
    EjsVar      *vp;
    int         i;

    mprAssert(ejsIsObject(obj) || ejsIsBlock(obj) || ejsIsFunction(obj) || ejsIsArray(obj) || ejsIsXML(obj));

    type = obj->var.type;

    for (i = 0; i < obj->numProp; i++) {
        vp = obj->slots[i];
        if (vp == 0 || vp == ejs->nullValue) {
            continue;
        }
        ejsMarkVar(ejs, (EjsVar*) obj, vp);
    }
}


/*
 *  Validate the supplied slot number. If set to -1, then return the next available property slot number.
 */
inline int ejsCheckObjSlot(Ejs *ejs, EjsObject *obj, int slotNum)
{
    if (slotNum < 0) {
        if (!obj->var.dynamic) {
            ejsThrowReferenceError(ejs, "Object is not dynamic");
            return EJS_ERR;
        }

        slotNum = obj->numProp;
        if (obj->numProp >= obj->capacity) {
            if (ejsGrowObject(ejs, obj, obj->numProp + 1) < 0) {
                ejsThrowMemoryError(ejs);
                return EJS_ERR;
            }
        } else {
            obj->numProp++;
        }

    } else if (slotNum >= obj->numProp) {
        if (ejsGrowObject(ejs, obj, slotNum + 1) < 0) {
            ejsThrowMemoryError(ejs);
            return EJS_ERR;
        }
    }
    return slotNum;
}


/**
 *  Set the value of a property.
 *  @param slot If slot is -1, then allocate the next free slot
 *  @return Return the property slot if successful. Return < 0 otherwise.
 */
static int setObjectProperty(Ejs *ejs, EjsObject *obj, int slotNum, EjsVar *value)
{
    mprAssert(ejs);
    mprAssert(obj);
    
    if ((slotNum = ejsCheckObjSlot(ejs, obj, slotNum)) < 0) {
        return EJS_ERR;
    }
    
    mprAssert(slotNum < obj->numProp);
    mprAssert(obj->numProp <= obj->capacity);

    if (obj->var.permanent && (EjsVar*) obj != ejs->global && !value->permanent) {
        value->permanent = 1;
    }
    mprAssert(value);
    obj->slots[slotNum] = value;
    return slotNum;
}


/*
 *  Set the name for a property. Objects maintain a hash lookup for property names. This is hash is created on demand 
 *  if there are more than N properties. If an object is not dynamic, it will use the types name hash. If dynamic, 
 *  then the types name hash will be copied when required. Callers must supply persistent names strings in qname. 
 */
static int setObjectPropertyName(Ejs *ejs, EjsObject *obj, int slotNum, EjsName *qname)
{
    EjsNames    *names;

    mprAssert(obj);
    mprAssert(qname);
    mprAssert(slotNum >= 0);

    if ((slotNum = ejsCheckObjSlot(ejs, obj, slotNum)) < 0) {
        return EJS_ERR;
    }

    /*
     *  If the hash is owned by the base type and this is a dynamic object, we need a new hash dedicated to the object.
     */
    if (obj->names == NULL) {
        if (ejsGrowObjectNames(obj, obj->numProp) < 0) {
            return EJS_ERR;
        }

    } else if (obj->var.dynamic && obj != mprGetParent(obj->names)) {
        /*
         *  Object is using the type's original names, must copy and use own names from here on.
         */
        if (ejsGrowObjectNames(obj, obj->numProp) < 0) {
            return EJS_ERR;
        }

    } else if (slotNum >= obj->names->sizeEntries) {
        if (ejsGrowObjectNames(obj, obj->numProp) < 0) {
            return EJS_ERR;
        }
    }
    names = obj->names;

    /*
     *  Remove the old hash entry if the name will change
     */
    if (names->entries[slotNum].nextSlot >= 0) {
        if (CMP_QNAME(&names->entries[slotNum].qname, qname)) {
            return slotNum;
        }
        removeHashEntry(obj, &names->entries[slotNum].qname);
    }

    /*
     *  Set the property name
     */
    names->entries[slotNum].qname = *qname;
    
    mprAssert(slotNum < obj->numProp);
    mprAssert(obj->numProp <= obj->capacity);
    
    if (obj->numProp <= EJS_HASH_MIN_PROP || qname->name == NULL) {
        return slotNum;
    }
    if (hashProperty(obj, slotNum, qname) < 0) {
        ejsThrowMemoryError(ejs);
        return EJS_ERR;
    }
    return slotNum;
}


void ejsMakePropertyDontDelete(EjsVar *vp, int dontDelete)
{
}


/*
 *  Set a property's enumerability by for/in. Return true if the property was enumerable.
 */

int ejsMakePropertyEnumerable(EjsVar *vp, bool enumerate)
{
    int     oldValue;

    oldValue = vp->hidden;
    vp->hidden = !enumerate;
    return oldValue;
}


/******************************* Slot Routines ********************************/
/*
 *  Grow the slot storage for the object and increase numProp
 */
int ejsGrowObject(Ejs *ejs, EjsObject *obj, int count)
{
    int     size;
    
    if (count <= 0) {
        return 0;
    }

    mprAssert(count >= obj->numProp);
    size = EJS_PROP_ROUNDUP(count);

    if (obj->capacity < count) {
        if (growSlots(ejs, obj, size) < 0) {
            return EJS_ERR;
        }
    }   
    if (obj->names && obj->names->sizeEntries < count) {
        if (ejsGrowObjectNames(obj, size) < 0) {
            return EJS_ERR;
        }
        if (obj->numProp > 0 && makeHash(obj) < 0) {
            return EJS_ERR;
        }
        mprAssert(obj->names->sizeEntries >= obj->numProp);
    }   
    if (count > obj->numProp) {
        obj->numProp = count;
    }
    
    mprAssert(count <= obj->capacity);
    mprAssert(obj->numProp <= obj->capacity);
    
    return 0;
}


/*
 *  Insert new slots at the specified offset and move up slots to make room. Increase numProp.
 */
int ejsInsertGrowObject(Ejs *ejs, EjsObject *obj, int incr, int offset)
{
    EjsHashEntry    *entries;
    EjsNames        *names;
    int             i, size, mark;

    mprAssert(obj);
    mprAssert(incr >= 0);

    if (incr == 0) {
        return 0;
    }
    
    /*
     *  Base this comparison on numProp and not on capacity as we may already have room to fit the inserted properties.
     */
    size = obj->numProp + incr;

    if (obj->capacity < size) {
        size = EJS_PROP_ROUNDUP(size);
        if (ejsGrowObjectNames(obj, size) < 0) {
            return EJS_ERR;
        }
        if (growSlots(ejs, obj, size) < 0) {
            return EJS_ERR;
        }
    }
    obj->numProp += incr;
    
    mprAssert(obj->numProp <= obj->capacity);
    
    if (ejsGrowObjectNames(obj, obj->numProp) < 0) {
        return EJS_ERR;
    }
    mprAssert(obj->names);
    names = obj->names;
    mark = offset + incr;
    for (i = obj->numProp - 1; i >= mark; i--) {
        obj->slots[i] = obj->slots[i - mark];
        names->entries[i] = names->entries[i - mark];
    }

    ejsZeroSlots(ejs, &obj->slots[offset], incr);

    entries = names->entries;
    for (i = offset; i < mark; i++) {
        entries[i].nextSlot = -1;
        entries[i].qname.name = "";
        entries[i].qname.space = "";
    }
    if (makeHash(obj) < 0) {
        return EJS_ERR;
    }   
    return 0;
}


/*
 *  Allocate or grow the slots storage for an object
 */
static int growSlots(Ejs *ejs, EjsObject *obj, int capacity)
{
    EjsVar      **slots;
    int         factor;

    mprAssert(obj);

    if (capacity <= obj->capacity) {
        return 0;
    }

    /*
     *  Allocate or grow the slots structures
     */
    if (capacity > obj->capacity) {
        if (obj->capacity > EJS_LOTSA_PROP) {
            /*
             *  Looks like a big object so grow by a bigger chunk.
             */
            factor = max(obj->capacity / 4, EJS_NUM_PROP);
            capacity = (capacity + factor) / factor * factor;
        }
        capacity = EJS_PROP_ROUNDUP(capacity);

        if (obj->slots == 0) {
            mprAssert(obj->capacity == 0);
            mprAssert(capacity > 0);
            obj->slots = (EjsVar**) mprAlloc(obj, sizeof(EjsVar*) * capacity);
            if (obj->slots == 0) {
                return EJS_ERR;
            }
            ejsZeroSlots(ejs, obj->slots, capacity);

        } else {
            if (obj->var.separateSlots) {
                mprAssert(obj->capacity > 0);
                obj->slots = (EjsVar**) mprRealloc(obj, obj->slots, sizeof(EjsVar*) * capacity);
            } else {
                slots = (EjsVar**) mprAlloc(obj, sizeof(EjsVar*) * capacity);
                memcpy(slots, obj->slots, obj->capacity * sizeof(EjsVar*));
                obj->var.separateSlots = 1;
                obj->slots = slots;
            }
            if (obj->slots == 0) {
                return EJS_ERR;
            }
            ejsZeroSlots(ejs, &obj->slots[obj->capacity], (capacity - obj->capacity));
        }
        obj->capacity = capacity;
    }
    return 0;
}


/*
 *  Remove a slot and name. Copy up all other properties. WARNING: this can only be used before property binding and 
 *  should only be used by the compiler.
 */
void ejsRemoveSlot(Ejs *ejs, EjsObject *obj, int slotNum, int compact)
{
    EjsNames    *names;
    int         i;

    mprAssert(obj);
    mprAssert(slotNum >= 0);
    mprAssert(slotNum >= 0);
    mprAssert(ejs->flags & EJS_FLAG_COMPILER);

    names = obj->names;

    if (compact) {
        mprAssert(names);

        for (i = slotNum + 1; i < obj->numProp; i++) {
            obj->slots[i - 1] = obj->slots[i];
            names->entries[i - 1] = names->entries[i];
        }
        obj->numProp--;
        i--;

    } else {
        i = slotNum;
    }

    obj->slots[i] = 0;
    names->entries[i].qname.name = "";
    names->entries[i].qname.space = "";
    names->entries[i].nextSlot = -1;
    
    makeHash(obj);
}


/******************************* Hash Routines ********************************/

/*
 *  Exponential primes
 */
static int hashSizes[] = {
     19, 29, 59, 79, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 0
};


int ejsGetHashSize(int numProp)
{
    int     i;

    for (i = 0; i < hashSizes[i]; i++) {
        if (numProp < hashSizes[i]) {
            return hashSizes[i];
        }
    }
    return hashSizes[i - 1];
}


/*
 *  Grow the names vector
 */
int ejsGrowObjectNames(EjsObject *obj, int size)
{
    EjsNames        *names;
    EjsHashEntry    *entries;
    bool            ownNames;
    int             i, oldSize;

    if (size == 0) {
        return 0;
    }
    names = obj->names;
    
    ownNames = obj == mprGetParent(names);
    oldSize = (names) ? names->sizeEntries: 0;

    if (names == NULL || !ownNames) {
        names = mprAllocObj(obj, EjsNames);
        if (names == 0) {
            return EJS_ERR;
        }
        names->buckets = 0;
        names->entries = 0;
        names->sizeEntries = 0;
        names->sizeBuckets = 0;
    }

    if (size < names->sizeEntries) {
        return 0;
    }
    size = EJS_PROP_ROUNDUP(size);
    
    if (ownNames) {
        entries = (EjsHashEntry*) mprRealloc(names, names->entries, sizeof(EjsHashEntry) * size);
        if (entries == 0) {
            return EJS_ERR;
        }

    } else {
        entries = (EjsHashEntry*) mprAlloc(names, sizeof(EjsHashEntry) * size);
        if (entries == 0) {
            return EJS_ERR;
        }
        if (obj->names) {
            for (i = 0; i < oldSize; i++) {
                entries[i] = obj->names->entries[i];
            }
        }
    }
    for (i = oldSize; i < size; i++) {
        entries[i].nextSlot = -1;
        entries[i].qname.name = "";
        entries[i].qname.space = "";
    }
    names->sizeEntries = size;
    names->entries = entries;
    obj->names = names;
    return 0;
}


static int hashProperty(EjsObject *obj, int slotNum, EjsName *qname)
{
    EjsNames    *names;
    EjsName     *slotName;
    int         chainSlotNum, lastSlot, index;

    mprAssert(qname);

    names = obj->names;
    mprAssert(names);
  
    /*
     *  Test if the number of hash buckets is too small or non-existant and re-make the hash.
     */
    if (names->sizeBuckets < obj->numProp) {
        return makeHash(obj);
    }

    index = ejsComputeHashCode(names, qname);

    /*
     *  Scan the collision chain
     */
    lastSlot = -1;
    chainSlotNum = names->buckets[index];
    mprAssert(chainSlotNum < obj->numProp);
    mprAssert(chainSlotNum < obj->capacity);

    while (chainSlotNum >= 0) {
        slotName = &names->entries[chainSlotNum].qname;
        if (CMP_QNAME(slotName, qname)) {
            return 0;
        }
        mprAssert(lastSlot != chainSlotNum);
        lastSlot = chainSlotNum;
        mprAssert(chainSlotNum != names->entries[chainSlotNum].nextSlot);
        chainSlotNum = names->entries[chainSlotNum].nextSlot;

        mprAssert(0 <= lastSlot && lastSlot < obj->numProp);
        mprAssert(0 <= lastSlot && lastSlot < obj->capacity);
    }

    if (lastSlot >= 0) {
        mprAssert(lastSlot < obj->numProp);
        mprAssert(lastSlot != slotNum);
        names->entries[lastSlot].nextSlot = slotNum;

    } else {
        /* Start a new hash chain */
        names->buckets[index] = slotNum;
    }

    names->entries[slotNum].nextSlot = -2;
    names->entries[slotNum].qname = *qname;

#if BLD_DEBUG
    if (obj->slots[slotNum]) {
        cchar   *name;
        name = mprGetName(obj);
        if (name && *name) {
            ejsSetDebugName(obj->slots[slotNum], qname->name);
        }
    }
#endif
    return 0;
}


/*
 *  Allocate or grow the properties storage for an object. This routine will also manage the hash index for the object. 
 *  If numInstanceProp is < 0, then grow the number of properties by an increment. Otherwise, set the number of properties 
 *  to numInstanceProp. We currently don't allow reductions.
 */
static int makeHash(EjsObject *obj)
{
    EjsHashEntry    *entries;
    EjsNames        *names;
    int             i, newHashSize;

    mprAssert(obj);

    names = obj->names;

    /*
     *  Don't make the hash if too few properties. Once we have a hash, keep using it even if we have too few properties now.
     */
    if (names == 0 || (obj->numProp <= EJS_HASH_MIN_PROP && names->buckets == 0)) {
        return 0;
    }

    /*
     *  Only reallocate the hash buckets if the hash needs to grow larger
     */
    newHashSize = ejsGetHashSize(obj->numProp);
    if (names->sizeBuckets < newHashSize) {
        mprFree(names->buckets);
        names->buckets = (int*) mprAlloc(names, newHashSize * sizeof(int));
        if (names->buckets == 0) {
            return EJS_ERR;
        }
        names->sizeBuckets = newHashSize;
    }
    mprAssert(names->buckets);

    /*
     *  Clear out hash linkage
     */
    memset(names->buckets, -1, names->sizeBuckets * sizeof(int));
    entries = names->entries;
    for (i = 0; i < names->sizeEntries; i++) {
        entries[i].nextSlot = -1;
    }

    /*
     *  Rehash all existing properties
     */
    for (i = 0; i < obj->numProp; i++) {
        if (entries[i].qname.name && hashProperty(obj, i, &entries[i].qname) < 0) {
            return EJS_ERR;
        }
    }

    return 0;
}


void ejsResetHash(Ejs *ejs, EjsObject *obj)
{
    EjsHashEntry    *entries;
    EjsNames        *names;
    EjsHashEntry    *he;
    int             i;

    names = obj->names;
    entries = names->entries;

    /*
     *  Clear out hash linkage
     */
    memset(names->buckets, -1, names->sizeBuckets * sizeof(int));
    entries = names->entries;
    for (i = 0; i < names->sizeEntries; i++) {
        he = &names->entries[i];
        he->nextSlot = -1;
        he->qname.name = "";
        he->qname.space = "";
    }
}


static void removeHashEntry(EjsObject *obj, EjsName *qname)
{
    EjsNames        *names;
    EjsHashEntry    *he;
    EjsName         *nextName;
    int             index, slotNum, lastSlot;

    names = obj->names;
    if (names == 0) {
        return;
    }

    if (names->buckets == 0) {
        /*
         *  No hash. Just do a linear search
         */
        for (slotNum = 0; slotNum < obj->numProp; slotNum++) {
            he = &names->entries[slotNum];
            if (CMP_QNAME(&he->qname, qname)) {
                he->qname.name = "";
                he->qname.space = "";
                he->nextSlot = -1;
                return;
            }
        }
        mprAssert(0);
        return;
    }


    index = ejsComputeHashCode(names, qname);
    slotNum = names->buckets[index];
    lastSlot = -1;
    while (slotNum >= 0) {
        he = &names->entries[slotNum];
        nextName = &he->qname;
        if (CMP_QNAME(nextName, qname)) {
            if (lastSlot >= 0) {
                names->entries[lastSlot].nextSlot = names->entries[slotNum].nextSlot;
            } else {
                names->buckets[index] = names->entries[slotNum].nextSlot;
            }
            he->qname.name = "";
            he->qname.space = "";
            he->nextSlot = -1;
            return;
        }
        lastSlot = slotNum;
        slotNum = names->entries[slotNum].nextSlot;
    }
    mprAssert(0);
}


int ejsRebuildHash(Ejs *ejs, EjsObject *obj)
{
    return makeHash(obj);
}


/*
 *  Compute a property name hash. Based on work by Paul Hsieh.
 */
int ejsComputeHashCode(EjsNames *names, EjsName *qname)
{
    uchar   *cdata;
    uint    len, hash, rem, tmp;

    mprAssert(names);
    mprAssert(qname);
    mprAssert(qname->name);

    if ((len = (int) strlen(qname->name)) == 0) {
        return 0;
    }

    rem = len & 3;
    hash = len;

#if KEEP_FOR_UNICODE
    for (len >>= 2; len > 0; len--, data += 2) {
        hash  += *data;
        tmp   =  (data[1] << 11) ^ hash;
        hash  =  (hash << 16) ^ tmp;
        hash  += hash >> 11;
    }
#endif

    cdata = (uchar*) qname->name;
    for (len >>= 2; len > 0; len--, cdata += 4) {
        hash  += *cdata | (cdata[1] << 8);
        tmp   =  ((cdata[2] | (cdata[3] << 8)) << 11) ^ hash;
        hash  =  (hash << 16) ^ tmp;
        hash  += hash >> 11;
    }

    switch (rem) {
    case 3: 
        hash += cdata[0] + (cdata[1] << 8);
        hash ^= hash << 16;
        hash ^= cdata[sizeof(ushort)] << 18;
        hash += hash >> 11;
        break;
    case 2: 
        hash += cdata[0] + (cdata[1] << 8);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1: hash += cdata[0];
        hash ^= hash << 10;
        hash += hash >> 1;
    }

    /* 
     *  Force "avalanching" of final 127 bits 
     */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    mprAssert(names->sizeBuckets);
    
    return hash % names->sizeBuckets;
}


static inline int cmpQname(EjsName *a, EjsName *b) 
{
    mprAssert(a);
    mprAssert(b);
    mprAssert(a->name);
    mprAssert(a->space);
    mprAssert(b->name);
    mprAssert(b->space);

    if (a->name == b->name && a->space == b->space) {
        return 1;
    }
    if (a->name[0] == b->name[0] && strcmp(a->name, b->name) == 0) {
        if (a->space[0] == b->space[0] && strcmp(a->space, b->space) == 0) {
            return 1;
        }
    }
    return 0;
}


#if KEEP
static inline int cmpName(EjsName *a, EjsName *b) 
{
    mprAssert(a);
    mprAssert(b);
    mprAssert(a->name);
    mprAssert(b->name);

    if (a->name == b->name) {
        return 1;
    }
    if (a->name[0] == b->name[0] && strcmp(a->name, b->name) == 0) {
        return 1;
    }
    return 0;
}
#endif

/*********************************** Methods **********************************/
/*
 *  WARNING: All methods here may be invoked by Native classes that are based on EjsVar and not on EjsObject. Because 
 *  all classes subclass Object, they need to be able to use these methods. They MUST NOT use EjsObject internals.
 */

static EjsVar *cloneObjectMethod(Ejs *ejs, EjsVar *op, int argc, EjsVar **argv)
{
    bool    deep;

    deep = (argc == 1 && argv[0] == (EjsVar*) ejs->trueValue);

    return ejsCloneVar(ejs, op, deep);
}


/*
 *  Function to iterate and return the next element name.
 *  NOTE: this is not a method of Object. Rather, it is a callback function for Iterator.
 */
static EjsVar *nextObjectKey(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsObject   *obj;
    EjsName     qname;

    obj = (EjsObject*) ip->target;
    if (!ejsIsObject(obj)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    for (; ip->index < obj->numProp; ip->index++) {
        qname = ejsGetPropertyName(ejs, (EjsVar*) obj, ip->index);
        if (qname.name == 0) {
            continue;
        }
        /*
         *  Enumerate over properties that have a public public or empty namespace 
         */
        if (qname.space[0] && strcmp(qname.space, EJS_PUBLIC_NAMESPACE) != 0) {
            continue;
        } else if (qname.name[0] == '\0') {
            continue;
        }
        ip->index++;
        return (EjsVar*) ejsCreateString(ejs, qname.name);
    }
    ejsThrowStopIteration(ejs);
    return 0;
}


/*
 *  Return the default iterator.
 *
 *  iterator native function get(deep: Boolean = false, namespaces: Array = null): Iterator
 */
static EjsVar *getObjectIterator(Ejs *ejs, EjsVar *op, int argc, EjsVar **argv)
{
    EjsVar      *namespaces;
    bool        deep;

    deep = (argc == 1) ? ejsGetBoolean(argv[0]): 0;
    namespaces =  (argc == 2) ? argv[1]: 0;

    return (EjsVar*) ejsCreateIterator(ejs, op, (EjsNativeFunction) nextObjectKey, deep, (EjsArray*) namespaces);
}


/*
 *  Function to iterate and return the next element value.
 *  NOTE: this is not a method of Object. Rather, it is a callback function for Iterator
 */
static EjsVar *nextObjectValue(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsObject   *obj;
    EjsVar      *vp;
    EjsName     qname;

    obj = (EjsObject*) ip->target;
    if (!ejsIsObject(obj)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    for (; ip->index < obj->numProp; ip->index++) {
        qname = ejsGetPropertyName(ejs, (EjsVar*) obj, ip->index);
        if (qname.name == 0) {
            continue;
        }
        if (qname.space[0] && strcmp(qname.space, EJS_PUBLIC_NAMESPACE) != 0) {
            continue;
        } else if (qname.name[0] == '\0') {
            continue;
        }
        vp = obj->slots[ip->index];
        if (vp) {
            ip->index++;
            return vp;
        }
    }
    ejsThrowStopIteration(ejs);
    return 0;
}


/*
 *  Return an iterator to return the next array element value.
 *
 *  iterator native function getValues(deep: Boolean = false, namespaces: Array = null): Iterator
 */
static EjsVar *getObjectValues(Ejs *ejs, EjsVar *op, int argc, EjsVar **argv)
{
    EjsVar      *namespaces;
    bool        deep;

    deep = (argc == 1) ? ejsGetBoolean(argv[0]): 0;
    namespaces =  (argc == 2) ? argv[1]: 0;

    return (EjsVar*) ejsCreateIterator(ejs, op, (EjsNativeFunction) nextObjectValue, deep, (EjsArray*) namespaces);
}


/*
 *  Get the length for the object.
 *
 *  intrinsic function get length(): Number
 */
static EjsVar *getObjectLength(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, ejsGetPropertyCount(ejs, vp));
}


#if ES_Object_seal
/**
 *  Seal a dynamic object. Once an object is sealed, further attempts to create or delete properties will fail and will throw
 *  @spec ejs-11
 */
static EjsVar *seal(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    vp->sealed = 1;
    return 0;
}
#endif


/*
 *  Convert the object to a JSON string. This also handles Json for Arrays.
 *
 *  intrinsic function toJSON(): String
 */
static EjsVar *objectToJson(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    MprBuf      *buf;
    EjsVar      *pp, *result;
    EjsBlock    *block;
    EjsName     qname;
    EjsObject   *obj;
    EjsString   *sv;
    char        key[16], *cp;
    int         c, isArray, i, count, slotNum, numInherited, maxDepth, flags, showAll, showBase;

    count = ejsGetPropertyCount(ejs, (EjsVar*) vp);
    if (count == 0 && vp->type != ejs->objectType && vp->type != ejs->arrayType) {
        return (EjsVar*) ejsToString(ejs, vp);
    }

    maxDepth = 99;
    flags = 0;
    showAll = 0;
    showBase = 0;

    isArray = ejsIsArray(vp);
    obj = (EjsObject*) vp;
    buf = mprCreateBuf(ejs, 0, 0);
    mprPutStringToBuf(buf, isArray ? "[\n" : "{\n");

    if (++ejs->serializeDepth <= maxDepth) {

        for (slotNum = 0; slotNum < count && !ejs->exception; slotNum++) {
            if (ejsIsBlock(obj)) {
                block = (EjsBlock*) obj;
                numInherited = ejsGetNumInheritedTraits(block);
                if (slotNum < numInherited && !(flags & EJS_FLAGS_ENUM_INHERITED)) {
                    continue;
                }
            }
            pp = ejsGetProperty(ejs, (EjsVar*) obj, slotNum);
            if (ejs->exception) {
                return 0;
            }
            if (pp == 0 || (pp->hidden && !(flags & EJS_FLAGS_ENUM_ALL))) {
                continue;
            }
            if (ejsIsFunction(pp) && !(flags & EJS_FLAGS_ENUM_ALL)) {
                continue;
            }
            if (isArray) {
                mprItoa(key, sizeof(key), slotNum, 10);
                qname.name = key;
                qname.space = "";
            } else {
                qname = ejsGetPropertyName(ejs, vp, slotNum);
            }

            if (qname.space && strstr(qname.space, ",private") != 0) {
                continue;
            }
            if (qname.space[0] == '\0' && qname.name[0] == '\0') {
                continue;
            }
            for (i = 0; i < ejs->serializeDepth; i++) {
                mprPutStringToBuf(buf, "  ");
            }
            if (!isArray) {
                mprPutCharToBuf(buf, '"');
                for (cp = (char*) qname.name; cp && *cp; cp++) {
                    c = *cp;
                    if (c == '"' || c == '\\') {
                        mprPutCharToBuf(buf, '\\');
                        mprPutCharToBuf(buf, c);
                    } else {
                        mprPutCharToBuf(buf, c);
                    }
                }
                mprPutStringToBuf(buf, "\": ");
            }
            sv = (EjsString*) ejsToJson(ejs, pp);
            if (sv == 0 || !ejsIsString(sv)) {
                if (!ejs->exception) {
                    ejsThrowTypeError(ejs, "Can't serialize property %s", qname.name);
                }
                return 0;
            } else {
                mprPutStringToBuf(buf, sv->value);
            }
            if ((slotNum + 1) < count) {
                mprPutCharToBuf(buf, ',');
            }
            mprPutStringToBuf(buf, "\n");
        }
    }
    for (i = --ejs->serializeDepth; i > 0; i--) {
        mprPutStringToBuf(buf, "  ");
    }
    mprPutCharToBuf(buf, isArray ? ']' : '}');

    mprAddNullToBuf(buf);
    result = (EjsVar*) ejsCreateString(ejs, mprGetBufStart(buf));
    mprFree(buf);
    return result;
}


#if ES_Object_toLocaleString
/*
 *  Convert the object to a localized string.
 *
 *  intrinsic function toLocaleString(): String
 */
static EjsVar *toLocaleString(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    return objectToString(ejs, vp, argc, argv);
}
#endif


/*
 *  Convert the object to a string.
 *
 *  intrinsic function toString(): String
 */
static EjsVar *objectToString(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    if (ejsIsString(vp)) {
        return vp;
    }
    return (EjsVar*) ejsCastVar(ejs, vp, ejs->stringType);
}


static void patchObjectSlots(Ejs *ejs, EjsObject *obj)
{
    EjsType     *type, *ot;
    EjsVar      *vp;
    EjsFunction *fun, *existingFun;
    int         i, j;

    ot = ejs->objectType;
    if (ejsIsType(obj)) {
        type = (EjsType*) obj;
        if (type != ot && ejsIsType(type) && !type->isInterface && type->objectBased && !ejsIsInstanceBlock(type)) {
            for (j = 0; j < ot->block.obj.numProp; j++) {
                fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) ot, j);
                if (ejsIsNativeFunction(fun)) {
                    existingFun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, j);
                    if (!ejsIsFunction(existingFun) || !existingFun->override) {
                        ejsSetProperty(ejs, (EjsVar*) type, j, (EjsVar*) fun);
                    }
                }
            }
        }
        if (type->instanceBlock) {
            patchObjectSlots(ejs, (EjsObject*) type->instanceBlock);
        }
    }
    for (i = 0; i < obj->numProp; i++) {
        if (ejsIsObject(obj)) {
            vp = obj->slots[i];
            if (vp == 0) {
                obj->slots[i] = ejs->nullValue;
            }
        }
    }
}


/************************************ Factory *********************************/
/*
 *  Create the object type
 */
void ejsCreateObjectType(Ejs *ejs)
{
    EjsName     qname;

    ejs->objectType = ejsCreateCoreType(ejs, ejsName(&qname, EJS_INTRINSIC_NAMESPACE, "Object"), 0, sizeof(EjsObject), 
        ES_Object, ES_Object_NUM_CLASS_PROP, ES_Object_NUM_INSTANCE_PROP, 
        EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_DYNAMIC_INSTANCE | EJS_ATTR_OBJECT_HELPERS);
}


void ejsConfigureObjectType(Ejs *ejs)
{
    EjsType     *ot;
    EjsObject   *obj;
    int         count, i;

    ot = ejs->objectType;
    mprAssert(ot);

    ejsBindMethod(ejs, ot, ES_Object_clone, cloneObjectMethod);
    ejsBindMethod(ejs, ot, ES_Object_get, getObjectIterator);
    ejsBindMethod(ejs, ot, ES_Object_getValues, getObjectValues);
    ejsBindMethod(ejs, ot, ES_Object_length, getObjectLength);
    ejsBindMethod(ejs, ot, ES_Object_toString, objectToString);
    ejsBindMethod(ejs, ot, ES_Object_toJSON, objectToJson);

#if ES_Object_seal
    ejsBindMethod(ejs, ot, ES_Object_seal, seal);
#endif
#if ES_Object_toLocaleString
    ejsBindMethod(ejs, ot, ES_Object_toLocaleString, toLocaleString);
#endif

    /*
     *  Patch native methods into all objects inheriting from object
     */
    patchObjectSlots(ejs, (EjsObject*) ejs->objectType);
    count = ejsGetPropertyCount(ejs, ejs->global);
    for (i = 0; i < count; i++) {
        obj = (EjsObject*) ejsGetProperty(ejs, ejs->global, i);
        if (ejsIsObject(obj)) {
            patchObjectSlots(ejs, obj);
        }
    }
}


void ejsInitializeObjectHelpers(EjsTypeHelpers *helpers)
{
    /*
     *  call is not implemented, EjsObject does not override and it is handled by the vm.
     */
    helpers->castVar                = (EjsCastVarHelper) castObject;
    helpers->cloneVar               = (EjsCloneVarHelper) ejsCopyObject;
    helpers->createVar              = (EjsCreateVarHelper) ejsCreateObject;
    helpers->defineProperty         = (EjsDefinePropertyHelper) defineObjectProperty;
    helpers->deleteProperty         = (EjsDeletePropertyHelper) deleteObjectProperty;
    helpers->deletePropertyByName   = (EjsDeletePropertyByNameHelper) deleteObjectPropertyByName;
    helpers->getProperty            = (EjsGetPropertyHelper) getObjectProperty;
    helpers->getPropertyCount       = (EjsGetPropertyCountHelper) getObjectPropertyCount;
    helpers->getPropertyName        = (EjsGetPropertyNameHelper) getObjectPropertyName;
    helpers->lookupProperty         = (EjsLookupPropertyHelper) lookupObjectProperty;
    helpers->invokeOperator         = (EjsInvokeOperatorHelper) ejsObjectOperator;
    helpers->markVar                = (EjsMarkVarHelper) ejsMarkObject;
    helpers->setProperty            = (EjsSetPropertyHelper) setObjectProperty;
    helpers->setPropertyName        = (EjsSetPropertyNameHelper) setObjectPropertyName;
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
