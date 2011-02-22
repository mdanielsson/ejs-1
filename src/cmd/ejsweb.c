/**
 *  ejsweb.c - Ejs Web Framework Application Manager and Generator
 *
 *  This command is now deprecated. Windows uses ejs.cmd.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"
#include    "ecCompiler.h"

#if BLD_FEATURE_EJS_WEB
/************************************ Code ************************************/

#if BLD_APPWEB_PRODUCT
    MAIN(ajswebMain, int argc, char **argv)
#else
    MAIN(ejswebMain, int argc, char **argv)
#endif
{
    EjsService  *vm;
    EcCompiler  *cp;
    Ejs         *ejs;
    Mpr         *mpr;
    int         rc;

    mpr = mprCreate(argc, argv, NULL);

    if (mprStart(mpr, 0) < 0) {
        mprError(mpr, "Can't start mpr services");
        return MPR_ERR_CANT_INITIALIZE;
    }
    if ((vm = ejsCreateService(mpr)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    ecInitCompiler(vm);
    if ((ejs = ejsCreate(vm, NULL, NULL, 0)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    ejs->argc = argc;
    ejs->argv = argv;

    if ((cp = ecCreateCompiler(ejs, EC_FLAGS_BIND, EJS_SPEC_FIXED)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    if ((rc = ejsLoadModule(ejs, EJS_EJSWEB_MOD, -1, -1, 0, NULL)) < 0) {
        ejsReportError(ejs, EJS_EJSWEB);
        rc = EJS_ERR;
    }
#if VXWORKS
    mprFree(ejs);
    if (mprStop(mpr)) {
        mprFree(mpr);
    }
#endif
    return (rc == 0) ? 0 : 1;
}

#endif /* BLD_FEATURE_EJS_WEB */

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
