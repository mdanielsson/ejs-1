/**
    ecCompile.c - Interface to the compiler

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"
#include    "ecCompiler.h"

/***************************** Forward Declarations ***************************/

static EjsVar   *loadScriptLiteral(Ejs *ejs, cchar *script);
static EjsVar   *loadScriptFile(Ejs *ejs, cchar *path);

/************************************ Code ************************************/

int ecInitCompiler(EjsService *service)
{
    service->loadScriptLiteral = loadScriptLiteral;
    service->loadScriptFile = loadScriptFile;
    return 0;
}


/*
    Load a script file. This indirect routine is used by the core VM to compile a file when required.
 */
static EjsVar *loadScriptFile(Ejs *ejs, cchar *path)
{
    if (ejsLoadScriptFile(ejs, path, EC_FLAGS_RUN | EC_FLAGS_NO_OUT | EC_FLAGS_DEBUG | EC_FLAGS_BIND | EC_FLAGS_THROW) < 0) {
        return 0;
    }
    return ejs->result;
}


/*
    Function for ejs->loadScriptLiteral. This indirect routine is used by the core VM to compile a script when required.
 */
static EjsVar *loadScriptLiteral(Ejs *ejs, cchar *path)
{
    if (ejsLoadScriptLiteral(ejs, path, EC_FLAGS_RUN | EC_FLAGS_NO_OUT | EC_FLAGS_DEBUG | EC_FLAGS_BIND | EC_FLAGS_THROW) < 0) {
        return 0;
    }
    return ejs->result;
}


/*
    Load and initialize a script file
 */
int ejsLoadScriptFile(Ejs *ejs, cchar *path, int flags)
{
    EcCompiler      *ec;

    if ((ec = ecCreateCompiler(ejs, flags, BLD_FEATURE_EJS_LANG)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    if (ecCompile(ec, 1, (char**) &path, 0) < 0) {
        if (flags & EC_FLAGS_THROW) {
            ejsThrowSyntaxError(ejs, "%s", ec->errorMsg ? ec->errorMsg : "Can't parse script");
        }
        mprFree(ec);
        return EJS_ERR;
    }
    mprFree(ec);
    if (ejsRun(ejs) < 0) {
        return EJS_ERR;
    }
    return 0;
}


/*
    Load and initialize a script literal
 */
int ejsLoadScriptLiteral(Ejs *ejs, cchar *script, int flags)
{
    EcCompiler      *ec;
    cchar           *path;

    if ((ec = ecCreateCompiler(ejs, EC_FLAGS_RUN | EC_FLAGS_NO_OUT | EC_FLAGS_DEBUG | EC_FLAGS_BIND,
            BLD_FEATURE_EJS_LANG)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    if (ecOpenMemoryStream(ec->lexer, (uchar*) script, (int) strlen(script)) < 0) {
        mprError(ejs, "Can't open memory stream");
        mprFree(ec);
        return EJS_ERR;
    }
    path = "__script__";
    if (ecCompile(ec, 1, (char**) &path, 0) < 0) {
        if (flags & EC_FLAGS_THROW) {
            ejsThrowSyntaxError(ejs, "%s", ec->errorMsg ? ec->errorMsg : "Can't parse script");
        }
        mprFree(ec);
        return EJS_ERR;
    }
    ecCloseStream(ec->lexer);
    if (ejsRun(ejs) < 0) {
        mprFree(ec);
        return EJS_ERR;
    }
    mprFree(ec);
    return 0;
}


/*
    One-line embedding. Evaluate a file. This will compile and interpret the given Ejscript source file.
 */
int ejsEvalFile(cchar *path)
{
    EjsService      *vm;   
    Ejs             *ejs;
    Mpr             *mpr;

    mpr = mprCreate(0, NULL, NULL);
    if ((vm = ejsCreateService(mpr)) == 0) {
        mprFree(mpr);
        return MPR_ERR_NO_MEMORY;
    }
    if ((ejs = ejsCreate(vm, NULL, NULL, 0)) == 0) {
        mprFree(mpr);
        return MPR_ERR_NO_MEMORY;
    }
    if (ejsLoadScriptFile(ejs, path, EC_FLAGS_RUN | EC_FLAGS_NO_OUT | EC_FLAGS_DEBUG | EC_FLAGS_BIND) == 0) {
        ejsReportError(ejs, "Error in program");
        mprFree(mpr);
        return MPR_ERR;
    }
    mprFree(mpr);
    return 0;
}


/*
    One-line embedding. Evaluate a script. This will compile and interpret the given script.
 */
int ejsEvalScript(cchar *script)
{
    EjsService      *vm;   
    Ejs             *ejs;
    Mpr             *mpr;

    mpr = mprCreate(0, NULL, NULL);
    if ((vm = ejsCreateService(mpr)) == 0) {
        mprFree(mpr);
        return MPR_ERR_NO_MEMORY;
    }
    if ((ejs = ejsCreate(vm, NULL, NULL, 0)) == 0) {
        mprFree(mpr);
        return MPR_ERR_NO_MEMORY;
    }
    if (ejsLoadScriptLiteral(ejs, script, 
            EC_FLAGS_RUN | EC_FLAGS_NO_OUT | EC_FLAGS_DEBUG | EC_FLAGS_BIND) == 0) {
        ejsReportError(ejs, "Error in program");
        mprFree(mpr);
        return MPR_ERR;
    }
    mprFree(mpr);
    return 0;
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
