/**
 *  ejsModule.c - Ejscript module management
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/******************************************************************************/

EjsModule *ejsCreateModule(Ejs *ejs, cchar *name, int version)
{
    EjsModule   *mp;

    mprAssert(version >= 0);

    mp = (EjsModule*) mprAllocZeroed(ejs, sizeof(EjsModule));
    if (mp == 0) {
        mprAssert(mp);
        return 0;
    }
    mp->name = mprStrdup(mp, name);
    mp->version = version;
    if (version) {
        mp->vname = mprAsprintf(mp, -1, "%s-%d", name, version);
    } else {
        mp->vname = mp->name;
    }

    mp->constants = mprAllocZeroed(mp, sizeof(EjsConst));
    if (mp->constants == 0) {
        return 0;
    }
    mp->constants->table = mprCreateHash(mp->constants, 0);
    return mp;
}


int ejsSetModuleConstants(Ejs *ejs, EjsModule *mp, cchar *pool, int poolSize)
{
    mprStealBlock(mp, pool);
    mp->constants->pool = (char*) pool;
    mp->constants->size = poolSize;
    mp->constants->len = poolSize;
    return 0;
}


/*
 *  Lookup a module name in the set of loaded modules
 *  If minVersion is <= 0, then any version up to, but not including maxVersion is acceptable.
 *  If maxVersion is < 0, then any version greater than minVersion is acceptable.
 *  If both are zero, then match the name itself and ignore minVersion and maxVersion
 *  If both are -1, then any version is acceptable.
 *  If both are equal, then only that version is acceptable.
 */
EjsModule *ejsLookupModule(Ejs *ejs, cchar *name, int minVersion, int maxVersion)
{
    EjsModule   *mp, *best;
    int         next;

    if (maxVersion < 0) {
        maxVersion = MAXINT;
    }
    best = 0;
    for (next = 0; (mp = (EjsModule*) mprGetNextItem(ejs->modules, &next)) != 0; ) {
        if ((minVersion == 0 && maxVersion == 0) || (minVersion <= mp->version && mp->version <= maxVersion)) {
            if (strcmp(mp->name, name) == 0) {
                if (best == 0 || best->version < mp->version) {
                    best = mp;
                }
            }
        }
    }
    return best;
}


int ejsAddModule(Ejs *ejs, EjsModule *mp)
{
    mprAssert(ejs->modules);
    return mprAddItem(ejs->modules, mp);
}


int ejsRemoveModule(Ejs *ejs, EjsModule *mp)
{
    mprAssert(ejs->modules);
    return mprRemoveItem(ejs->modules, mp);
}


MprList *ejsGetModuleList(Ejs *ejs)
{
    return ejs->modules;
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
