/**
    ejsMath.c - Math type class

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"
#include     <math.h>

/**************************** Forward Declarations ****************************/

#if BLD_FEATURE_NUM_TYPE_DOUBLE
#define fixed(n) ((int64) (floor(n)))
#else
#define fixed(n) (n)
#endif

/******************************************************************************/
/*
    function abs(value: Number): Number
 */
static EjsVar *math_abs(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) fabs(ejsGetNumber(argv[0])));
}


/*
    function acos(value: Number): Number
 */
static EjsVar *math_acos(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprNumber   value;
    
    value = ejsGetNumber(argv[0]);
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) acos(ejsGetNumber(argv[0])));
}


/*
    function asin(value: Number): Number
 */
static EjsVar *math_asin(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) asin(ejsGetNumber(argv[0])));
}


/*
    function atan(value: Number): Number
 */
static EjsVar *math_atan(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) atan(ejsGetNumber(argv[0])));
}


/*
    function atan2(x: Number, y: Number): Number
 */
static EjsVar *math_atan2(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) atan2(ejsGetNumber(argv[0]), ejsGetNumber(argv[1])));
}


/*
    function ceil(value: Number): Number
 */
static EjsVar *math_ceil(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) ceil(ejsGetNumber(argv[0])));
}


/*
    function cos(value: Number): Number
 */
static EjsVar *math_cos(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) cos(ejsGetNumber(argv[0])));
}


/*
    function exp(value: Number): Number
 */
static EjsVar *math_exp(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) exp(ejsGetNumber(argv[0])));
}


/*
    function floor(value: Number): Number
 */
static EjsVar *math_floor(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) floor(ejsGetNumber(argv[0])));
}


/*
    function log10(value: Number): Number
 */
static EjsVar *math_log10(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) log10(ejsGetNumber(argv[0])));
}


/*
    function log(value: Number): Number
 */
static EjsVar *math_log(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) log(ejsGetNumber(argv[0])));
}


/*
    function max(x: Number, y: Number): Number
 */
static EjsVar *math_max(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprNumber   x, y;
    
    x = ejsGetNumber(argv[0]);
    y = ejsGetNumber(argv[1]);
    if (x > y) {
        return argv[0];
    }
    return argv[1];
}


/*
    function min(value: Number): Number
 */
static EjsVar *math_min(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprNumber   x, y;
    
    x = ejsGetNumber(argv[0]);
    y = ejsGetNumber(argv[1]);
    if (x < y) {
        return argv[0];
    }
    return argv[1];
}


/*
    function pow(x: Number, y: Number): Number
 */
static EjsVar *math_pow(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprNumber   x, y, result;
    
    x = ejsGetNumber(argv[0]);
    y = ejsGetNumber(argv[1]);
    result = pow(x, y);
#if CYGWIN
    /* Cygwin computes (0.0 / -1) == -Infinity */
    if (result < 0 && x == 0.0) {
        result = -result;
    }
#endif
    return (EjsObj*) ejsCreateNumber(ejs, (MprNumber) result);
}


/*
    function random(value: Number): Number
 */
static EjsVar *math_random(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprNumber   value;
    uint        uvalue;
    static int  initialized = 0;
    
    if (!initialized) {
#if WIN
        uint seed = (uint) time(0);
        srand(seed);
#elif !MACOSX && !VXWORKS
        srandom(time(0));
#endif
        initialized = 1;
    }
    
#if WIN
{
    errno_t rand_s(uint *value);
    rand_s(&uvalue);
}
#elif LINUX
    uvalue = random();
#elif MACOSX 
    uvalue = arc4random();
#else
{
    int64   data[16];
    int     i;
    mprGetRandomBytes(ejs, (char*) data, sizeof(data), 0);
    uvalue = 0;
    for (i = 0; i < sizeof(data) / sizeof(int64); i++) {
        uvalue += data[i];
    }
}
#endif
    value = ((MprNumber) (uvalue & 0x7FFFFFFF) / INT_MAX);
    return (EjsVar*) ejsCreateNumber(ejs, value);
}


/*
    function round(value: Number): Number
 */
static EjsVar *math_round(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprNumber   n;

    n = ejsGetNumber(argv[0]);

    if (n < 0 && n >= -0.5) {
        n = -0.0;
    } else {
        n += 0.5;
    }
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) floor(n));
}


/*
    function sin(value: Number): Number
 */
static EjsVar *math_sin(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) sin(ejsGetNumber(argv[0])));
}


/*
    function sqrt(value: Number): Number
 */
static EjsVar *math_sqrt(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) sqrt(ejsGetNumber(argv[0])));
}


/*
    function tan(value: Number): Number
 */
static EjsVar *math_tan(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) tan(ejsGetNumber(argv[0])));
}


/*********************************** Factory **********************************/

void ejsConfigureMathType(Ejs *ejs)
{
    EjsType     *type;

    type = ejs->mathType = ejsGetType(ejs, ES_Math);
    ejsBindMethod(ejs, type, ES_Math_abs, (EjsNativeFunction) math_abs);
    ejsBindMethod(ejs, type, ES_Math_acos, (EjsNativeFunction) math_acos);
    ejsBindMethod(ejs, type, ES_Math_asin, (EjsNativeFunction) math_asin);
    ejsBindMethod(ejs, type, ES_Math_atan, (EjsNativeFunction) math_atan);
    ejsBindMethod(ejs, type, ES_Math_atan2, (EjsNativeFunction) math_atan2);
    ejsBindMethod(ejs, type, ES_Math_ceil, (EjsNativeFunction) math_ceil);
    ejsBindMethod(ejs, type, ES_Math_cos, (EjsNativeFunction) math_cos);
    ejsBindMethod(ejs, type, ES_Math_exp, (EjsNativeFunction) math_exp);
    ejsBindMethod(ejs, type, ES_Math_floor, (EjsNativeFunction) math_floor);
    ejsBindMethod(ejs, type, ES_Math_log, (EjsNativeFunction) math_log);
    ejsBindMethod(ejs, type, ES_Math_log10, (EjsNativeFunction) math_log10);
    ejsBindMethod(ejs, type, ES_Math_max, (EjsNativeFunction) math_max);
    ejsBindMethod(ejs, type, ES_Math_min, (EjsNativeFunction) math_min);
    ejsBindMethod(ejs, type, ES_Math_pow, (EjsNativeFunction) math_pow);
    ejsBindMethod(ejs, type, ES_Math_random, (EjsNativeFunction) math_random);
    ejsBindMethod(ejs, type, ES_Math_round, (EjsNativeFunction) math_round);
    ejsBindMethod(ejs, type, ES_Math_sin, (EjsNativeFunction) math_sin);
    ejsBindMethod(ejs, type, ES_Math_sqrt, (EjsNativeFunction) math_sqrt);
    ejsBindMethod(ejs, type, ES_Math_tan, (EjsNativeFunction) math_tan);
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
