/*
 *  ejsSystem.c -- System class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/************************************ Methods *********************************/
#if BLD_FEATURE_CMD
#if ES_ejs_sys_System_run
/*
 *  function run(cmd: String): String
 */
static EjsVar *run(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprCmd      *cmd;
    EjsString   *result;
    char        *cmdline, *err, *output;
    int         status;

    mprAssert(argc == 1 && ejsIsString(argv[0]));

    cmd = mprCreateCmd(ejs);
    cmdline = ejsGetString(argv[0]);
    status = mprRunCmd(cmd, cmdline, &output, &err, 0);
    if (status) {
        ejsThrowError(ejs, "Command failed: %s\n\nExit status: %d\n\nError Output: \n%s\nPrevious Output: \n%s\n", 
            cmdline, status, err, output);
        mprFree(cmd);
        return 0;
    }
    result = ejsCreateString(ejs, output);
    mprFree(cmd);
    return (EjsVar*) result;
}
#endif


#if ES_ejs_sys_System_runx
/*
 *  function runx(cmd: String): Void
 */
static EjsVar *runx(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprCmd      *cmd;
    char        *err;
    int         status;

    mprAssert(argc == 1 && ejsIsString(argv[0]));

    cmd = mprCreateCmd(ejs);
    status = mprRunCmd(cmd, ejsGetString(argv[0]), NULL, &err, 0);
    if (status) {
        ejsThrowError(ejs, "Can't run command: %s\nDetails: %s", ejsGetString(argv[0]), err);
        mprFree(err);
    }
    mprFree(cmd);
    return 0;
}
#endif


#if ES_ejs_sys_System_daemon
/*
 *  function daemon(cmd: String): Number
 */
static EjsVar *runDaemon(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprCmd      *cmd;
    int         status, pid;

    mprAssert(argc == 1 && ejsIsString(argv[0]));

    cmd = mprCreateCmd(ejs);
    status = mprRunCmd(cmd, ejsGetString(argv[0]), NULL, NULL, MPR_CMD_DETACH);
    if (status) {
        ejsThrowError(ejs, "Can't run command: %s", ejsGetString(argv[0]));
    }
    pid = cmd->pid;
    mprFree(cmd);
    return (EjsVar*) ejsCreateNumber(ejs, pid);
}
#endif


#if ES_ejs_sys_System_exec
/*
 *  function exec(cmd: String): Void
 */
static EjsVar *execCmd(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
#if BLD_UNIX_LIKE
    char    **argVector;
    int     argCount;

    mprMakeArgv(ejs, NULL, ejsGetString(argv[0]), &argCount, &argVector);
    execv(argVector[0], argVector);
#endif
    ejsThrowStateError(ejs, "Can't exec %s", ejsGetString(argv[0]));
    return 0;
}
#endif
#endif

/*
 *  function get hostname(): String
 */
static EjsVar *system_hostname(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprStrdup(ejs, mprGetHostName(ejs)));
}


/************************************ Factory *********************************/

void ejsCreateSystemType(Ejs *ejs)
{
    EjsName     qname;

    ejsCreateCoreType(ejs, ejsName(&qname, "ejs.sys", "System"), ejs->objectType, sizeof(EjsObject), ES_ejs_sys_System,
        ES_ejs_sys_System_NUM_CLASS_PROP, ES_ejs_sys_System_NUM_INSTANCE_PROP, EJS_ATTR_NATIVE | EJS_ATTR_OBJECT_HELPERS);
}


void ejsConfigureSystemType(Ejs *ejs)
{
    EjsType         *type;

    type = ejsGetType(ejs, ES_ejs_sys_System);

#if BLD_FEATURE_CMD
    ejsBindMethod(ejs, type, ES_ejs_sys_System_daemon, (EjsNativeFunction) runDaemon);
    ejsBindMethod(ejs, type, ES_ejs_sys_System_exec, (EjsNativeFunction) execCmd);
    ejsBindMethod(ejs, type, ES_ejs_sys_System_run, (EjsNativeFunction) run);
    ejsBindMethod(ejs, type, ES_ejs_sys_System_runx, (EjsNativeFunction) runx);
    ejsBindMethod(ejs, type, ES_ejs_sys_System_hostname, (EjsNativeFunction) system_hostname);
#endif
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
