/*
 *  ejsApp.c -- App class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/*********************************** Methods **********************************/
/*
 *  Get the application command line arguments
 *
 *  static function get args(): String
 */
static EjsVar *getArgs(Ejs *ejs, EjsObject *unused, int argc, EjsVar **argv)
{
    EjsArray    *args;
    int         i;

    args = ejsCreateArray(ejs, ejs->argc);
    for (i = 0; i < ejs->argc; i++) {
        ejsSetProperty(ejs, (EjsVar*) args, i, (EjsVar*) ejsCreateString(ejs, ejs->argv[i]));
    }
    return (EjsVar*) args;
}


/*
 *  Get the current working directory
 *
 *  function get dir(): Path
 */
static EjsVar *currentDir(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetCurrentPath(ejs));
}


/*
 *  Set the current working directory
 *
 *  function chdir(value: String|Path): void
 */
static EjsVar *changeCurrentDir(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    char    *path;

    mprAssert(argc == 1);

    if (ejsIsPath(argv[0])) {
        path = ((EjsPath*) argv[0])->path;
    } else if (ejsIsString(argv[0])) {
        path = ejsGetString(argv[0]);
    } else {
        ejsThrowIOError(ejs, "Bad path");
        return NULL;
    }

    if (chdir(path) < 0) {
        ejsThrowIOError(ejs, "Can't change the current directory");
    }
    return 0;
}

/*
 *  Get an environment var
 *
 *  function getenv(key: String): String
 */
static EjsVar *getEnvVar(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{
    cchar   *value;

    value = getenv(ejsGetString(argv[0]));
    if (value == 0) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreateString(ejs, value);
}


/*
 *  Put an environment var
 *
 *  function putenv(key: String, value: String): void
 */
static EjsVar *putEnvVar(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{

#if !WINCE
#if BLD_UNIX_LIKE
    char    *key, *value;

    key = mprStrdup(ejs, ejsGetString(argv[0]));
    value = mprStrdup(ejs, ejsGetString(argv[1]));
    setenv(key, value, 1);
#else
    char   *cmd;

    cmd = mprStrcat(app, -1, ejsGetString(argv[0]), "=", ejsGetString(argv[1]), NULL);
    putenv(cmd);
#endif
#endif
    return 0;
}


static EjsVar *getErrorStream(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateFileFromFd(ejs, 2, "stderr", O_WRONLY);
}


#if ES_ejs_sys_App_exeDir
/*
 *  Get the directory containing the application's executable file.
 *
 *  static function get exeDir(): Path
 */
static EjsVar *exeDir(Ejs *ejs, EjsObject *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePath(ejs, mprGetAppDir(ejs));
}
#endif


#if ES_ejs_sys_App_exePath
/*
 *  Get the application's executable filename.
 *
 *  static function get exePath(): Path
 */
static EjsVar *exePath(Ejs *ejs, EjsObject *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePath(ejs, mprGetAppPath(ejs));
}
#endif


/**
 *  Exit the application
 *
 *  static function exit(status: Number): void
 */
static EjsVar *exitApp(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    int     status;

    status = argc == 0 ? 0 : ejsGetInt(argv[0]);
    mprBreakpoint();
    if (status != 0) {
        exit(status);
    } else {
        mprTerminate(mprGetMpr(ejs), 1);
    }
    return 0;
}


static EjsVar *getInputStream(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateFileFromFd(ejs, 0, "stdin", O_RDONLY);
}


/**
 *  Control if the application will exit when the last script completes.
 *
 *  static function noexit(exit: Boolean): void
 */
static EjsVar *noexit(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    ejs->flags |= EJS_FLAG_NOEXIT;
    return 0;
}


/*
 *  Get the ejs module search path (EJSPATH). Does not actually read the environment.
 *
 *  function get searchPath(): String
 */
static EjsVar *getSearchPath(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, ejs->ejsPath);
}


/*
 *  Set the ejs module search path (EJSPATH). Does not actually update the environment.
 *
 *  function set searchPath(path: String): Void
 */
static EjsVar *setSearchPath(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{
    ejsSetSearchPath(ejs, ejsGetString(argv[0]));
    return 0;
}


static EjsVar *getOutputStream(Ejs *ejs, EjsObject *app, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateFileFromFd(ejs, 1, "stdout", O_WRONLY);
}


EjsVar *ejsServiceEvents(Ejs *ejs, int count, int timeout, int flags)
{
    MprTime     mark, remaining;
    int         rc;

    if (count < 0) {
        count = MAXINT;
    }
    if (timeout < 0) {
        timeout = MAXINT;
    }
    mark = mprGetTime(ejs);
    do {
        rc = mprServiceEvents(ejs->dispatcher, timeout, MPR_SERVICE_EVENTS | MPR_SERVICE_ONE_THING);
        if (rc > 0) {
            count -= rc;
        }
        remaining = mprGetRemainingTime(ejs, mark, timeout);
    } while (count > 0 && remaining > 0 && !mprIsExiting(ejs) && !ejs->exiting);
    return 0;
}


/*
 *  static function serviceEvents(count: Number = -1, timeout: Number = -1): void
 */
static EjsVar *serviceEvents(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    int     count, timeout;

    count = (argc > 1) ? ejsGetInt(argv[0]) : MAXINT;
    timeout = (argc > 1) ? ejsGetInt(argv[1]) : MAXINT;
    ejsServiceEvents(ejs, count, timeout, MPR_SERVICE_EVENTS | MPR_SERVICE_ONE_THING);
    return 0;
}


#if ES_ejs_sys_App_sleep
/**
 *  Pause the application
 *
 *  static function sleep(delay: Number = -1): void
 */
static EjsVar *sleepProc(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    int     timeout;

    timeout = (argc > 0) ? ejsGetInt(argv[0]): MAXINT;
    ejsServiceEvents(ejs, -1, timeout, MPR_SERVICE_EVENTS | MPR_SERVICE_ONE_THING);
    return 0;
}
#endif


/*********************************** Factory **********************************/

void ejsCreateAppType(Ejs *ejs)
{
    EjsName     qname;

    ejsCreateCoreType(ejs, ejsName(&qname, "ejs.sys", "App"), ejs->objectType, sizeof(EjsObject), ES_ejs_sys_App,
        ES_ejs_sys_App_NUM_CLASS_PROP, ES_ejs_sys_App_NUM_INSTANCE_PROP, EJS_ATTR_NATIVE | EJS_ATTR_OBJECT_HELPERS);
}


void ejsConfigureAppType(Ejs *ejs)
{
    EjsType         *type;

    type = ejsGetType(ejs, ES_ejs_sys_App);

    ejsBindMethod(ejs, type, ES_ejs_sys_App_args, (EjsNativeFunction) getArgs);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_dir, (EjsNativeFunction) currentDir);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_chdir, (EjsNativeFunction) changeCurrentDir);
#if ES_ejs_sys_App_exeDir
    ejsBindMethod(ejs, type, ES_ejs_sys_App_exeDir, (EjsNativeFunction) exeDir);
#endif
#if ES_ejs_sys_App_exePath
    ejsBindMethod(ejs, type, ES_ejs_sys_App_exePath, (EjsNativeFunction) exePath);
#endif
    ejsBindMethod(ejs, type, ES_ejs_sys_App_exit, (EjsNativeFunction) exitApp);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_errorStream, (EjsNativeFunction) getErrorStream);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_getenv, (EjsNativeFunction) getEnvVar);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_putenv, (EjsNativeFunction) putEnvVar);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_inputStream, (EjsNativeFunction) getInputStream);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_noexit, (EjsNativeFunction) noexit);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_outputStream, (EjsNativeFunction) getOutputStream);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_searchPath, (EjsNativeFunction) getSearchPath);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_set_searchPath, (EjsNativeFunction) setSearchPath);
    ejsBindMethod(ejs, type, ES_ejs_sys_App_serviceEvents, (EjsNativeFunction) serviceEvents);
#if ES_ejs_sys_App_sleep
    ejsBindMethod(ejs, type, ES_ejs_sys_App_sleep, (EjsNativeFunction) sleepProc);
#endif
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
