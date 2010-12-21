/**
    ejsNumber.c - Number type class

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/**************************** Forward Declarations ****************************/

#if BLD_FEATURE_NUM_TYPE_DOUBLE
#define fixed(n) ((int64) (floor(n)))
#else
#define fixed(n) (n)
#endif

#if UNUSED
#if BLD_WIN_LIKE || VXWORKS
static double localRint(double num)
{
    double low = floor(num);
    double high = ceil(num);
    return ((high - num) >= (num - low)) ? low : high;
}
#define rint localRint
#endif
#endif

/******************************************************************************/
/*
    Cast the operand to the specified type
 */
static EjsVar *castNumber(Ejs *ejs, EjsNumber *vp, EjsType *type)
{
    switch (type->id) {
    case ES_Boolean:
        return (EjsVar*) ((vp->value) ? ejs->trueValue : ejs->falseValue);

    case ES_String:
        {
#if BLD_FEATURE_NUM_TYPE_DOUBLE
        char *result = mprDtoa(vp, vp->value, 0, 0, 0);
        return (EjsVar*) ejsCreateStringAndFree(ejs, result);
#elif MPR_64_BIT
        char     numBuf[32];
        mprSprintf(numBuf, sizeof(numBuf), "%Ld", vp->value);
        return (EjsVar*) ejsCreateString(ejs, numBuf);
#else
        char     numBuf[32];
        mprItoa(numBuf, sizeof(numBuf), (int) vp->value, 10);
        return (EjsVar*) ejsCreateString(ejs, numBuf);
#endif
        }

    case ES_Number:
        return (EjsVar*) vp;
            
    default:
        ejsThrowTypeError(ejs, "Can't cast to this type");
        return 0;
    }
}


static EjsVar *coerceNumberOperands(Ejs *ejs, EjsVar *lhs, int opcode, EjsVar *rhs)
{
    switch (opcode) {
    /*
     *  Binary operators
     */
    case EJS_OP_ADD:
        if (ejsIsUndefined(rhs)) {
            return (EjsVar*) ejs->nanValue;
        } else if (ejsIsNull(rhs)) {
            return (EjsVar*) lhs;
        } else if (ejsIsBoolean(rhs) || ejsIsDate(rhs)) {
            return ejsInvokeOperator(ejs, lhs, opcode, (EjsVar*) ejsToNumber(ejs, rhs));
        } else {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);
        }
        break;

    case EJS_OP_AND: case EJS_OP_DIV: case EJS_OP_MUL: case EJS_OP_OR: case EJS_OP_REM:
    case EJS_OP_SHL: case EJS_OP_SHR: case EJS_OP_SUB: case EJS_OP_USHR: case EJS_OP_XOR:
        return ejsInvokeOperator(ejs, lhs, opcode, (EjsVar*) ejsToNumber(ejs, rhs));

    case EJS_OP_COMPARE_EQ: case EJS_OP_COMPARE_NE:
    case EJS_OP_COMPARE_LE: case EJS_OP_COMPARE_LT:
    case EJS_OP_COMPARE_GE: case EJS_OP_COMPARE_GT:
        if (ejsIsNull(rhs) || ejsIsUndefined(rhs)) {
            return (EjsVar*) ((opcode == EJS_OP_COMPARE_EQ) ? ejs->falseValue: ejs->trueValue);
        } else if (ejsIsNumber(rhs)) {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, rhs);
        } else if (ejsIsString(rhs)) {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);
        }
        return ejsInvokeOperator(ejs, lhs, opcode, (EjsVar*) ejsToNumber(ejs, rhs));

    case EJS_OP_COMPARE_STRICTLY_NE:
        return (EjsVar*) ejs->trueValue;

    case EJS_OP_COMPARE_STRICTLY_EQ:
        return (EjsVar*) ejs->falseValue;

    /*
     *  Unary operators
     */
    case EJS_OP_LOGICAL_NOT: case EJS_OP_NOT: case EJS_OP_NEG:
        return 0;

    case EJS_OP_COMPARE_NOT_ZERO:
    case EJS_OP_COMPARE_TRUE:
        return (EjsVar*) (((EjsNumber*) lhs)->value ? ejs->trueValue : ejs->falseValue);

    case EJS_OP_COMPARE_ZERO:
    case EJS_OP_COMPARE_FALSE:
        return (EjsVar*) (((EjsNumber*) lhs)->value ? ejs->falseValue: ejs->trueValue);

    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
        return (EjsVar*) ejs->falseValue;

    default:
        ejsThrowTypeError(ejs, "Opcode %d not valid for type %s", opcode, lhs->type->qname.name);
        return ejs->undefinedValue;
    }
    return 0;
}


static EjsVar *invokeNumberOperator(Ejs *ejs, EjsNumber *lhs, int opcode, EjsNumber *rhs)
{
    EjsVar      *result;

    mprAssert(lhs);
    
    if (rhs == 0 || lhs->obj.var.type != rhs->obj.var.type) {
        if (!ejsIsA(ejs, (EjsVar*) lhs, ejs->numberType) || !ejsIsA(ejs, (EjsVar*) rhs, ejs->numberType)) {
            if ((result = coerceNumberOperands(ejs, (EjsVar*) lhs, opcode, (EjsVar*) rhs)) != 0) {
                return result;
            }
        }
    }

    /*
     *  Types now match, both numbers
     */
    switch (opcode) {

    case EJS_OP_COMPARE_EQ: case EJS_OP_COMPARE_STRICTLY_EQ:
        return (EjsVar*) ((lhs->value == rhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_NE: case EJS_OP_COMPARE_STRICTLY_NE:
        return (EjsVar*) ((lhs->value != rhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_LT:
        return (EjsVar*) ((lhs->value < rhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_LE:
        return (EjsVar*) ((lhs->value <= rhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_GT:
        return (EjsVar*) ((lhs->value > rhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_GE:
        return (EjsVar*) ((lhs->value >= rhs->value) ? ejs->trueValue: ejs->falseValue);

    /*
     *  Unary operators
     */
    case EJS_OP_COMPARE_NOT_ZERO:
        return (EjsVar*) ((lhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_ZERO:
        return (EjsVar*) ((lhs->value == 0) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
    case EJS_OP_COMPARE_FALSE:
    case EJS_OP_COMPARE_TRUE:
        return (EjsVar*) ejs->falseValue;

    case EJS_OP_NEG:
        return (EjsVar*) ejsCreateNumber(ejs, -lhs->value);

    case EJS_OP_LOGICAL_NOT:
        return (EjsVar*) ejsCreateBoolean(ejs, !fixed(lhs->value));

    case EJS_OP_NOT:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (~fixed(lhs->value)));


    /*
     *  Binary operations
     */
    case EJS_OP_ADD:
        return (EjsVar*) ejsCreateNumber(ejs, lhs->value + rhs->value);

    case EJS_OP_AND:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) & fixed(rhs->value)));

    case EJS_OP_DIV:
#if !BLD_FEATURE_NUM_TYPE_DOUBLE
        if (rhs->value == 0) {
            ejsThrowArithmeticError(ejs, "Divisor is zero");
            return 0;
        }
#endif
        return (EjsVar*) ejsCreateNumber(ejs, lhs->value / rhs->value);

    case EJS_OP_MUL:
        return (EjsVar*) ejsCreateNumber(ejs, lhs->value * rhs->value);

    case EJS_OP_OR:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) | fixed(rhs->value)));

    case EJS_OP_REM:
#if BLD_FEATURE_NUM_TYPE_DOUBLE
        if (rhs->value == 0) {
            ejsThrowArithmeticError(ejs, "Divisor is zero");
            return 0;
        }
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) % fixed(rhs->value)));
#else
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) % fixed(rhs->value)));
#endif

    case EJS_OP_SHL:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) << fixed(rhs->value)));

    case EJS_OP_SHR:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) >> fixed(rhs->value)));

    case EJS_OP_SUB:
        return (EjsVar*) ejsCreateNumber(ejs, lhs->value - rhs->value);

    case EJS_OP_USHR:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) >> fixed(rhs->value)));

    case EJS_OP_XOR:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (fixed(lhs->value) ^ fixed(rhs->value)));

    default:
        ejsThrowTypeError(ejs, "Opcode %d not implemented for type %s", opcode, lhs->obj.var.type->qname.name);
        return 0;
    }
}


/*********************************** Methods **********************************/
/*
    Number constructor.
        function Number(value: Object = null)
 */
static EjsVar *numberConstructor(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
    EjsNumber   *num;

    mprAssert(argc == 0 || argc == 1);

    if (argc == 1) {
        num = ejsToNumber(ejs, argv[0]);
        if (num) {
            np->value = num->value;
        }
    }
    return (EjsVar*) np;
}


/*
    Function to iterate and return each number in sequence.
    NOTE: this is not a method of Number. Rather, it is a callback function for Iterator.
 */
static EjsVar *nextNumber(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsNumber   *np;

    np = (EjsNumber*) ip->target;
    if (!ejsIsNumber(np)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    if (ip->index < np->value) {
        return (EjsVar*) ejsCreateNumber(ejs, ip->index++);
    }
    ejsThrowStopIteration(ejs);
    return 0;
}


/*
    function integral(size: Number = 32): Number
 */
static EjsVar *integral(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
    int64   mask, result;
    int     size;

    size = (argc > 0) ? ejsGetInt(argv[0]) : 32;

    result = ((int64) np->value);
    if (size < 64) {
        mask = 1;
        mask = (mask << size) - 1;
        result &= mask;
    }
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) result);
}


/*
    function get isFinite(): Boolean
 */
static EjsVar *isFinite(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
    if (np->value == ejs->nanValue->value || 
            np->value == ejs->infinityValue->value || 
            np->value == ejs->negativeInfinityValue->value) {
        return (EjsVar*) ejs->falseValue;
    }
    return (EjsVar*) ejs->trueValue;
}


/*
    function get isNaN(): Boolean
 */
static EjsVar *isNaN(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
#if BLD_FEATURE_NUM_TYPE_DOUBLE
    return (EjsVar*) (mprIsNan(np->value) ? ejs->trueValue : ejs->falseValue);
#else
    return (EjsVar*) ejs->falseValue;
#endif
}


/*
    function toExponential(fractionDigits: Number = 0): String
    Display with only one digit before the decimal point.
 */
static EjsVar *toExponential(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
#if BLD_FEATURE_NUM_TYPE_DOUBLE
    char    *result;
    int     ndigits;
    
    ndigits = (argc > 0) ? ejsGetInt(argv[0]): 0;
    result = mprDtoa(np, np->value, ndigits, MPR_DTOA_N_DIGITS, MPR_DTOA_EXPONENT_FORM);
    return (EjsVar*) ejsCreateStringAndFree(ejs, result);
#else
    char    numBuf[32];
    mprItoa(numBuf, sizeof(numBuf), (int) np->value, 10);
    return (EjsVar*) ejsCreateString(ejs, numBuf);
#endif
}


/*
    function toFixed(fractionDigits: Number = 0): String

    Display the specified number of fractional digits
 */
static EjsVar *toFixed(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
#if BLD_FEATURE_NUM_TYPE_DOUBLE
    char    *result;
    int     ndigits;
    
    ndigits = (argc > 0) ? ejsGetInt(argv[0]) : 0;
    result = mprDtoa(np, np->value, ndigits, MPR_DTOA_N_FRACTION_DIGITS, MPR_DTOA_FIXED_FORM);
    return (EjsVar*) ejsCreateStringAndFree(ejs, result);
#else
    char    numBuf[32];
    mprItoa(numBuf, sizeof(numBuf), (int) np->value, 10);
    return (EjsVar*) ejsCreateString(ejs, numBuf);
#endif
}


/*
    function toPrecision(numDigits: Number = MAX_VALUE): String
    Display the specified number of total digits
 */
static EjsVar *toPrecision(Ejs *ejs, EjsNumber *np, int argc, EjsVar **argv)
{
#if BLD_FEATURE_NUM_TYPE_DOUBLE
    char    *result;
    int     ndigits;
    
    ndigits = (argc > 0) ? ejsGetInt(argv[0]) : 0;
    result = mprDtoa(np, np->value, ndigits, MPR_DTOA_N_DIGITS, 0);
    return (EjsVar*) ejsCreateStringAndFree(ejs, result);
#else
    char    numBuf[32];
    mprItoa(numBuf, sizeof(numBuf), (int) np->value, 10);
    return (EjsVar*) ejsCreateString(ejs, numBuf);
#endif
}


/*
    Return the default iterator. This returns the index names.
    iterator native function get(): Iterator
 */
static EjsVar *getNumberIterator(Ejs *ejs, EjsVar *np, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateIterator(ejs, np, (EjsNativeFunction) nextNumber, 0, NULL);
}


/*
    Convert the number to a string.
    intrinsic function toString(): String
 */
static EjsVar *numberToString(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    return castNumber(ejs, (EjsNumber*) vp, ejs->stringType);
}


/*********************************** Support **********************************/

#ifndef ejsIsNan
int ejsIsNan(double f)
{
#if BLD_FEATURE_NUM_TYPE_DOUBLE
#if BLD_WIN_LIKE
    return _isnan(f);
#elif VXWORKS
    return 0;
#else
    return (f == FP_NAN);
#endif
#else
    return 0;
#endif
}
#endif


bool ejsIsInfinite(MprNumber f)
{
#if BLD_FEATURE_NUM_TYPE_DOUBLE
#if BLD_WIN_LIKE
    return !_finite(f);
#elif VXWORKS
    return 0;
#else
    return (f == FP_INFINITE);
#endif
#else
    return 0;
#endif
}

/*********************************** Factory **********************************/
/*
    Create an initialized number
 */

EjsNumber *ejsCreateNumber(Ejs *ejs, MprNumber value)
{
    EjsNumber   *vp;

    if (value == 0) {
        return ejs->zeroValue;
    } else if (value == 1) {
        return ejs->oneValue;
    } else if (value == -1) {
        return ejs->minusOneValue;
    }

    vp = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    if (vp != 0) {
        vp->value = value;
        vp->obj.var.primitive = 1;
    }
    ejsSetDebugName(vp, "number value");
    return vp;
}


void ejsCreateNumberType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;
#if BLD_FEATURE_NUM_TYPE_DOUBLE
    static double zero = 0.0;
#endif

    type = ejsCreateCoreType(ejs, ejsName(&qname, EJS_INTRINSIC_NAMESPACE, "Number"), ejs->objectType, sizeof(EjsNumber),
        ES_Number, ES_Number_NUM_CLASS_PROP, ES_Number_NUM_INSTANCE_PROP, EJS_ATTR_NATIVE | EJS_ATTR_HAS_CONSTRUCTOR);
    ejs->numberType = type;

    /*
     *  Define the helper functions.
     */
    type->helpers->castVar = (EjsCastVarHelper) castNumber;
    type->helpers->invokeOperator = (EjsInvokeOperatorHelper) invokeNumberOperator;

    ejs->zeroValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->zeroValue->value = 0;
    ejs->oneValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->oneValue->value = 1;
    ejs->minusOneValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->minusOneValue->value = -1;

#if BLD_FEATURE_NUM_TYPE_DOUBLE
    ejs->infinityValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->infinityValue->value = 1.0 / zero;
    ejs->negativeInfinityValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->negativeInfinityValue->value = -1.0 / zero;
    ejs->nanValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->nanValue->value = 0.0 / zero;

    ejs->maxValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->maxValue->value = 1.7976931348623157e+308;
    ejs->minValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->minValue->value = 5e-324;

    ejsSetDebugName(ejs->infinityValue, "Infinity");
    ejsSetDebugName(ejs->negativeInfinityValue, "NegativeInfinity");
    ejsSetDebugName(ejs->nanValue, "NaN");

#else
    ejs->maxValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->maxValue->value = MAXINT;
    ejs->minValue = (EjsNumber*) ejsCreateVar(ejs, ejs->numberType, 0);
    ejs->minValue->value = -MAXINT;
    ejs->nanValue = ejs->zeroValue;
    ejs->infinityValue = ejs->maxValue;
    ejs->negativeInfinityValue = ejs->minValue;
    ejs->nanValue = (EjsNumber*) ejs->undefinedValue;
#endif

    ejsSetDebugName(ejs->minusOneValue, "-1");
    ejsSetDebugName(ejs->oneValue, "1");
    ejsSetDebugName(ejs->zeroValue, "0");
    ejsSetDebugName(ejs->maxValue, "MaxValue");
    ejsSetDebugName(ejs->minValue, "MinValue");
}


void ejsConfigureNumberType(Ejs *ejs)
{
    EjsType     *type;

    type = ejs->numberType;

    ejsSetProperty(ejs, (EjsVar*) type, ES_Number_MaxValue, (EjsVar*) ejs->maxValue);
    ejsSetProperty(ejs, (EjsVar*) type, ES_Number_MinValue, (EjsVar*) ejs->minValue);
    ejsBindMethod(ejs, type, ES_Number_Number, (EjsNativeFunction) numberConstructor);
    ejsBindMethod(ejs, type, ES_Number_integral, (EjsNativeFunction) integral);
    ejsBindMethod(ejs, type, ES_Number_isFinite, (EjsNativeFunction) isFinite);
    ejsBindMethod(ejs, type, ES_Number_isNaN, (EjsNativeFunction) isNaN);
    ejsBindMethod(ejs, type, ES_Number_toExponential, (EjsNativeFunction) toExponential);
    ejsBindMethod(ejs, type, ES_Number_toFixed, (EjsNativeFunction) toFixed);
    ejsBindMethod(ejs, type, ES_Number_toPrecision, (EjsNativeFunction) toPrecision);
    ejsBindMethod(ejs, type, ES_Object_get, getNumberIterator);
    ejsBindMethod(ejs, type, ES_Object_getValues, getNumberIterator);
    ejsBindMethod(ejs, type, ES_Object_toString, numberToString);
    ejsSetProperty(ejs, (EjsVar*) type, ES_Number_NEGATIVE_INFINITY, (EjsVar*) ejs->negativeInfinityValue);
    ejsSetProperty(ejs, (EjsVar*) type, ES_Number_POSITIVE_INFINITY, (EjsVar*) ejs->infinityValue);
    ejsSetProperty(ejs, (EjsVar*) type, ES_Number_NaN, (EjsVar*) ejs->nanValue);
    ejsSetProperty(ejs, ejs->global, ES_NegativeInfinity, (EjsVar*) ejs->negativeInfinityValue);
    ejsSetProperty(ejs, ejs->global, ES_Infinity, (EjsVar*) ejs->infinityValue);
    ejsSetProperty(ejs, ejs->global, ES_NaN, (EjsVar*) ejs->nanValue);
    ejsSetProperty(ejs, ejs->global, ES_num, (EjsVar*) type);
}

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.

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
