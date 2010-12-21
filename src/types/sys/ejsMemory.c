/*
 *  ejsMemory.c - Memory class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */
/********************************** Includes **********************************/

#include    "ejs.h"

/*********************************** Methods *********************************/
/*
 *  native static function get allocated(): Number
 */
static EjsVar *getAllocatedMemory(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (int) alloc->bytesAllocated);
}


/*
 *  native static function callback(fn: Function): Void
 */
static EjsVar *setRedlineCallback(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    mprAssert(argc == 1 && ejsIsFunction(argv[0]));

    if (!ejsIsFunction(argv[0])) {
        ejsThrowArgError(ejs, "Callaback is not a function");
        return 0;
    }
    ejs->memoryCallback = (EjsFunction*) argv[0];
    return 0;
}


/*
 *  native static function get maximum(): Number
 */
static EjsVar *getMaxMemory(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (int) alloc->maxMemory);
}


/*
 *  native static function set maximum(limit: Number): Void
 */
static EjsVar *setMaxMemory(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    int     maxMemory;

    mprAssert(argc == 1 && ejsIsNumber(argv[0]));

    maxMemory = ejsGetInt(argv[0]);
    mprSetAllocLimits(ejs, -1, maxMemory);
    return 0;
}


/*
 *  native static function get peak(): Number
 */
static EjsVar *getPeakMemory(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (int) alloc->peakAllocated);
}


/*
 *  native static function get redline(): Number
 */
static EjsVar *getRedline(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (int) alloc->redLine);
}


/*
 *  native static function set redline(limit: Number): Void
 */
static EjsVar *setRedline(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    int     redline;

    mprAssert(argc == 1 && ejsIsNumber(argv[0]));

    redline = ejsGetInt(argv[0]);
    if (redline <= 0) {
        redline = INT_MAX;
    }
    mprSetAllocLimits(ejs, redline, -1);
    return 0;
}


/*
 *  native static function get resident(): Number
 */
static EjsVar *getResident(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (int) alloc->rss);
}


/*
 *  native static function get stack(): Number
 */
static EjsVar *getStack(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (int) alloc->peakStack);
}


/*
 *  native static function get system(): Number
 */
static EjsVar *getSystemRam(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    MprAlloc    *alloc;

    alloc = mprGetAllocStats(ejs);
    return (EjsVar*) ejsCreateNumber(ejs, (double) alloc->ram);
}


/*
 *  native static function stats(): Void
 */
static EjsVar *printStats(Ejs *ejs, EjsVar *thisObj, int argc, EjsVar **argv)
{
    ejsPrintAllocReport(ejs);
    mprPrintAllocReport(ejs, "Memroy Report");
    return 0;
}


/******************************** Initialization ******************************/

void ejsCreateMemoryType(Ejs *ejs)
{
    EjsName     qname;

    ejsCreateCoreType(ejs, ejsName(&qname, "ejs.sys", "Memory"), ejs->objectType, sizeof(EjsObject), ES_ejs_sys_Memory,
        ES_ejs_sys_Memory_NUM_CLASS_PROP, ES_ejs_sys_Memory_NUM_INSTANCE_PROP, EJS_ATTR_NATIVE | EJS_ATTR_OBJECT_HELPERS);
}


void ejsConfigureMemoryType(Ejs *ejs)
{
    EjsType         *type;

    type = ejsGetType(ejs, ES_ejs_sys_Memory);

    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_allocated, (EjsNativeFunction) getAllocatedMemory);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_set_callback, (EjsNativeFunction) setRedlineCallback);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_maximum, (EjsNativeFunction) getMaxMemory);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_set_maximum, (EjsNativeFunction) setMaxMemory);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_peak, (EjsNativeFunction) getPeakMemory);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_redline, (EjsNativeFunction) getRedline);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_set_redline, (EjsNativeFunction) setRedline);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_resident, (EjsNativeFunction) getResident);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_stack, (EjsNativeFunction) getStack);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_system, (EjsNativeFunction) getSystemRam);
    ejsBindMethod(ejs, type, ES_ejs_sys_Memory_stats, (EjsNativeFunction) printStats);
}

/*
 *  @copy   default
 *  
 *  Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
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
