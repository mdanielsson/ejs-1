/**
 *  ejsWebCache.c - Native code for the Cache class.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if BLD_FEATURE_EJS_WEB
/*********************************** Forwards *********************************/
#if 0
static void cacheActivity(Ejs *ejs, EjsWebCache *sp);
static void cacheTimer(EjsWebControl *control, MprEvent *event);
#endif
/************************************* Code ***********************************/
 
static EjsVar *cacheConstructor(Ejs *ejs, EjsWebCache *cp, int argc, EjsVar **argv)
{
    Ejs         *master;
    EjsType     *cacheType;
    EjsName     qname;

    master = ejs->master ? ejs->master : ejs;

    cacheType = (EjsType*) ejsGetPropertyByName(master, master->global, ejsName(&qname, "ejs.web", "Cache"));
    if (cacheType == 0) {
        ejsThrowTypeError(ejs, "Can't find Cache type");
        return 0;
    }
#if ES_ejs_web_Cache_Cache
    cp->cache = ejsGetProperty(master, (EjsVar*) cacheType, ES_ejs_web_Cache_cache);
#endif
    return 0;
}


static EjsVar *readCache(Ejs *ejs, EjsWebCache *cp, int argc, EjsVar **argv)
{
    Ejs         *master;
    EjsName     qname;
    EjsVar      *vp;
    cchar       *domain, *key;

    master = ejs->master ? ejs->master : ejs;
    domain = ejsGetString(argv[0]);
    key = ejsGetString(argv[1]);
    ejsLockVm(master);

    vp = ejsGetPropertyByName(master, cp->cache, ejsName(&qname, domain, key));
    if (vp == 0) {
        ejsUnlockVm(master);
        return ejs->nullValue;
    }
    vp = ejsDeserialize(ejs, (EjsString*) vp);
    if (vp == ejs->undefinedValue) {
        vp = (EjsVar*) ejs->emptyStringValue;
    }
    ejsUnlockVm(master);
    return vp;
}


static EjsVar *removeCache(Ejs *ejs, EjsWebCache *cp, int argc, EjsVar **argv)
{
    Ejs         *master;
    EjsName     qname;
    cchar       *domain, *key;

    master = ejs->master ? ejs->master : ejs;
    domain = ejsGetString(argv[0]);
    key = ejsGetString(argv[1]);

    ejsLockVm(master);
    ejsDeletePropertyByName(master, cp->cache, ejsName(&qname, domain, key));
    ejsUnlockVm(master);
    return 0;
}


static EjsVar *writeCache(Ejs *ejs, EjsWebCache *cp, int argc, EjsVar **argv)
{
    Ejs         *master;
    EjsName     qname;
    EjsVar      *value;
    cchar       *domain, *key;

    master = ejs->master ? ejs->master : ejs;
    domain = ejsGetString(argv[0]);
    key = ejsGetString(argv[1]);
    ejsLockVm(master);

    value = (EjsVar*) ejsSerialize(master, argv[2], 0, 0, 0);
    ejsSetPropertyByName(master, cp->cache, ejsName(&qname, domain, key), value);
    ejsUnlockVm(master);
    return 0;
}


void ejsConfigureWebCacheType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.web", "Cache"));
    if (type == 0) {
        if (!(ejs->flags & EJS_FLAG_EMPTY)) {
            mprError(ejs, "Can't find Cache class");
            ejs->hasError = 1;
        }
        return;
    }
    type->instanceSize = sizeof(EjsWebCache);
    mprAssert(type->objectBased);

#if ES_ejs_web_Cache_Cache
    ejsBindMethod(ejs, type, ES_ejs_web_Cache_Cache, (EjsNativeFunction) cacheConstructor);
    ejsBindMethod(ejs, type, ES_ejs_web_Cache_read, (EjsNativeFunction) readCache);
    ejsBindMethod(ejs, type, ES_ejs_web_Cache_write, (EjsNativeFunction) writeCache);
    ejsBindMethod(ejs, type, ES_ejs_web_Cache_remove, (EjsNativeFunction) removeCache);
#endif
}

#endif /* BLD_FEATURE_EJS_WEB */

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
