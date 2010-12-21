/**
    ejsPath.c - Path class.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if ES_ejs_io_Path
/************************************ Forwards ********************************/

static char *getPath(Ejs *ejs, EjsVar *vp);

/************************************ Helpers *********************************/

static void destroyPath(Ejs *ejs, EjsPath *pp)
{
    mprAssert(pp);

    mprFree(pp->path);
    pp->path = 0;
    ejsFreeVar(ejs, (EjsVar*) pp, -1);
}


static EjsVar *coercePathOperands(Ejs *ejs, EjsPath *lhs, int opcode,  EjsVar *rhs)
{
    switch (opcode) {
    /*
     *  Binary operators
     */
    case EJS_OP_ADD:
        return ejsInvokeOperator(ejs, (EjsVar*) ejsCreateString(ejs, lhs->path), opcode, rhs);

    case EJS_OP_COMPARE_EQ: case EJS_OP_COMPARE_NE:
    case EJS_OP_COMPARE_LE: case EJS_OP_COMPARE_LT:
    case EJS_OP_COMPARE_GE: case EJS_OP_COMPARE_GT:
        if (ejsIsNull(rhs) || ejsIsUndefined(rhs)) {
            return (EjsVar*) ((opcode == EJS_OP_COMPARE_EQ) ? ejs->falseValue: ejs->trueValue);
        }
        return ejsInvokeOperator(ejs, (EjsVar*) ejsCreateString(ejs, lhs->path), opcode, rhs);

    case EJS_OP_COMPARE_STRICTLY_NE:
        return (EjsVar*) ejs->trueValue;

    case EJS_OP_COMPARE_STRICTLY_EQ:
        return (EjsVar*) ejs->falseValue;

    case EJS_OP_COMPARE_NOT_ZERO:
    case EJS_OP_COMPARE_TRUE:
        return (EjsVar*) ejs->trueValue;

    case EJS_OP_COMPARE_ZERO:
    case EJS_OP_COMPARE_FALSE:
        return (EjsVar*) ejs->falseValue;

    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
        return (EjsVar*) ejs->falseValue;

    default:
        ejsThrowTypeError(ejs, "Opcode %d not valid for type %s", opcode, lhs->obj.var.type->qname.name);
        return ejs->undefinedValue;
    }
    return 0;
}


static EjsVar *invokePathOperator(Ejs *ejs, EjsPath *lhs, int opcode,  EjsPath *rhs, void *data)
{
    EjsVar      *result;

    if (rhs == 0 || lhs->obj.var.type != rhs->obj.var.type) {
        if ((result = coercePathOperands(ejs, lhs, opcode, (EjsVar*) rhs)) != 0) {
            return result;
        }
    }
    /*
     *  Types now match, both paths
     */
    switch (opcode) {
    case EJS_OP_COMPARE_STRICTLY_EQ:
    case EJS_OP_COMPARE_EQ:
        if (lhs == rhs || (lhs->path == rhs->path)) {
            return (EjsVar*) ejs->trueValue;
        }
        return (EjsVar*) ejsCreateBoolean(ejs,  mprStrcmp(lhs->path, rhs->path) == 0);

    case EJS_OP_COMPARE_NE:
    case EJS_OP_COMPARE_STRICTLY_NE:
        return (EjsVar*) ejsCreateBoolean(ejs,  mprStrcmp(lhs->path, rhs->path) != 0);

    case EJS_OP_COMPARE_LT:
        return (EjsVar*) ejsCreateBoolean(ejs,  mprStrcmp(lhs->path, rhs->path) < 0);

    case EJS_OP_COMPARE_LE:
        return (EjsVar*) ejsCreateBoolean(ejs,  mprStrcmp(lhs->path, rhs->path) <= 0);

    case EJS_OP_COMPARE_GT:
        return (EjsVar*) ejsCreateBoolean(ejs,  mprStrcmp(lhs->path, rhs->path) > 0);

    case EJS_OP_COMPARE_GE:
        return (EjsVar*) ejsCreateBoolean(ejs,  mprStrcmp(lhs->path, rhs->path) >= 0);

    /*
     *  Unary operators
     */
    case EJS_OP_COMPARE_NOT_ZERO:
        return (EjsVar*) ((lhs->path) ? ejs->trueValue: ejs->falseValue);

    case EJS_OP_COMPARE_ZERO:
        return (EjsVar*) ((lhs->path == 0) ? ejs->trueValue: ejs->falseValue);


    case EJS_OP_COMPARE_UNDEFINED:
    case EJS_OP_COMPARE_NULL:
    case EJS_OP_COMPARE_FALSE:
    case EJS_OP_COMPARE_TRUE:
        return (EjsVar*) ejs->falseValue;

    /*
     *  Binary operators
     */
    case EJS_OP_ADD:
        return (EjsVar*) ejsCreatePath(ejs, mprJoinPath(ejs, lhs->path, rhs->path));

    default:
        ejsThrowTypeError(ejs, "Opcode %d not implemented for type %s", opcode, lhs->obj.var.type->qname.name);
        return 0;
    }
    mprAssert(0);
}


/************************************ Methods *********************************/
/*
    Constructor

    function Path(path: String)

 */
static EjsVar *pathConstructor(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    mprAssert(argc == 1);
    if ((fp->path = getPath(ejs, argv[0])) == 0) {
        return (EjsVar*) fp;
    }
    fp->path = mprStrdup(fp, fp->path);
    return (EjsVar*) fp;
}


/*
    Return an absolute path name for the file

    function get absolutePath()
 */
static EjsVar *absolutePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetAbsPath(fp, fp->path));
}


/*
    Get when the file was last accessed.

    function get accessed(): Date
 */
static EjsVar *getAccessedDate(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    mprGetPathInfo(ejs, fp->path, &info);
    if (!info.valid) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreateDate(ejs, ((MprTime) info.atime) * 1000);
}


/*
    Get the base name of a file

    function basename(): Path
 */
static EjsVar *getPathBasename(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetPathBase(ejs, fp->path));
}


/*
    Get the path components

    function components(): Array
 */
static EjsVar *getPathComponents(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFileSystem   *fs;
    EjsArray        *ap;
    char            *cp, *last;
    int             index;

    fs = mprLookupFileSystem(ejs, fp->path);
    ap = ejsCreateArray(ejs, 0);
    index = 0;
    for (last = cp = mprGetAbsPath(fp, fp->path); *cp; cp++) {
        if (*cp == fs->separators[0] || *cp == fs->separators[1]) {
            *cp++ = '\0';
            ejsSetProperty(ejs, (EjsVar*) ap, index++, (EjsVar*) ejsCreateString(ejs, last));
            last = cp;
        }
    }
    if (cp > last) {
        ejsSetProperty(ejs, (EjsVar*) ap, index++, (EjsVar*) ejsCreateString(ejs, last));
    }
    return (EjsVar*) ap;
}


/*
    Copy a file

    function copy(to: Object, options: Object = null): Void
 */
static EjsVar *copyPath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFile     *from, *to;
    char        *buf, *toPath;
    uint        bytes;
    int         rc;

    mprAssert(argc == 1);
    if ((toPath = getPath(ejs, argv[0])) == 0) {
        return 0;
    }

    from = mprOpen(ejs, fp->path, O_RDONLY | O_BINARY, 0);
    if (from == 0) {
        ejsThrowIOError(ejs, "Cant open %s", fp->path);
        return 0;
    }

    to = mprOpen(ejs, toPath, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, EJS_FILE_PERMS);
    if (to == 0) {
        ejsThrowIOError(ejs, "Cant create %s", toPath);
        mprFree(from);
        return 0;
    }

    buf = mprAlloc(ejs, MPR_BUFSIZE);
    if (buf == 0) {
        ejsThrowMemoryError(ejs);
        mprFree(to);
        mprFree(from);
        return 0;
    }

    rc = 0;
    while ((bytes = mprRead(from, buf, MPR_BUFSIZE)) > 0) {
        if (mprWrite(to, buf, bytes) != bytes) {
            ejsThrowIOError(ejs, "Write error to %s", toPath);
            rc = 0;
            break;
        }
    }
    mprFree(from);
    mprFree(to);
    mprFree(buf);
    return 0;
}


/*
    Return when the file was created.

    function get created(): Date
 */
static EjsVar *getCreatedDate(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    mprGetPathInfo(ejs, fp->path, &info);
    if (!info.valid) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreateDate(ejs, ((MprTime) info.ctime) * 1000);
}


/**
    Get the directory name portion of a file.

    function get dirname(): Path
 */
static EjsVar *getPathDirname(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetPathDir(ejs, fp->path));
}


/*
    Test to see if this file exists.

    function get exists(): Boolean
 */
static EjsVar *getPathExists(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    return (EjsVar*) ejsCreateBoolean(ejs, mprGetPathInfo(ejs, fp->path, &info) == 0);
}


/*
    Get the file extension portion of the file name.

    function get extension(): String
 */
static EjsVar *getPathExtension(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    char    *cp;

    if ((cp = strrchr(fp->path, '.')) == 0) {
        return (EjsVar*) ejs->emptyStringValue;
    }
    return (EjsVar*) ejsCreateString(ejs, &cp[1]);
}


/*
    Function to iterate and return the next element index.
    NOTE: this is not a method of Array. Rather, it is a callback function for Iterator
 */
static EjsVar *nextPathKey(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsPath     *fp;

    fp = (EjsPath*) ip->target;
    if (!ejsIsPath(fp)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    if (ip->index < mprGetListCount(fp->files)) {
        return (EjsVar*) ejsCreateNumber(ejs, ip->index++);
    }
    ejsThrowStopIteration(ejs);
    return 0;
}


/*
    Return the default iterator for use with "for ... in". This will iterate over the files in a directory.

    iterator native function get(): Iterator
 */
static EjsVar *getPathIterator(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    mprFree(fp->files);
    fp->files = mprGetPathFiles(fp, fp->path, 0);
    return (EjsVar*) ejsCreateIterator(ejs, (EjsVar*) fp, (EjsNativeFunction) nextPathKey, 0, NULL);
}


/*
    Function to iterate and return the next element value.
    NOTE: this is not a method of Array. Rather, it is a callback function for Iterator
 */
static EjsVar *nextPathValue(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsPath     *fp;
    MprDirEntry *dp;

    fp = (EjsPath*) ip->target;
    if (!ejsIsPath(fp)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    if (ip->index < mprGetListCount(fp->files)) {
        dp = (MprDirEntry*) mprGetItem(fp->files, ip->index++);
        return (EjsVar*) ejsCreatePath(ejs, dp->name);
    }
    ejsThrowStopIteration(ejs);
    return 0;
}


/*
    Return an iterator to enumerate the bytes in the file. For use with "for each ..."

    iterator native function getValues(): Iterator
 */
static EjsVar *getPathValues(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    mprFree(fp->files);
    fp->files = mprGetPathFiles(fp, fp->path, 0);
    return (EjsVar*) ejsCreateIterator(ejs, (EjsVar*) fp, (EjsNativeFunction) nextPathValue, 0, NULL);
}


/*
    Get the files in a directory.
    function getFiles(enumDirs: Boolean = false): Array
 */
static EjsVar *getPathFiles(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    EjsArray        *array;
    MprList         *list;
    MprDirEntry     *dp;
    char            *path;
    bool            enumDirs, noPath;
    int             next;

    mprAssert(argc == 0 || argc == 1);
    enumDirs = (argc == 1) ? ejsGetBoolean(argv[0]): 0;

    array = ejsCreateArray(ejs, 0);
    if (array == 0) {
        return 0;
    }
    list = mprGetPathFiles(array, fp->path, enumDirs);
    if (list == 0) {
        ejsThrowIOError(ejs, "Can't read directory");
        return 0;
    }
    noPath = (fp->path[0] == '.' && fp->path[1] == '\0') || 
        (fp->path[0] == '.' && fp->path[1] == '/' && fp->path[2] == '\0');

    for (next = 0; (dp = mprGetNextItem(list, &next)) != 0; ) {
        if (strcmp(dp->name, ".") == 0 || strcmp(dp->name, "..") == 0) {
            continue;
        }
        if (enumDirs || !(dp->isDir)) {
            if (noPath) {
                ejsSetProperty(ejs, (EjsVar*) array, -1, (EjsVar*) ejsCreatePath(ejs, dp->name));
            } else {
                /*
                 *  Prepend the directory name
                 */
                path = mprJoinPath(ejs, fp->path, dp->name);
                ejsSetProperty(ejs, (EjsVar*) array, -1, (EjsVar*) ejsCreatePathAndFree(ejs, path));
            }
        }
    }
    mprFree(list);
    return (EjsVar*) array;
}


/*
    Determine if the file path has a drive spec (C:) in the file name

    static function hasDrive(): Boolean
 */
static EjsVar *pathHasDrive(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, 
        (isalpha((int) fp->path[0]) && fp->path[1] == ':' && (fp->path[2] == '/' || fp->path[2] == '\\')));
}


/*
    function get isAbsolute(): Boolean
 */
static EjsVar *isPathAbsolute(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) (mprIsAbsPath(ejs, fp->path) ? ejs->trueValue: ejs->falseValue);
}


/*
    Determine if the file name is a directory

    function get isDir(): Boolean
 */
static EjsVar *isPathDir(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;
    int         rc;

    rc = mprGetPathInfo(ejs, fp->path, &info);
    return (EjsVar*) ejsCreateBoolean(ejs, rc == 0 && info.isDir);
}


/*
    function get isLink(): Boolean
 */
static EjsVar *isPathLink(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;
    int         rc;

    rc = mprGetPathInfo(ejs, fp->path, &info);
    return (EjsVar*) ejsCreateBoolean(ejs, rc == 0 && info.isLink);
}


/*
    Determine if the file name is a regular file

    function get isRegular(): Boolean
 */
static EjsVar *isPathRegular(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    mprGetPathInfo(ejs, fp->path, &info);

    return (EjsVar*) ejsCreateBoolean(ejs, info.isReg);
}


/*
    function get isRelative(): Boolean
 */
static EjsVar *isPathRelative(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) (mprIsRelPath(ejs, fp->path) ? ejs->trueValue: ejs->falseValue);
}


/*
    Join paths. Returns a normalized path.

    function join(...others): Path
 */
static EjsVar *joinPath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    EjsArray    *args;
    cchar       *other, *join;
    int         i;

    args = (EjsArray*) argv[0];
    join = fp->path;
    for (i = 0; i < args->length; i++) {
        if ((other = getPath(ejs, ejsGetProperty(ejs, (EjsVar*) args, i))) == NULL) {
            return 0;
        }
        join = mprJoinPath(ejs, join, other);
    }
    return (EjsVar*) ejsCreatePath(ejs, join);;
}


/*
    Join extension

    function joinExt(ext: String): Path
 */
static EjsVar *joinPathExt(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    cchar   *ext;
    if (mprGetPathExtension(ejs, fp->path)) {
        return (EjsVar*) fp;
    }
    ext = ejsGetString(argv[0]);
    while (ext && *ext == '.') {
        ext++;
    }
    return (EjsVar*) ejsCreatePath(ejs, mprStrcat(ejs, -1, fp->path, ".", ext, NULL));
}


/*
    Get the length of the path name.

    override intrinsic function get length(): Number
 */
static EjsVar *pathLength(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, strlen(fp->path));
}


static EjsVar *pathLinkTarget(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    char    *path;

    if ((path = mprGetPathLink(ejs, fp->path)) == 0) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetPathLink(ejs, fp->path));
}


/*
    function makeDir(options: Object = null): Void

    Options: permissions, owner, group
 */
static EjsVar *makePathDir(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;
    EjsName     qname;
    EjsVar      *options, *permissions;
    int         perms;
    
    perms = 0755;

    if (argc == 1) {
        if (!ejsIsObject(argv[0])) {
            ejsThrowArgError(ejs, "Bad args");
        }
        options = argv[0];

        if (ejsIsObject(options)) {
            permissions = ejsGetPropertyByName(ejs, options, ejsName(&qname, EJS_PUBLIC_NAMESPACE, "permissions"));
            if (permissions) {
                perms = ejsGetInt(permissions);
            }
        }
    }
    if (mprGetPathInfo(ejs, fp->path, &info) == 0 && info.isDir) {
        return 0;
    }
    if (mprMakeDir(ejs, fp->path, perms, 1) < 0) {
        ejsThrowIOError(ejs, "Cant create directory %s", fp->path);
        return 0;
    }
    return 0;
}


/*
    function makeLink(target: Path, hard: Boolean = false): Void
 */
static EjsVar *makePathLink(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    cchar   *target;
    int     hard;

    target = ((EjsPath*) argv[0])->path;
    hard = (argc >= 2) ? (argv[1] == (EjsVar*) ejs->trueValue) : 0;
    if (mprMakeLink(ejs, fp->path, target, hard) < 0) {
        ejsThrowIOError(ejs, "Can't make link");
    }
    return 0;
}


/*
    Make a temporary file. Creates a new, uniquely named temporary file.

    function makeTemp(): Path
 */
static EjsVar *makePathTemp(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    char    *path;

    if ((path = mprGetTempPath(ejs, fp->path)) == NULL) {
        ejsThrowIOError(ejs, "Can't make temp file");
        return 0;
    }
    return (EjsVar*) ejsCreatePathAndFree(ejs, path);
}


/*
    function map(separator: String): Path
 */
static EjsVar *pa_map(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    cchar   *sep;
    char    *path;
    int     separator;

    sep = ejsGetString(argv[0]);
    separator = *sep ? *sep : '/';
    path = mprStrdup(ejs, fp->path);
    mprMapSeparators(ejs, path, separator);
    return (EjsVar*) ejsCreatePathAndFree(ejs, path);
}


EjsVar *pa_mimeType(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, mprLookupMimeType(ejs, fp->path));
}


/*
    Get when the file was created or last modified.

    function get modified(): Date
 */
static EjsVar *getModifiedDate(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    mprGetPathInfo(ejs, fp->path, &info);
    if (!info.valid) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreateDate(ejs, ((MprTime) info.mtime) * 1000);
}


/*
    Get the name of the path as a string.

    function get name(): String
 */
static EjsVar *getPathName(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, fp->path);
}


/*
    function get natural(): Path
 */
static EjsVar *getNaturalPath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetNativePath(ejs, fp->path));
}


/*
    function get normalize(): Path
 */
static EjsVar *normalizePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetNormalizedPath(ejs, fp->path));
}


/*
    Get the parent directory of the absolute path of the file.

    function get parent(): String
 */
static EjsVar *getPathParent(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetPathParent(fp, fp->path));
}


#if ES_ejs_io_Path_perms
/*
    Get the path permissions

    function get perms(): Number
 */
static EjsVar *getPerms(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    if (mprGetPathInfo(ejs, fp->path, &info) < 0) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreateNumber(ejs, info.perms);
}
#endif


#if ES_ejs_io_Path_set_perms
/*
    Set the path permissions

    function set perms(perms: Number): Void
 */
static EjsVar *setPerms(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
#if !VXWORKS
    int     perms;

    perms = ejsGetInt(argv[0]);
    if (chmod(fp->path, perms) < 0) {
        ejsThrowIOError(ejs, "Can't update permissions for %s", fp->path);
    }
#endif
    return 0;
}
#endif


/*
    Get a portable (unix-like) representation of the path
  
    function get portable(lower: Boolean = false): Path
 */
static EjsVar *getPortablePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    char    *path;
    int     lower;

    lower = (argc >= 1 && argv[0] == (EjsVar*) ejs->trueValue);
    path = mprGetPortablePath(ejs, fp->path);
    if (lower) {
        mprStrLower(path);
    }
    return (EjsVar*) ejsCreatePathAndFree(ejs, path);
}



#if KEEP
/*
    Get the file contents as a byte array

    static function readBytes(path: String): ByteArray
 */
static EjsVar *readBytes(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFile         *file;
    EjsByteArray    *result;
    cchar           *path;
    char            buffer[MPR_BUFSIZE];
    int             bytes, offset, rc;

    mprAssert(argc == 1 && ejsIsString(argv[0]));
    path = ejsGetString(argv[0]);

    file = mprOpen(ejs, path, O_RDONLY | O_BINARY, 0);
    if (file == 0) {
        ejsThrowIOError(ejs, "Can't open %s", path);
        return 0;
    }

    result = ejsCreateByteArray(ejs, (int) mprGetFileSize(file));
    if (result == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }

    rc = 0;
    offset = 0;
    while ((bytes = mprRead(file, buffer, MPR_BUFSIZE)) > 0) {
        if (ejsCopyToByteArray(ejs, result, offset, buffer, bytes) < 0) {
            ejsThrowMemoryError(ejs);
            rc = -1;
            break;
        }
        offset += bytes;
    }
    ejsSetByteArrayPositions(ejs, result, 0, offset);

    mprFree(file);
    return (EjsVar*) result;
}


/**
    Read the file contents as an array of lines.

    static function readLines(path: String): Array
 */
static EjsVar *readLines(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFile     *file;
    MprBuf      *data;
    EjsArray    *result;
    cchar       *path;
    char        *start, *end, *cp, buffer[MPR_BUFSIZE];
    int         bytes, rc, lineno;

    mprAssert(argc == 1 && ejsIsString(argv[0]));
    path = ejsGetString(argv[0]);

    result = ejsCreateArray(ejs, 0);
    if (result == NULL) {
        ejsThrowMemoryError(ejs);
        return 0;
    }

    file = mprOpen(ejs, path, O_RDONLY | O_BINARY, 0);
    if (file == 0) {
        ejsThrowIOError(ejs, "Can't open %s", path);
        return 0;
    }

    data = mprCreateBuf(ejs, 0, (int) mprGetFileSize(file) + 1);
    result = ejsCreateArray(ejs, 0);
    if (result == NULL || data == NULL) {
        ejsThrowMemoryError(ejs);
        mprFree(file);
        return 0;
    }

    rc = 0;
    while ((bytes = mprRead(file, buffer, MPR_BUFSIZE)) > 0) {
        if (mprPutBlockToBuf(data, buffer, bytes) != bytes) {
            ejsThrowMemoryError(ejs);
            rc = -1;
            break;
        }
    }

    start = mprGetBufStart(data);
    end = mprGetBufEnd(data);
    for (lineno = 0, cp = start; cp < end; cp++) {
        if (*cp == '\n') {
            if (ejsSetProperty(ejs, (EjsVar*) result, lineno++, 
                    (EjsVar*) ejsCreateStringWithLength(ejs, start, (int) (cp - start))) < 0) {
                break;
            }
            start = cp + 1;
        } else if (*cp == '\r') {
            start = cp + 1;
        }
    }
    if (cp > start) {
        ejsSetProperty(ejs, (EjsVar*) result, lineno++, (EjsVar*) ejsCreateStringWithLength(ejs, start, (int) (cp - start)));
    }

    mprFree(file);
    mprFree(data);

    return (EjsVar*) result;
}


/**
    Read the file contents as a string

    static function readString(path: String): String
 */
static EjsVar *readFileAsString(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFile     *file;
    MprBuf      *data;
    EjsVar      *result;
    cchar       *path;
    char        buffer[MPR_BUFSIZE];
    int         bytes, rc;

    mprAssert(argc == 1 && ejsIsString(argv[0]));
    path = ejsGetString(argv[0]);

    file = mprOpen(ejs, path, O_RDONLY | O_BINARY, 0);
    if (file == 0) {
        ejsThrowIOError(ejs, "Can't open %s", path);
        return 0;
    }

    data = mprCreateBuf(ejs, 0, (int) mprGetFileSize(file) + 1);
    if (data == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }

    rc = 0;
    while ((bytes = mprRead(file, buffer, MPR_BUFSIZE)) > 0) {
        if (mprPutBlockToBuf(data, buffer, bytes) != bytes) {
            ejsThrowMemoryError(ejs);
            rc = -1;
            break;
        }
    }
    result = (EjsVar*) ejsCreateStringWithLength(ejs, mprGetBufStart(data),  mprGetBufLength(data));
    mprFree(file);
    mprFree(data);
    return result;
}


/*
    Get the file contents as an XML object

    static function readXML(path: String): XML
 */
static EjsVar *readXML(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return 0;
}
#endif

/*
    Return a relative path name for the file.

    function get relativePath(): Path
 */
static EjsVar *relativePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprGetRelPath(fp, fp->path));
}


/*
    Remove the file associated with the File object. This may be a file or directory.

    function remove(): void
 */
static EjsVar *removePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    if (mprGetPathInfo(ejs, fp->path, &info) == 0) {
        if (mprDeletePath(ejs, fp->path) < 0) {
            ejsThrowIOError(ejs, "Cant remove %s", fp->path);
        }
    }
    return 0;
}


/*
    Rename the file

    function rename(to: String): Void
 */
static EjsVar *renamePathFile(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    char    *to;

    mprAssert(argc == 1 && ejsIsString(argv[0]));
    to = ejsGetString(argv[0]);

    unlink(to);
    if (rename(fp->path, to) < 0) {
        ejsThrowIOError(ejs, "Cant rename file %s to %s", fp->path, to);
        return 0;
    }
    return 0;
}


static char *getPathString(Ejs *ejs, EjsVar *vp)
{
    if (ejsIsString(vp)) {
        return (char*) ejsGetString(vp);
    } else if (ejsIsPath(vp)) {
        return ((EjsPath*) vp)->path;
    }
    ejsThrowIOError(ejs, "Bad path");
    return NULL;
}

/*
    Resolve paths against others. Returns a normalized path.
  
    function resolve(...paths): Path
 */
static EjsVar *resolvePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    EjsArray    *args;
    cchar       *next;
    char        *result, *prior;
    int         i;

    args = (EjsArray*) argv[0];
    result = fp->path;
    for (i = 0; i < args->length; i++) {
        if ((next = getPathString(ejs, ejsGetProperty(ejs, (EjsVar*) args, i))) == NULL) {
            return 0;
        }
        prior = result;
        result = mprResolvePath(ejs, prior, next);
        if (prior != fp->path) {
            mprFree(prior);
        }
    }
    return (EjsVar*) ejsCreatePath(ejs, result);
}


/*
    Return true if the paths refer to the same file.

    function same(other: Object): Boolean
 */
static EjsVar *isPathSame(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    cchar   *other;

    if (ejsIsString(argv[0])) {
        other = ejsGetString(argv[0]);
    } else if (ejsIsPath(argv[0])) {
        other = ((EjsPath*) (argv[0]))->path;
    } else {
        return (EjsVar*) ejs->falseValue;
    }
    return (EjsVar*) (mprSamePath(ejs, fp->path, other) ? ejs->trueValue : ejs->falseValue);
}


/*
    function get separator(): String
 */
static EjsVar *pathSeparator(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFileSystem   *fs;
    cchar           *cp;

    if ((cp = mprGetFirstPathSeparator(ejs, fp->path)) != 0) {
        return (EjsVar*) ejsCreateStringAndFree(ejs, mprAsprintf(ejs, -1, "%c", (int) *cp));
    }
    fs = mprLookupFileSystem(ejs, fp->path);
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprAsprintf(ejs, -1, "%c", (int) fs->separators[0]));
}


/*
    Get the size of the file associated with this Path

    intrinsic function get size(): Number
 */
static EjsVar *getPathFileSize(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    if (mprGetPathInfo(ejs, fp->path, &fp->info) < 0) {
        return (EjsVar*) ejs->minusOneValue;
    }
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) fp->info.size);
}


/*
    function toString(): String
 */
static EjsVar *pathToJSON(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    EjsVar  *result;
    MprBuf  *buf;
    int     i, c, len;

    buf = mprCreateBuf(fp, 0, 0);
    len = strlen(fp->path);
    mprPutCharToBuf(buf, '"');
    for (i = 0; i < len; i++) {
        c = fp->path[i];
        if (c == '"' || c == '\\') {
            mprPutCharToBuf(buf, '\\');
            mprPutCharToBuf(buf, c);
        } else {
            mprPutCharToBuf(buf, c);
        }
    }
    mprPutCharToBuf(buf, '"');
    mprAddNullToBuf(buf);
    result = (EjsVar*) ejsCreateString(ejs, mprGetBufStart(buf));
    mprFree(buf);
    return result;
}

/*
    function toString(): String
 */
static EjsVar *pathToString(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, fp->path);
}


/*
    function trimExt(): Path
 */
static EjsVar *trimExt(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePathAndFree(ejs, mprTrimPathExtension(ejs, fp->path));
}


/*
    function truncate(size: Number): Void
 */
static EjsVar *truncatePath(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    int     size;

    size = ejsGetInt(argv[0]);
    if (mprTruncatePath(ejs, fp->path, size) < 0) {
        ejsThrowIOError(ejs, "Cant truncate %s", fp->path);
    }
    return 0;
}


#if KEEP
/*
    Put the file contents

    static function write(path: String, permissions: Number, ...args): void
 */
static EjsVar *writeToFile(Ejs *ejs, EjsPath *fp, int argc, EjsVar **argv)
{
    MprFile     *file;
    EjsArray    *args;
    char        *path, *data;
    int         i, bytes, length, permissions;

    mprAssert(argc == 3);

    path = ejsGetString(argv[0]);
    permissions = ejsGetInt(argv[1]);
    args = (EjsArray*) argv[2];

    /*
     *  Create fails if already present
     */
    mprDeletePath(ejs, path);
    file = mprOpen(ejs, path, O_CREAT | O_WRONLY | O_BINARY, permissions);
    if (file == 0) {
        ejsThrowIOError(ejs, "Cant create %s", path);
        return 0;
    }

    for (i = 0; i < args->length; i++) {
        data = ejsGetString(ejsToString(ejs, ejsGetProperty(ejs, (EjsVar*) args, i)));
        length = (int) strlen(data);
        bytes = mprWrite(file, data, length);
        if (bytes != length) {
            ejsThrowIOError(ejs, "Write error to %s", path);
            break;
        }
    }
    mprFree(file);
    return 0;
}
#endif


static char *getPath(Ejs *ejs, EjsVar *vp)
{
    if (ejsIsString(vp)) {
        return ejsGetString(vp);
    } else if (ejsIsPath(vp)) {
        return ((EjsPath*) vp)->path;
    }
    ejsThrowIOError(ejs, "Bad path");
    return NULL;
}

/*********************************** Factory **********************************/

EjsPath *ejsCreatePath(Ejs *ejs, cchar *path)
{
    EjsPath     *fp;
    EjsVar      *arg;

    fp = (EjsPath*) ejsCreateVar(ejs, ejsGetType(ejs, ES_ejs_io_Path), 0);
    if (fp == 0) {
        return 0;
    }
    arg = (EjsVar*) ejsCreateString(ejs, path);
    pathConstructor(ejs, fp, 1, (EjsVar**) &arg);
    return fp;
}


EjsPath *ejsCreatePathAndFree(Ejs *ejs, char *value)
{
    EjsPath     *path;

    path = ejsCreatePath(ejs, value);
    mprFree(value);
    return path;
}


void ejsCreatePathType(Ejs *ejs)
{
    EjsName     qname;
    EjsType     *type;

    type = ejsCreateCoreType(ejs, ejsName(&qname, "ejs.io", "Path"), ejs->objectType, sizeof(EjsPath), ES_ejs_io_Path,
        ES_ejs_io_Path_NUM_CLASS_PROP, ES_ejs_io_Path_NUM_INSTANCE_PROP, 
        EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_HAS_CONSTRUCTOR | EJS_ATTR_OBJECT_HELPERS);
    type->helpers->invokeOperator = (EjsInvokeOperatorHelper) invokePathOperator;
    type->helpers->destroyVar = (EjsDestroyVarHelper) destroyPath;
}


void ejsConfigurePathType(Ejs *ejs)
{
    EjsType     *type;

    if ((type = ejsGetType(ejs, ES_ejs_io_Path)) == 0) {
        return;
    }

    ejsBindMethod(ejs, type, ES_ejs_io_Path_Path, (EjsNativeFunction) pathConstructor);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_absolute, (EjsNativeFunction) absolutePath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_accessed, (EjsNativeFunction) getAccessedDate);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_basename, (EjsNativeFunction) getPathBasename);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_components, (EjsNativeFunction) getPathComponents);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_copy, (EjsNativeFunction) copyPath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_created, (EjsNativeFunction) getCreatedDate);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_dirname, (EjsNativeFunction) getPathDirname);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_exists, (EjsNativeFunction) getPathExists);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_extension, (EjsNativeFunction) getPathExtension);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_files, (EjsNativeFunction) getPathFiles);
    ejsBindMethod(ejs, type, ES_Object_get, (EjsNativeFunction) getPathIterator);
    ejsBindMethod(ejs, type, ES_Object_getValues, (EjsNativeFunction) getPathValues);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_hasDrive, (EjsNativeFunction) pathHasDrive);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_isAbsolute, (EjsNativeFunction) isPathAbsolute);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_isDir, (EjsNativeFunction) isPathDir);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_isLink, (EjsNativeFunction) isPathLink);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_isRegular, (EjsNativeFunction) isPathRegular);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_isRelative, (EjsNativeFunction) isPathRelative);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_join, (EjsNativeFunction) joinPath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_joinExt, (EjsNativeFunction) joinPathExt);
    ejsBindMethod(ejs, type, ES_Object_length, (EjsNativeFunction) pathLength);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_linkTarget, (EjsNativeFunction) pathLinkTarget);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_makeDir, (EjsNativeFunction) makePathDir);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_makeLink, (EjsNativeFunction) makePathLink);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_makeTemp, (EjsNativeFunction) makePathTemp);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_map, (EjsNativeFunction) pa_map);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_mimeType, (EjsNativeFunction) pa_mimeType);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_modified, (EjsNativeFunction) getModifiedDate);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_name, (EjsNativeFunction) getPathName);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_natural, (EjsNativeFunction) getNaturalPath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_normalize, (EjsNativeFunction) normalizePath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_parent, (EjsNativeFunction) getPathParent);
#if ES_ejs_io_Path_perms
    ejsBindMethod(ejs, type, ES_ejs_io_Path_perms, (EjsNativeFunction) getPerms);
#endif
#if ES_ejs_io_Path_set_perms
    ejsBindMethod(ejs, type, ES_ejs_io_Path_set_perms, (EjsNativeFunction) setPerms);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Path_portable, (EjsNativeFunction) getPortablePath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_relative, (EjsNativeFunction) relativePath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_remove, (EjsNativeFunction) removePath);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_rename, (EjsNativeFunction) renamePathFile);
#if ES_ejs_io_Path_resolve
    ejsBindMethod(ejs, type, ES_ejs_io_Path_resolve, (EjsNativeFunction) resolvePath);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Path_same, (EjsNativeFunction) isPathSame);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_separator, (EjsNativeFunction) pathSeparator);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_size, (EjsNativeFunction) getPathFileSize);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_trimExt, (EjsNativeFunction) trimExt);
    ejsBindMethod(ejs, type, ES_ejs_io_Path_truncate, (EjsNativeFunction) truncatePath);

    ejsBindMethod(ejs, type, ES_Object_toJSON, (EjsNativeFunction) pathToJSON);
    ejsBindMethod(ejs, type, ES_Object_toString, (EjsNativeFunction) pathToString);
}


#endif /* ES_Path */
/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.

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

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
