/**
    ejsDate.c - Date type class

    Date/time is store internally as milliseconds since 1970/01/01 GMT

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/***************************** Forward Declarations ***************************/
#if BLD_FEATURE_FLOATING_POINT
#define fixed(n) ((int64) (floor(n)))
#else
#define fixed(n) (n)
#endif

#if BLD_WIN_LIKE
#pragma warning (disable:4244)
#endif

#define getNumber(ejs, a) ejsGetNumber((EjsVar*) ejsToNumber(ejs, ((EjsVar*) a)))

/******************************************************************************/
/*
    Cast the operand to the specified type

    intrinsic function cast(type: Type) : Object
 */

static EjsVar *castDate(Ejs *ejs, EjsDate *dp, EjsType *type)
{
    struct tm   tm;

    switch (type->id) {

    case ES_Boolean:
        return (EjsVar*) ejs->trueValue;

    case ES_Number:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) dp->value);

    case ES_String:
        /*
            Format:  Tue Jul 15 2010 10:53:23 GMT-0700 (PDT)
         */
        mprDecodeLocalTime(ejs, &tm, dp->value);
        return (EjsVar*) ejsCreateStringAndFree(ejs, mprFormatTime(ejs, "%a %b %d %Y %T GMT%z (%Z)", &tm));

    default:
        ejsThrowTypeError(ejs, "Can't cast to this type");
        return 0;
    }
    return 0;
}


static EjsVar *coerceDateOperands(Ejs *ejs, EjsVar *lhs, int opcode, EjsVar *rhs)
{
    switch (opcode) {
    /*
        Binary operators
     */
    case EJS_OP_ADD:
        if (ejsIsUndefined(rhs)) {
            return (EjsVar*) ejs->nanValue;
        } else if (ejsIsNull(rhs)) {
            rhs = (EjsVar*) ejs->zeroValue;
        } else if (ejsIsBoolean(rhs) || ejsIsNumber(rhs)) {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, rhs);
        } else {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);
        }
        break;

    case EJS_OP_AND: case EJS_OP_DIV: case EJS_OP_MUL: case EJS_OP_OR: case EJS_OP_REM:
    case EJS_OP_SHL: case EJS_OP_SHR: case EJS_OP_SUB: case EJS_OP_USHR: case EJS_OP_XOR:
        return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, rhs);

    case EJS_OP_COMPARE_EQ: case EJS_OP_COMPARE_NE:
    case EJS_OP_COMPARE_LE: case EJS_OP_COMPARE_LT:
    case EJS_OP_COMPARE_GE: case EJS_OP_COMPARE_GT:
        if (ejsIsString(rhs)) {
            return ejsInvokeOperator(ejs, (EjsVar*) ejsToString(ejs, lhs), opcode, rhs);
        }
        return ejsInvokeOperator(ejs, (EjsVar*) ejsToNumber(ejs, lhs), opcode, rhs);

    case EJS_OP_COMPARE_STRICTLY_NE:
        return (EjsVar*) ejs->trueValue;

    case EJS_OP_COMPARE_STRICTLY_EQ:
        return (EjsVar*) ejs->falseValue;

    /*
        Unary operators
     */
    case EJS_OP_LOGICAL_NOT: case EJS_OP_NOT: case EJS_OP_NEG:
        return 0;

    case EJS_OP_COMPARE_NOT_ZERO:
    case EJS_OP_COMPARE_TRUE:
        return (EjsVar*) (((EjsDate*) lhs)->value ? ejs->trueValue : ejs->falseValue);

    case EJS_OP_COMPARE_ZERO:
    case EJS_OP_COMPARE_FALSE:
        return (EjsVar*) (((EjsDate*) lhs)->value ? ejs->falseValue: ejs->trueValue);

    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
        return (EjsVar*) ejs->falseValue;

    default:
        ejsThrowTypeError(ejs, "Opcode %d not valid for type %s", opcode, lhs->type->qname.name);
        return ejs->undefinedValue;
    }
    return 0;
}


static EjsVar *invokeDateOperator(Ejs *ejs, EjsDate *lhs, int opcode, EjsDate *rhs)
{
    EjsVar      *result;

    if (rhs == 0 || lhs->obj.var.type != rhs->obj.var.type) {
        if (!ejsIsA(ejs, (EjsVar*) lhs, ejs->dateType) || !ejsIsA(ejs, (EjsVar*) rhs, ejs->dateType)) {
            if ((result = coerceDateOperands(ejs, (EjsVar*) lhs, opcode, (EjsVar*) rhs)) != 0) {
                return result;
            }
        }
    }

    switch (opcode) {
    case EJS_OP_COMPARE_EQ: case EJS_OP_COMPARE_STRICTLY_EQ:
        return (EjsVar*) ejsCreateBoolean(ejs, lhs->value == rhs->value);

    case EJS_OP_COMPARE_NE: case EJS_OP_COMPARE_STRICTLY_NE:
        return (EjsVar*) ejsCreateBoolean(ejs, !(lhs->value == rhs->value));

    case EJS_OP_COMPARE_LT:
        return (EjsVar*) ejsCreateBoolean(ejs, lhs->value < rhs->value);

    case EJS_OP_COMPARE_LE:
        return (EjsVar*) ejsCreateBoolean(ejs, lhs->value <= rhs->value);

    case EJS_OP_COMPARE_GT:
        return (EjsVar*) ejsCreateBoolean(ejs, lhs->value > rhs->value);

    case EJS_OP_COMPARE_GE:
        return (EjsVar*) ejsCreateBoolean(ejs, lhs->value >= rhs->value);

    case EJS_OP_COMPARE_NOT_ZERO:
        return (EjsVar*) ((lhs->value) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_ZERO:
        return (EjsVar*) ((lhs->value == 0) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
    case EJS_OP_COMPARE_FALSE:
    case EJS_OP_COMPARE_TRUE:
        return (EjsVar*) ejs->falseValue;

    /*
        Unary operators
     */
    case EJS_OP_NEG:
        return (EjsVar*) ejsCreateNumber(ejs, - (MprNumber) lhs->value);

    case EJS_OP_LOGICAL_NOT:
        return (EjsVar*) ejsCreateBoolean(ejs, (MprNumber) !fixed(lhs->value));

    case EJS_OP_NOT:
        return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) (~fixed(lhs->value)));

    /*
        Binary operators
     */
    case EJS_OP_ADD:
        return (EjsVar*) ejsCreateDate(ejs, lhs->value + rhs->value);

    case EJS_OP_AND:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) & fixed(rhs->value)));

    case EJS_OP_DIV:
#if BLD_FEATURE_FLOATING_POINT
        if (rhs->value == 0) {
            ejsThrowArithmeticError(ejs, "Divisor is zero");
            return 0;
        }
#endif
        return (EjsVar*) ejsCreateDate(ejs, lhs->value / rhs->value);

    case EJS_OP_MUL:
        return (EjsVar*) ejsCreateDate(ejs, lhs->value * rhs->value);

    case EJS_OP_OR:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) | fixed(rhs->value)));

    case EJS_OP_REM:
#if BLD_FEATURE_FLOATING_POINT
        if (rhs->value == 0) {
            ejsThrowArithmeticError(ejs, "Divisor is zero");
            return 0;
        }
#endif
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) % fixed(rhs->value)));

    case EJS_OP_SHL:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) << fixed(rhs->value)));

    case EJS_OP_SHR:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) >> fixed(rhs->value)));

    case EJS_OP_SUB:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) - fixed(rhs->value)));

    case EJS_OP_USHR:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) >> fixed(rhs->value)));

    case EJS_OP_XOR:
        return (EjsVar*) ejsCreateDate(ejs, (MprNumber) (fixed(lhs->value) ^ fixed(rhs->value)));

    default:
        ejsThrowTypeError(ejs, "Opcode %d not implemented for type %s", opcode, lhs->obj.var.type->qname.name);
        return 0;
    }
    /* Should never get here */
}


/*
    Serialize using JSON encoding. This uses the ISO date format of UTC time.
 */
static EjsVar *date_toJSON(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;
    char        *base, *str;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    base = mprFormatTime(ejs, "%Y-%m-%dT%H:%M:%S", &tm);
    str = mprAsprintf(ejs, -1, "\"%sZ\"", base);
    mprFree(base);
    return (EjsVar*) ejsCreateStringAndFree(ejs, str);
}


/*********************************** Methods **********************************/
/*
    Date constructor
        Date()
        Date(milliseconds)
        Date(dateString)
        Date(year, month, date, hour, minute, second, msec)
        @param milliseconds Integer representing milliseconds since 1 January 1970 00:00:00 UTC.
        @param dateString String date value in a format recognized by parse().
        @param year Integer value for the year. Should be a Four digit year (e.g. 1998).
        @param month Integer month value (0-11)
        @param date Integer date of the month (1-31)
        @param hour Integer hour value (0-23)
        @param minute Integer minute value (0-59)
        @param second Integer second value (0-59)
        @param msec Integer millisecond value (0-999)
*/
static EjsVar *date_Date(Ejs *ejs, EjsDate *date, int argc, EjsVar **argv)
{
    EjsArray    *args;
    EjsVar      *vp;
    struct tm   tm;
    int         year;

    mprAssert(argc == 1 && ejsIsArray(argv[0]));

    args = (EjsArray*) argv[0];

    if (args->length == 0) {
        /* Now */
        date->value = mprGetTime(ejs);

    } else if (args->length == 1) {
        vp = ejsGetProperty(ejs, (EjsVar*) args, 0);
        if (ejsIsNumber(vp)) {
            /* Milliseconds */
            date->value = ejsGetNumber(vp);

        } else if (ejsIsString(vp)) {
            if (mprParseTime(ejs, &date->value, ejsGetString(vp), MPR_LOCAL_TIMEZONE, NULL) < 0) {
                ejsThrowArgError(ejs, "Can't parse date string: %s", ejsGetString(vp));
                return 0;
            }
        } else if (ejsIsDate(vp)) {
            date->value = ((EjsDate*) vp)->value;

        } else {
            ejsThrowArgError(ejs, "Can't construct date from this argument");
        }

    } else {
        /* Date(year, month, date, hour, minute, second, msec) or any portion thereof */
        memset(&tm, 0, sizeof(tm));
        tm.tm_isdst = -1;
        vp = ejsGetProperty(ejs, (EjsVar*) args, 0);
        year = getNumber(ejs, vp);
        if (year < 100) {
            year += 1900;
        }
        tm.tm_year = year - 1900;
        if (args->length > 1) {
            vp = ejsGetProperty(ejs, (EjsVar*) args, 1);
            tm.tm_mon = getNumber(ejs, vp);
        }
        if (args->length > 2) {
            vp = ejsGetProperty(ejs, (EjsVar*) args, 2);
            tm.tm_mday = getNumber(ejs, vp);
        }
        if (args->length > 3) {
            vp = ejsGetProperty(ejs, (EjsVar*) args, 3);
            tm.tm_hour = getNumber(ejs, vp);
        }
        if (args->length > 4) {
            vp = ejsGetProperty(ejs, (EjsVar*) args, 4);
            tm.tm_min = getNumber(ejs, vp);
        }
        if (args->length > 5) {
            vp = ejsGetProperty(ejs, (EjsVar*) args, 5);
            tm.tm_sec = getNumber(ejs, vp);
        }
        date->value = mprMakeTime(ejs, &tm);
        if (date->value == -1) {
            ejsThrowArgError(ejs, "Can't construct date from this argument");
        } else if (args->length > 6) {
            vp = ejsGetProperty(ejs, (EjsVar*) args, 6);
            date->value += getNumber(ejs, vp);
        }
    }
    return (EjsVar*) date;
}


/*
    function get day(): Number
    Range: 0-6, where 0 is Sunday
 */
static EjsVar *date_day(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_wday);
}


/*
    function set day(day: Number): Void
    Range: 0-6, where 0 is Sunday
*/
static EjsVar *date_set_day(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;
    int         dayDiff, day;

    day = ejsGetNumber(argv[0]);
    if (day < 0 || day > 6) {
        ejsThrowArgError(ejs, "Bad day. Range 0-6");
        return 0;
    }
    mprDecodeLocalTime(ejs, &tm, dp->value);
    dayDiff = day - tm.tm_wday;
    dp->value += dayDiff * 86400 * MPR_TICKS_PER_SEC;
    return 0;
}


/*
    function get dayOfYear(): Number
    Return day of year (0 - 365)
 */
static EjsVar *date_dayOfYear(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_yday);
}


/*
    function set dayOfYear(day: Number): Void
    Set the day of year (0 - 365)
 */
static EjsVar *date_set_dayOfYear(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;
    int         dayDiff, day;

    day = ejsGetNumber(argv[0]);
    if (day < 0 || day > 365) {
        ejsThrowArgError(ejs, "Bad day. Range 0-365");
        return 0;
    }
    mprDecodeLocalTime(ejs, &tm, dp->value);
    dayDiff = day - tm.tm_yday;
    dp->value += dayDiff * 86400 * MPR_TICKS_PER_SEC;
    return 0;
}


/*
    function get date(): Number
    Return day of month (1-31)
 */
static EjsVar *date_date(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_mday);
}


/*
    function set date(date: Number): Void
    Range day of month (1-31)
 */
static EjsVar *date_set_date(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;
    int         dayDiff, day;

    day = ejsGetNumber(argv[0]);
    if (day < 1 || day > 31) {
        ejsThrowArgError(ejs, "Bad day. Range 1-31");
        return 0;
    }
    mprDecodeLocalTime(ejs, &tm, dp->value);
    dayDiff = day - tm.tm_mday;
    dp->value += dayDiff * 86400 * MPR_TICKS_PER_SEC;
    return 0;
}


/*
    function get elapsed(): Number
    Get the elapsed time in milliseconds since the Date object was constructed
 */
static EjsVar *date_elapsed(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, mprGetElapsedTime(ejs, dp->value));
}


/*
    function format(layout: String): String
 */
static EjsVar *date_format(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprFormatTime(ejs, ejsGetString(argv[0]), &tm));
}


/*
    function formatUTC(layout: String): String
 */
static EjsVar *date_formatUTC(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprFormatTime(ejs, ejsGetString(argv[0]), &tm));
}


/*
    function get fullYear(): Number
    Return year in 4 digits
 */
static EjsVar *date_fullYear(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_year + 1900);
}


/*
    function set fullYear(year: Number): void
    Update the year component using a 4 digit year
 */
static EjsVar *date_set_fullYear(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    tm.tm_year = ejsGetNumber(argv[0]) - 1900;
    dp->value = mprMakeTime(ejs, &tm);
    return 0;
}


/**
    Return the number of minutes between the local computer time and Coordinated Universal Time.
    @return Integer containing the number of minutes between UTC and local time. The offset is positive if
    local time is behind UTC and negative if it is ahead. E.g. American PST is UTC-8 so 480 will be retured.
    This value will vary if daylight savings time is in effect.

    function getTimezoneOffset(): Number
*/
static EjsVar *date_getTimezoneOffset(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, -mprGetMpr(ejs)->timezone);
}


/*
    function getUTCDate(): Number
    Range: 0-31
 */
static EjsVar *date_getUTCDate(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_mday);
}


/*
    function getUTCDay(): Number
    Range: 0-6
 */
static EjsVar *date_getUTCDay(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_wday);
}


/*
    function getUTCFullYear(): Number
    Range: 4 digits
 */
static EjsVar *date_getUTCFullYear(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_year + 1900);
}


/*
    function getUTCHours(): Number
    Range: 0-23
 */
static EjsVar *date_getUTCHours(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_hour);
}


/*
    function getUTCMilliseconds(): Number
    Range: 0-999
 */
static EjsVar *date_getUTCMilliseconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, ((int64) dp->value) % MPR_TICKS_PER_SEC);
}


/*
    function getUTCMinutes(): Number
    Range: 0-31
 */
static EjsVar *date_getUTCMinutes(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_min);
}


/*
    function getUTCMonth(): Number
    Range: 1-12
 */
static EjsVar *date_getUTCMonth(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_mon);
}


/*
    function getUTCSeconds(): Number
    Range: 0-59
 */
static EjsVar *date_getUTCSeconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_sec);
}


/*
    function get hours(): Number
    Return hour of day (0-23)
 */
static EjsVar *date_hours(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_hour);
}


/*
    function set hours(hour: Number): void
    Update the hour of the day using a 0-23 hour
 */
static EjsVar *date_set_hours(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    tm.tm_hour = ejsGetNumber(argv[0]);
    dp->value = mprMakeTime(ejs, &tm);
    return 0;
}


/*
    function get milliseconds(): Number
 */
static EjsVar *date_milliseconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, ((int64) dp->value) % MPR_TICKS_PER_SEC);
}


/*
    function set milliseconds(ms: Number): void
 */
static EjsVar *date_set_milliseconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    dp->value = (dp->value / MPR_TICKS_PER_SEC  * MPR_TICKS_PER_SEC) + ejsGetNumber(argv[0]);
    return 0;
}


/*
    function get minutes(): Number
 */
static EjsVar *date_minutes(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_min);
}


/*
    function set minutes(min: Number): void
 */
static EjsVar *date_set_minutes(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    tm.tm_min = ejsGetNumber(argv[0]);
    dp->value = mprMakeTime(ejs, &tm);
    return 0;
}


/*
    function get month(): Number
    Get the month (0-11)
 */
static EjsVar *date_month(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_mon);
}


/*
    function set month(month: Number): void
 */
static EjsVar *date_set_month(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    tm.tm_mon = ejsGetNumber(argv[0]);
    dp->value = mprMakeTime(ejs, &tm);
    return 0;
}


/*
    function nextDay(inc: Number = 1): Date
 */
static EjsVar *date_nextDay(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    int64       inc;

    if (argc == 1) {
        inc = ejsGetNumber(argv[0]);
    } else {
        inc = 1;
    }
    return (EjsVar*) ejsCreateDate(ejs, dp->value + (inc * 86400 * 1000));
}


/*
    function now(): Number
 */
static EjsVar *date_now(Ejs *ejs, EjsDate *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, mprGetTime(ejs));
}


/*
    static function parse(arg: String): Date
 */
static EjsVar *date_parse(Ejs *ejs, EjsDate *unused, int argc, EjsVar **argv)
{
    MprTime     when;

    if (mprParseTime(ejs, &when, ejsGetString(argv[0]), MPR_LOCAL_TIMEZONE, NULL) < 0) {
        ejsThrowArgError(ejs, "Can't parse date string: %s", ejsGetString(argv[0]));
        return 0;
    }
    return (EjsVar*) ejsCreateNumber(ejs, when);
}


/*
    static function parseDate(arg: String, defaultDate: Date = null): Date
 */
static EjsVar *date_parseDate(Ejs *ejs, EjsDate *unused, int argc, EjsVar **argv)
{
    struct tm   tm, *defaults;
    MprTime     when;

    if (argc >= 2) {
        mprDecodeLocalTime(ejs, &tm, ((EjsDate*) argv[1])->value);
        defaults = &tm;
    } else {
        defaults = 0;
    }
    if (mprParseTime(ejs, &when, ejsGetString(argv[0]), MPR_LOCAL_TIMEZONE, defaults) < 0) {
        ejsThrowArgError(ejs, "Can't parse date string: %s", ejsGetString(argv[0]));
        return 0;
    }
    return (EjsVar*) ejsCreateDate(ejs, when);
}


/*
    static function parseUTCDate(arg: String, defaultDate: Date = null): Date
 */
static EjsVar *date_parseUTCDate(Ejs *ejs, EjsDate *unused, int argc, EjsVar **argv)
{
    struct tm   tm, *defaults;
    MprTime     when;

    if (argc >= 2) {
        mprDecodeUniversalTime(ejs, &tm, ((EjsDate*) argv[1])->value);
        defaults = &tm;
    } else {
        defaults = 0;
    }
    if (mprParseTime(ejs, &when, ejsGetString(argv[0]), MPR_UTC_TIMEZONE, defaults) < 0) {
        ejsThrowArgError(ejs, "Can't parse date string: %s", ejsGetString(argv[0]));
        return 0;
    }
    return (EjsVar*) ejsCreateDate(ejs, when);
}


/*
    function get seconds(): Number
    Get seconds (0-59)
 */
static EjsVar *date_seconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_sec);
}


/*
    function set seconds(sec: Number): void
 */
static EjsVar *date_set_seconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    tm.tm_sec = ejsGetNumber(argv[0]);
    dp->value = mprMakeTime(ejs, &tm);
    return 0;
}


/*
    function setUTCDate(date: Number): Void
    Range month (1-31)
 */
static EjsVar *date_setUTCDate(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;
    int         dayDiff, day;

    day = ejsGetNumber(argv[0]);
    if (day < 1 || day > 31) {
        ejsThrowArgError(ejs, "Bad day. Range 1-31");
        return 0;
    }
    mprDecodeUniversalTime(ejs, &tm, dp->value);
    dayDiff = day - tm.tm_mday;
    dp->value += dayDiff * 86400 * MPR_TICKS_PER_SEC;
    return 0;
}


/*
   function setUTCFullYear(y: Number): void
 */
static EjsVar *date_setUTCFullYear(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    tm.tm_year = ejsGetNumber(argv[0]) - 1900;
    dp->value = mprMakeUniversalTime(ejs, &tm);
    return 0;
}


/*
    function setUTCHours(h: Number): void
 */
static EjsVar *date_setUTCHours(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    tm.tm_hour = ejsGetNumber(argv[0]);
    dp->value = mprMakeUniversalTime(ejs, &tm);
    return 0;
}


/*
    function setUTCMilliseconds(ms: Number): void
 */
static EjsVar *date_setUTCMilliseconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    /* Same as set_milliseconds */
    dp->value = (dp->value / MPR_TICKS_PER_SEC  * MPR_TICKS_PER_SEC) + ejsGetNumber(argv[0]);
    return 0;
}


/*
    function setUTCMinutes(min: Number): void
 */
static EjsVar *date_setUTCMinutes(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    tm.tm_min = ejsGetNumber(argv[0]);
    dp->value = mprMakeUniversalTime(ejs, &tm);
    return 0;
}


/*
    function setUTCMonth(mon: Number): void
 */
static EjsVar *date_setUTCMonth(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    tm.tm_mon = ejsGetNumber(argv[0]);
    dp->value = mprMakeUniversalTime(ejs, &tm);
    return 0;
}


/*
    function setUTCSeconds(sec: Number, msec: Number = null): void
 */
static EjsVar *date_setUTCSeconds(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    tm.tm_sec = ejsGetNumber(argv[0]);
    dp->value = mprMakeUniversalTime(ejs, &tm);
    if (argc >= 2) {
        dp->value = (dp->value / MPR_TICKS_PER_SEC  * MPR_TICKS_PER_SEC) + ejsGetNumber(argv[1]);
    }
    return 0;
}


/*
    Get the number of millsecs since Jan 1, 1970 UTC.
    function get time(): Number
 */
static EjsVar *date_time(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, dp->value);
}


/*
    function set time(value: Number): Number
 */
static EjsVar *date_set_time(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    dp->value = ejsGetNumber(argv[0]);
    return 0;
}


/**
    Return an ISO formatted date string.
    Sample format: "2006-12-15T23:45:09.33-08:00"
    function toISOString(): String
*/
static EjsVar *date_toISOString(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    EjsVar      *vp;
    struct tm   tm;
    char        *base, *str;

    mprDecodeUniversalTime(ejs, &tm, dp->value);
    base = mprFormatTime(ejs, "%Y-%m-%dT%H:%M:%S", &tm);
    str = mprAsprintf(ejs, -1, "%s.%03dZ", base, dp->value % MPR_TICKS_PER_SEC);
    vp = (EjsVar*) ejsCreateStringAndFree(ejs, str);
    mprFree(base);
    return vp;
}


/*
    override native function toString(): String
 */
static EjsVar *date_toString(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    return castDate(ejs, dp, ejs->stringType);
}


/*
    Construct a date from UTC values
    function UTC(year, month, date, hour = 0, minute = 0, second = 0, msec = 0): Number
 */
static EjsVar *date_UTC(Ejs *ejs, EjsDate *unused, int argc, EjsVar **argv)
{
    EjsDate     *dp;
    struct tm   tm;
    int         year;

    memset(&tm, 0, sizeof(tm));
    year = getNumber(ejs, argv[0]);
    if (year < 100) {
        year += 1900;
    }
    tm.tm_year = year - 1900;
    if (argc > 1) {
        tm.tm_mon = getNumber(ejs, argv[1]);
    }
    if (argc > 2) {
        tm.tm_mday = getNumber(ejs, argv[2]);
    }
    if (argc > 3) {
        tm.tm_hour = getNumber(ejs, argv[3]);
    }
    if (argc > 4) {
        tm.tm_min = getNumber(ejs, argv[4]);
    }
    if (argc > 5) {
        tm.tm_sec = getNumber(ejs, argv[5]);
    }
    dp = ejsCreateDate(ejs, mprMakeUniversalTime(ejs, &tm));
    if (argc > 6) {
        dp->value += getNumber(ejs, argv[6]);
    }
    return (EjsVar*) ejsCreateNumber(ejs, dp->value);
}


/*
    function get year(): Number
 */
static EjsVar *date_year(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    return (EjsVar*) ejsCreateNumber(ejs, tm.tm_year + 1900);
}


/*
    function set year(year: Number): void
 */
static EjsVar *date_set_year(Ejs *ejs, EjsDate *dp, int argc, EjsVar **argv)
{
    struct tm   tm;
    MprNumber   value;

    mprDecodeLocalTime(ejs, &tm, dp->value);
    tm.tm_year = ejsGetNumber(argv[0]) - 1900;
    value = mprMakeTime(ejs, &tm);
    if (value == -1) {
        ejsThrowArgError(ejs, "Invalid year");
    } else {
        dp->value = value;
    }
    return 0;
}

/*********************************** Factory **********************************/
/*
    Create an initialized date object. Set to the current time if value is zero.
 */

EjsDate *ejsCreateDate(Ejs *ejs, MprTime value)
{
    EjsDate *vp;

    vp = (EjsDate*) ejsCreateVar(ejs, ejs->dateType, 0);
    if (vp != 0) {
        vp->value = value;
    }
    return vp;
}


void ejsCreateDateType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = ejsCreateCoreType(ejs, ejsName(&qname, EJS_INTRINSIC_NAMESPACE, "Date"), ejs->objectType, sizeof(EjsDate),
        ES_Date, ES_Date_NUM_CLASS_PROP, ES_Date_NUM_INSTANCE_PROP, EJS_ATTR_NATIVE | EJS_ATTR_HAS_CONSTRUCTOR);
    ejs->dateType = type;

    /*
        Define the helper functions.
     */
    type->helpers->castVar = (EjsCastVarHelper) castDate;
    type->helpers->invokeOperator = (EjsInvokeOperatorHelper) invokeDateOperator;
}


void ejsConfigureDateType(Ejs *ejs)
{
    EjsType     *type;

    type = ejs->dateType;

    ejsBindMethod(ejs, type, ES_Date_Date, (EjsNativeFunction) date_Date);
    ejsBindMethod(ejs, type, ES_Date_day, (EjsNativeFunction) date_day);
    ejsBindMethod(ejs, type, ES_Date_set_day, (EjsNativeFunction) date_set_day);
    ejsBindMethod(ejs, type, ES_Date_dayOfYear, (EjsNativeFunction) date_dayOfYear);
    ejsBindMethod(ejs, type, ES_Date_set_dayOfYear, (EjsNativeFunction) date_set_dayOfYear);
    ejsBindMethod(ejs, type, ES_Date_date, (EjsNativeFunction) date_date);
    ejsBindMethod(ejs, type, ES_Date_set_date, (EjsNativeFunction) date_set_date);
    ejsBindMethod(ejs, type, ES_Date_elapsed, (EjsNativeFunction) date_elapsed);
    ejsBindMethod(ejs, type, ES_Date_format, (EjsNativeFunction) date_format);
    ejsBindMethod(ejs, type, ES_Date_formatUTC, (EjsNativeFunction) date_formatUTC);
    ejsBindMethod(ejs, type, ES_Date_fullYear, (EjsNativeFunction) date_fullYear);
    ejsBindMethod(ejs, type, ES_Date_set_fullYear, (EjsNativeFunction) date_set_fullYear);
    ejsBindMethod(ejs, type, ES_Date_getTimezoneOffset, (EjsNativeFunction) date_getTimezoneOffset); 
    ejsBindMethod(ejs, type, ES_Date_getUTCDate, (EjsNativeFunction) date_getUTCDate);
    ejsBindMethod(ejs, type, ES_Date_getUTCDay, (EjsNativeFunction) date_getUTCDay);
    ejsBindMethod(ejs, type, ES_Date_getUTCFullYear, (EjsNativeFunction) date_getUTCFullYear);
    ejsBindMethod(ejs, type, ES_Date_getUTCHours, (EjsNativeFunction) date_getUTCHours);
    ejsBindMethod(ejs, type, ES_Date_getUTCMilliseconds, (EjsNativeFunction) date_getUTCMilliseconds);
    ejsBindMethod(ejs, type, ES_Date_getUTCMinutes, (EjsNativeFunction) date_getUTCMinutes);
    ejsBindMethod(ejs, type, ES_Date_getUTCMonth, (EjsNativeFunction) date_getUTCMonth);
    ejsBindMethod(ejs, type, ES_Date_getUTCSeconds, (EjsNativeFunction) date_getUTCSeconds);
    ejsBindMethod(ejs, type, ES_Date_hours, (EjsNativeFunction) date_hours);
    ejsBindMethod(ejs, type, ES_Date_set_hours, (EjsNativeFunction) date_set_hours);
    ejsBindMethod(ejs, type, ES_Date_milliseconds, (EjsNativeFunction) date_milliseconds);
    ejsBindMethod(ejs, type, ES_Date_set_milliseconds, (EjsNativeFunction) date_set_milliseconds);
    ejsBindMethod(ejs, type, ES_Date_minutes, (EjsNativeFunction) date_minutes);
    ejsBindMethod(ejs, type, ES_Date_set_minutes, (EjsNativeFunction) date_set_minutes);
    ejsBindMethod(ejs, type, ES_Date_month, (EjsNativeFunction) date_month);
    ejsBindMethod(ejs, type, ES_Date_set_month, (EjsNativeFunction) date_set_month);
    ejsBindMethod(ejs, type, ES_Date_nextDay, (EjsNativeFunction) date_nextDay);
    ejsBindMethod(ejs, type, ES_Date_now, (EjsNativeFunction) date_now);
    ejsBindMethod(ejs, type, ES_Date_parse, (EjsNativeFunction) date_parse);
    ejsBindMethod(ejs, type, ES_Date_parseDate, (EjsNativeFunction) date_parseDate);
    ejsBindMethod(ejs, type, ES_Date_parseUTCDate, (EjsNativeFunction) date_parseUTCDate);
    ejsBindMethod(ejs, type, ES_Date_seconds, (EjsNativeFunction) date_seconds);
    ejsBindMethod(ejs, type, ES_Date_set_seconds, (EjsNativeFunction) date_set_seconds);
    ejsBindMethod(ejs, type, ES_Date_setUTCDate, (EjsNativeFunction) date_setUTCDate);
    ejsBindMethod(ejs, type, ES_Date_setUTCFullYear, (EjsNativeFunction) date_setUTCFullYear);
    ejsBindMethod(ejs, type, ES_Date_setUTCHours, (EjsNativeFunction) date_setUTCHours);
    ejsBindMethod(ejs, type, ES_Date_setUTCMilliseconds, (EjsNativeFunction) date_setUTCMilliseconds);
    ejsBindMethod(ejs, type, ES_Date_setUTCMinutes, (EjsNativeFunction) date_setUTCMinutes);
    ejsBindMethod(ejs, type, ES_Date_setUTCMonth, (EjsNativeFunction) date_setUTCMonth);
    ejsBindMethod(ejs, type, ES_Date_setUTCSeconds, (EjsNativeFunction) date_setUTCSeconds);
    ejsBindMethod(ejs, type, ES_Date_time, (EjsNativeFunction) date_time);
    ejsBindMethod(ejs, type, ES_Date_set_time, (EjsNativeFunction) date_set_time);
    ejsBindMethod(ejs, type, ES_Date_toISOString, (EjsNativeFunction) date_toISOString);
    ejsBindMethod(ejs, type, ES_Date_UTC, (EjsNativeFunction) date_UTC);
    ejsBindMethod(ejs, type, ES_Date_year, (EjsNativeFunction) date_year);
    ejsBindMethod(ejs, type, ES_Date_set_year, (EjsNativeFunction) date_set_year);

    ejsBindMethod(ejs, type, ES_Object_toString, (EjsNativeFunction) date_toString);
    ejsBindMethod(ejs, type, ES_Object_toJSON, (EjsNativeFunction) date_toJSON);
}

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.

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

    @end
 */
