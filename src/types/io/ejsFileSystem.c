/**
 *  ejsFileSystem.c - FileSystem class.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if ES_ejs_io_FileSystem
/************************************ Methods *********************************/
/*
 *  Constructor
 *
 *  function FileSystem(path: String)
 */
static EjsVar *fileSystemConstructor(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    char    *path;

    mprAssert(argc == 1 && ejsIsString(argv[0]));

    path = ejsGetString(argv[0]);
    fp->path = mprGetNormalizedPath(fp, path);
    fp->fs = mprLookupFileSystem(ejs, path);
    return (EjsVar*) fp;
}


#if ES_ejs_io_space
/*
 *  Return the amount of free space in the file system that would contain the given path.
 *  function freeSpace(path: String = null): Number
 */
static EjsVar *fileSystemSpace(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
#if BREW
    Mpr     *mpr;
    uint    space;

    mpr = mprGetMpr(ejs);
    space = IFILEMGR_GetFreeSpace(mpr->fileMgr, 0);
    ejsSetReturnValueToInteger(ejs, space);
#endif
    return 0;
}
#endif


/*
 *  Determine if the file system has a drive specs (C:) in paths
 *
 *  static function hasDrives(): Boolean
 */
static EjsVar *hasDrives(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, fp->fs->hasDriveSpecs);
}


#if ES_ejs_io_isReady
/*
 *  Determine if the file system is ready for I/O
 *
 *  function get isReady(): Boolean
 */
static EjsVar *isReady(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    MprPath     info;
    int         rc;

    rc = mprGetPathInfo(ejs, fp->path, &info);
    return (EjsVar*) ejsCreateBoolean(ejs, rc == 0 && info.isDir);
}
#endif


#if ES_ejs_io_isWritable
static EjsVar *isWritable(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
{
    MprPath     info;
    int         rc;

    rc = mprGetPathInfo(ejs, fp->path, &info);
    return (EjsVar*) ejsCreateBoolean(ejs, rc == 0 && info.isDir);
}
#endif


/*
 *  Get the newline characters
 *
 *  function get newline(): String
 */
static EjsVar *getNewline(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, mprGetPathNewline(ejs, fp->path));
}


/*
 *  set the newline characters
 *
 *  function set newline(terminator: String): Void
 */
static EjsVar *setNewline(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    mprAssert(ejsIsString(argv[0]));
    mprSetPathNewline(ejs, fp->path, ((EjsString*) argv[0])->value);
    return 0;
}


static EjsVar *root(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    char    *path, *cp;
    int     sep;

    sep = mprGetPathSeparator(fp, fp->path);
    path = mprGetAbsPath(ejs, fp->path);
    if ((cp = strchr(path, sep)) != 0) {
        *++cp = '\0';
    }
    return (EjsObj*) ejsCreatePathAndFree(ejs, path);
}


/*
 *  Return the path directory separators
 *
 *  static function get separators(): String
 */
static EjsVar *getSeparators(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, fp->fs->separators);
}


/*
 *  Set the path directory separators
 *
 *  static function set separators(value: String): void
 */
static EjsVar *setSeparators(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    mprAssert(argc == 1 && ejsIsString(argv[0]));
    mprSetPathSeparators(ejs, fp->path, ejsGetString(argv[0]));
    return 0;
}


#if ES_ejs_io_size
static EjsVar *size(Ejs *ejs, EjsFileSystem *fp, int argc, EjsVar **argv)
{
    return 0;
}
#endif

/*********************************** Factory **********************************/

EjsFileSystem *ejsCreateFileSystem(Ejs *ejs, cchar *path)
{
    EjsFileSystem   *fs;
    EjsVar          *arg;

    fs = (EjsFileSystem*) ejsCreateVar(ejs, ejsGetType(ejs, ES_ejs_io_FileSystem), 0);
    if (fs == 0) {
        return 0;
    }

    arg = (EjsVar*) ejsCreateString(ejs, path);
    fileSystemConstructor(ejs, fs, 1, (EjsVar**) &arg);
    return fs;
}


void ejsCreateFileSystemType(Ejs *ejs)
{
    EjsName     qname;

    ejsCreateCoreType(ejs, ejsName(&qname, "ejs.io", "FileSystem"), ejs->objectType, sizeof(EjsFileSystem), 
        ES_ejs_io_FileSystem, ES_ejs_io_FileSystem_NUM_CLASS_PROP, ES_ejs_io_FileSystem_NUM_INSTANCE_PROP, 
        EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_HAS_CONSTRUCTOR | EJS_ATTR_OBJECT_HELPERS);
}


void ejsConfigureFileSystemType(Ejs *ejs)
{
    EjsType     *type;

    if ((type = ejsGetType(ejs, ES_ejs_io_FileSystem)) == 0) {
        return;
    }

    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_FileSystem, (EjsNativeFunction) fileSystemConstructor);
#if ES_ejs_io_space
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_space, (EjsNativeFunction) fileSystemSpace);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_hasDrives, (EjsNativeFunction) hasDrives);
#if ES_ejs_io_isReady
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_isReady, (EjsNativeFunction) isReady);
#endif
#if ES_ejs_io_isWritable
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_isWritable, (EjsNativeFunction) isWritable);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_newline, (EjsNativeFunction) getNewline);
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_set_newline, (EjsNativeFunction) setNewline);
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_root, (EjsNativeFunction) root);
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_separators, (EjsNativeFunction) getSeparators);
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_set_separators, (EjsNativeFunction) setSeparators);
#if ES_ejs_io_size
    ejsBindMethod(ejs, type, ES_ejs_io_FileSystem_size, (EjsNativeFunction) size);
#endif
}

#endif /* ES_FileSystem */
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
    vim: sw=8 ts=8 expandtab

    @end
 */
