/**
 *  ejsGC.c - Garbage collector class for the EJS Object Model
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/************************************ Methods *********************************/
/*
 *  native static function get enabled(): Boolean
 */
static EjsVar *getEnable(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    return (EjsVar*) ((ejs->gc.enabled) ? ejs->trueValue: ejs->falseValue);
}


/*
 *  native static function set enabled(on: Boolean): Void
 */
static EjsVar *setEnable(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    mprAssert(argc == 1 && ejsIsBoolean(argv[0]));
    ejs->gc.enabled = ejsGetBoolean(argv[0]);
    return 0;
}


/*
 *  run(deep: Boolean = false)
 *  Note: deep currently is not implemented
 */
static EjsVar *runGC(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    int     deep;

    deep = ((argc == 1) && ejsIsBoolean(argv[1]));
    ejsCollectGarbage(ejs, EJS_GEN_NEW);
    return 0;
}


/*
 *  native static function get workQuota(): Number
 */
static EjsVar *getWorkQuota(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, ejs->workQuota);
}


/*
 *  native static function set workQuota(quota: Number): Void
 */
static EjsVar *setWorkQuota(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    int     quota;

    mprAssert(argc == 1 && ejsIsNumber(argv[0]));
    quota = ejsGetInt(argv[0]);

    if (quota < EJS_GC_SHORT_WORK_QUOTA && quota != 0) {
        ejsThrowArgError(ejs, "Bad work quota");
        return 0;
    }
    ejs->workQuota = quota;
    return 0;
}



void ejsCreateGCType(Ejs *ejs)
{
    EjsName     qname;

    ejsCreateCoreType(ejs, ejsName(&qname, "ejs.sys", "GC"), ejs->objectType, sizeof(EjsObject), ES_ejs_sys_GC,
        ES_ejs_sys_GC_NUM_CLASS_PROP, ES_ejs_sys_GC_NUM_INSTANCE_PROP, EJS_ATTR_NATIVE | EJS_ATTR_OBJECT_HELPERS);
}


void ejsConfigureGCType(Ejs *ejs)
{
    EjsType         *type;

    type = ejsGetType(ejs, ES_ejs_sys_GC);

    ejsBindMethod(ejs, type, ES_ejs_sys_GC_enabled, (EjsNativeFunction) getEnable);
    ejsBindMethod(ejs, type, ES_ejs_sys_GC_set_enabled, (EjsNativeFunction) setEnable);
    ejsBindMethod(ejs, type, ES_ejs_sys_GC_workQuota, (EjsNativeFunction) getWorkQuota);
    ejsBindMethod(ejs, type, ES_ejs_sys_GC_set_workQuota, (EjsNativeFunction) setWorkQuota);
    ejsBindMethod(ejs, type, ES_ejs_sys_GC_run, (EjsNativeFunction) runGC);
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
