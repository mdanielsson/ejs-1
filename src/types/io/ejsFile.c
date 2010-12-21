/**
 *  ejsFile.c - File class.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/********************************** Defines ***********************************/

#if BLD_WIN_LIKE
#define isDelim(fp, c)  (c == '/' || c == fp->delimiter)
#else
#define isDelim(fp, c)  (c == fp->delimiter)
#endif

#define FILE_OPEN           0x1
#define FILE_READ           0x2
#define FILE_WRITE          0x4

/**************************** Forward Declarations ****************************/

static int mapMode(cchar *mode);
static EjsVar *openFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv);
static int  readData(Ejs *ejs, EjsFile *fp, EjsByteArray *ap, int offset, int count);

/************************************ Helpers *********************************/

static void destroyFile(Ejs *ejs, EjsFile *fp)
{
    mprAssert(fp);

    mprFree(fp->path);
    fp->path = 0;
    ejsFreeVar(ejs, (EjsVar*) fp, -1);
}


/*
 *  Index into a file and extract a byte. This is random access reading.
 */
static EjsVar *getFileProperty(Ejs *ejs, EjsFile *fp, int slotNum)
{
    int     c, offset;

    if (!(fp->mode & FILE_OPEN)) {
        ejsThrowIOError(ejs, "File is not open");
        return 0;
    }
#if KEEP
    if (fp->mode & FILE_READ) {
        if (slotNum >= fp->info.size) {
            ejsThrowOutOfBoundsError(ejs, "Bad file index");
            return 0;
        }
    }
    if (slotNum < 0) {
        ejsThrowOutOfBoundsError(ejs, "Bad file index");
        return 0;
    }
#endif

    offset = mprSeek(fp->file, SEEK_CUR, 0);
    if (offset != slotNum) {
        if (mprSeek(fp->file, SEEK_SET, slotNum) != slotNum) {
            ejsThrowIOError(ejs, "Can't seek to file offset");
            return 0;
        }
    }
    c = mprPeekc(fp->file);
    if (c < 0) {
        ejsThrowIOError(ejs, "Can't read file");
        return 0;
    }
    return (EjsVar*) ejsCreateNumber(ejs, c);
}


/*
 *  Set a byte in the file at the offset designated by slotNum.
 */
static int setFileProperty(Ejs *ejs, EjsFile *fp, int slotNum, EjsVar *value)
{
    int     c, offset;

    if (!(fp->mode & FILE_OPEN)) {
        ejsThrowIOError(ejs, "File is not open");
        return 0;
    }
    if (!(fp->mode & FILE_WRITE)) {
        ejsThrowIOError(ejs, "File is not opened for writing");
        return 0;
    }
    c = ejsIsNumber(value) ? ejsGetInt(value) : ejsGetInt(ejsToNumber(ejs, value));

    offset = mprSeek(fp->file, SEEK_CUR, 0);
    if (slotNum < 0) {
        //  could have an mprGetPosition(file) API
        slotNum = offset;
    }

    if (offset != slotNum && mprSeek(fp->file, SEEK_SET, slotNum) != slotNum) {
        ejsThrowIOError(ejs, "Can't seek to file offset");
        return 0;
    }
    if (mprPutc(fp->file, c) < 0) {
        ejsThrowIOError(ejs, "Can't write file");
        return 0;
    }
    return slotNum;
}


/************************************ Methods *********************************/

int ejsGetNumOption(Ejs *ejs, EjsVar *options, cchar *field, int defaultValue, bool optional)
{
    EjsVar      *vp;
    EjsName     qname;
    EjsNumber   *num;

    if (!ejsIsObject(options)) {
        if (!ejs->exception) {
            ejsThrowArgError(ejs, "Bad args. Options not an object");
        }
        return 0;
    }
    vp = ejsGetPropertyByName(ejs, options, ejsName(&qname, "", field));
    if (vp == 0) {
        if (optional) {
            return defaultValue;
        }
        ejsThrowArgError(ejs, "Required option \"%s\" is missing", field);
        return 0;
    }
    num = ejsToNumber(ejs, vp);
    if (!ejsIsNumber(num)) {
        ejsThrowArgError(ejs, "Bad option type for field \"%s\"", field);
        return 0;
    }
    return (int) num->value;
}


cchar *ejsGetStrOption(Ejs *ejs, EjsVar *options, cchar *field, cchar *defaultValue, bool optional)
{
    EjsVar      *vp;
    EjsName     qname;
    EjsString   *str;

    if (!ejsIsObject(options)) {
        if (!ejs->exception) {
            ejsThrowArgError(ejs, "Bad args. Options not an object");
        }
        return 0;
    }
    vp = ejsGetPropertyByName(ejs, options, ejsName(&qname, "", field));
    if (vp == 0) {
        if (optional) {
            return defaultValue;
        }
        ejsThrowArgError(ejs, "Required option %s is missing", field);
        return 0;
    }
    str = ejsToString(ejs, vp);
    if (!ejsIsString(str)) {
        ejsThrowArgError(ejs, "Bad option type for field \"%s\"", field);
        return 0;
    }
    return str->value;
}


/*
 *  Constructor
 *
 *  function File(path: Object, options: Object = null)
 */
static EjsVar *fileConstructor(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    char    *path;

    if (argc < 1 || argc > 2) {
        ejsThrowArgError(ejs, "Bad args");
        return 0;
    }
    if (ejsIsPath(argv[0])) {
        path = ((EjsPath*) argv[0])->path;
    } else if (ejsIsString(argv[0])) {
        path = ejsGetString(argv[0]);
    } else {
        ejsThrowIOError(ejs, "Bad path");
        return NULL;
    }
    fp->path = mprGetNormalizedPath(fp, path);
    if (argc == 2) {
        openFile(ejs, fp, 1, &argv[1]);
    }
    return (EjsVar*) fp;
}


/*
 *  function get canRead(): Boolean
 */
static EjsVar *canReadFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, fp->mode & FILE_OPEN && (fp->mode & FILE_READ));
}


/*
 *  function get canRead(): Boolean
 */
static EjsVar *canWriteFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, fp->mode & FILE_OPEN && (fp->mode & FILE_WRITE));
}

/*
 *  Close the file and free up all associated resources.
 *
 *  function close(graceful: Boolean): void
 */
static EjsVar *closeFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    if (fp->mode & FILE_OPEN && fp->mode & FILE_WRITE) {
        if (mprFlush(fp->file) < 0) {
            ejsThrowIOError(ejs, "Can't flush file data");
            return 0;
        }
    }

    if (fp->file) {
        mprFree(fp->file);
        fp->file = 0;
    }
    fp->mode = 0;
    mprFree(fp->modeString);
    fp->modeString = 0;
    return 0;
}


/*
 *  Flush the stream. This is a noop as all I/O is unbuffered.
 *
 *  function flush(): void
 */
static EjsVar *flushFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
#if KEEP
    if (!(fp->mode & FILE_OPEN)) {
        ejsThrowIOError(ejs, "File is not open");
        return 0;
    }
    if (!(fp->mode & FILE_WRITE)) {
        ejsThrowIOError(ejs, "File is not opened for writing");
        return 0;
    }
    if (mprFlush(fp->file) < 0) {
        ejsThrowIOError(ejs, "Can't flush file data");
        return 0;
    }
#endif
    return 0;
}


/*
 *  Function to iterate and return the next element index.
 *  NOTE: this is not a method of Array. Rather, it is a callback function for Iterator
 */
static EjsVar *nextKey(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsFile     *fp;

    fp = (EjsFile*) ip->target;
    if (!ejsIsFile(fp)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    if (ip->index < fp->info.size) {
        return (EjsVar*) ejsCreateNumber(ejs, ip->index++);
    }

    ejsThrowStopIteration(ejs);
    return 0;
}


/*
 *  Return the default iterator for use with "for ... in". This returns byte offsets in the file.
 *
 *  iterator native function get(): Iterator
 */
static EjsVar *getFileIterator(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    mprGetPathInfo(ejs, fp->path, &fp->info);
    return (EjsVar*) ejsCreateIterator(ejs, (EjsVar*) fp, (EjsNativeFunction) nextKey, 0, NULL);
}


/*
 *  Function to iterate and return the next element value.
 *  NOTE: this is not a method of Array. Rather, it is a callback function for Iterator
 */
static EjsVar *nextValue(Ejs *ejs, EjsIterator *ip, int argc, EjsVar **argv)
{
    EjsFile     *fp;

    fp = (EjsFile*) ip->target;
    if (!ejsIsFile(fp)) {
        ejsThrowReferenceError(ejs, "Wrong type");
        return 0;
    }

    if (ip->index < fp->info.size) {
#if !BLD_CC_MMU || 1
        if (mprSeek(fp->file, SEEK_CUR, 0) != ip->index) {
            if (mprSeek(fp->file, SEEK_SET, ip->index) != ip->index) {
                ejsThrowIOError(ejs, "Can't seek to %d", ip->index);
                return 0;
            }
        }
        ip->index++;
        return (EjsVar*) ejsCreateNumber(ejs, mprGetc(fp->file));
#else
        return (EjsVar*) ejsCreateNumber(ejs, fp->mapped[ip->index++]);
#endif
    }

    ejsThrowStopIteration(ejs);
    return 0;
}


/*
 *  Return an iterator to enumerate the bytes in the file. For use with "for each ..."
 *
 *  iterator native function getValues(): Iterator
 */
static EjsVar *getFileValues(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    mprGetPathInfo(ejs, fp->path, &fp->info);

    return (EjsVar*) ejsCreateIterator(ejs, (EjsVar*) fp, (EjsNativeFunction) nextValue, 0, NULL);
}


/*
 *  Get a path object for the file
 *
 *  function get path(): Path
 */
static EjsVar *getFilePath(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreatePath(ejs, fp->path);
}


/*
 *  Get the current I/O position in the file.
 *
 *  function get position(): Number
 */
static EjsVar *getFilePosition(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    if (fp->file == 0) {
        ejsThrowStateError(ejs, "File not opened");
        return 0;
    }
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) mprGetFilePosition(fp->file));
}


/*
 *  Seek to a new location in the file and set the File marker to a new read/write position.
 *
 *  function set position(value: Number): void
 */
static EjsVar *setFilePosition(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    long        pos;

    mprAssert(argc == 1 && ejsIsNumber(argv[0]));
    pos = ejsGetInt(argv[0]);

    if (fp->file == 0) {
        ejsThrowStateError(ejs, "File not opened");
        return 0;
    }
    pos = ejsGetInt(argv[0]);
    if (mprSeek(fp->file, SEEK_SET, pos) != pos) {
        ejsThrowIOError(ejs, "Can't seek to %ld", pos);
    }
    return 0;
}


/*
 *  function get isOpen(): Boolean
 */
static EjsVar *isFileOpen(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, fp->mode & FILE_OPEN);
}


/*
 *  Constructor
 *
 *  function open(options: Object = null): File
 *
 *  NOTE: options can be an options hash or as mode string
 */
static EjsVar *openFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    EjsVar  *options;
    cchar   *mode;
    int     perms, omode;

    if (argc < 0 || argc > 1) {
        ejsThrowArgError(ejs, "Bad args");
        return 0;
    }
    options = argv[0];
    if (argc == 0 || ejsIsNull(options) || ejsIsUndefined(options)) {
        omode = O_RDONLY | O_BINARY;
        perms = EJS_FILE_PERMS;
        fp->mode = FILE_READ;
        mode = "r";
    } else {
        if (ejsIsString(options)) {
            mode = ejsGetString(options);
            perms = EJS_FILE_PERMS;
        } else {
            perms = ejsGetNumOption(ejs, options, "permissions", EJS_FILE_PERMS, 1);
            mode = ejsGetStrOption(ejs, options, "mode", "r", 1);
            if (ejs->exception) {
                return 0;
            }
        }
        omode = mapMode(mode);
        if (!(omode & O_WRONLY)) {
            fp->mode |= FILE_READ;
        }
        if (omode & (O_WRONLY | O_RDWR)) {
            fp->mode |= FILE_WRITE;
        }
    }

    if (fp->file) {
        mprFree(fp->file);
    }
    fp->modeString = mprStrdup(fp, mode);
    fp->perms = perms;

    fp->file = mprOpen(fp, fp->path, omode, perms);
    if (fp->file == 0) {
        ejsThrowIOError(ejs, "Can't open %s", fp->path);
        return 0;
    }

    fp->mode |= FILE_OPEN;
    return (EjsVar*) fp;
}


static EjsVar *getFileOptions(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    EjsName     qname;
    EjsVar      *options;

    options = (EjsVar*) ejsCreateSimpleObject(ejs);
    ejsSetPropertyByName(ejs, options, ejsName(&qname, "", "mode"), (EjsVar*) ejsCreateString(ejs, fp->modeString));
    ejsSetPropertyByName(ejs, options, ejsName(&qname, "", "permissions"), (EjsVar*) ejsCreateNumber(ejs, fp->perms));
    return options;
}

/*
 *  Read data bytes from a file
 *
 *  function readBytes(count: Number = -1): ByteArray
 */
static EjsVar *readFileBytes(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    EjsByteArray    *result;
    MprPath         info;
    int             count, totalRead;

    if (argc == 0) {
        count = -1;
    } else if (argc != 1) {
        count = 0;
        ejsThrowArgError(ejs, "Bad args");
        return 0;
    } else {
        mprAssert(argc == 1 && ejsIsNumber(argv[0]));
        count = ejsGetInt(argv[0]);
    }

    if (fp->file == 0) {
        ejsThrowStateError(ejs, "File not open");
        return 0;
    }

    if (!(fp->mode & FILE_READ)) {
        ejsThrowStateError(ejs, "File not opened for reading");
        return 0;
    }
    if (count < 0) {
        if (mprGetPathInfo(fp, fp->path, &info) == 0) {
            count = (int) info.size;
            count -= (int) mprGetFilePosition(fp->file);
        } else {
            count = MPR_BUFSIZE;
        }
        mprAssert(count >= 0);
    }
    result = ejsCreateByteArray(ejs, count);
    if (result == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    totalRead = readData(ejs, fp, result, 0, count);
    if (totalRead < 0) {
        ejsThrowIOError(ejs, "Can't read from file: %s", fp->path);
        return 0;
    } else if (totalRead == 0) {
        return ejs->nullValue;
    }
    ejsSetByteArrayPositions(ejs, result, 0, totalRead);

    return (EjsVar*) result;
}


/*
 *  Read data as a string
 *
 *  function readString(count: Number = -1): String
 */
static EjsVar *readFileString(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    EjsString       *result;
    MprPath         info;
    int             count, totalRead;

    if (argc == 0) {
        count = -1;
    } else if (argc != 1) {
        count = 0;
        ejsThrowArgError(ejs, "Bad args");
        return 0;
    } else {
        mprAssert(argc == 1 && ejsIsNumber(argv[0]));
        count = ejsGetInt(argv[0]);
    }

    if (fp->file == 0) {
        ejsThrowStateError(ejs, "File not open");
        return 0;
    }

    if (!(fp->mode & FILE_READ)) {
        ejsThrowStateError(ejs, "File not opened for reading");
        return 0;
    }

    if (count < 0) {
        if (mprGetPathInfo(fp, fp->path, &info) == 0) {
            count = (int) info.size;
            count -= (int) mprGetFilePosition(fp->file);
        } else {
            count = MPR_BUFSIZE;
        }
        mprAssert(count >= 0);
    }
    result = ejsCreateBareString(ejs, count);
    if (result == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }

    totalRead = mprRead(fp->file, result->value, count);
    if (totalRead != count) {
        ejsThrowIOError(ejs, "Can't read from file: %s", fp->path);
        return 0;
    }
    return (EjsVar*) result;
}


/*
 *  Read data bytes from a file. If offset is < 0, then append to the write position.
 *
 *  function read(buffer: ByteArray, offset: Number = 0, count: Number = -1): Number
 */
static EjsVar *readFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    EjsByteArray    *buffer;
    MprPath         info;
    int             count, offset, totalRead;

    mprAssert(1 <= argc && argc <= 3);

    buffer = (EjsByteArray*) argv[0];
    offset = (argc >= 2) ? ejsGetInt(argv[1]): 0;
    count = (argc >= 3) ? ejsGetInt(argv[2]): -1;

    if (offset >= buffer->length) {
        ejsThrowOutOfBoundsError(ejs, "Bad read offset value");
        return 0;
    }
    if (offset < 0) {
        offset = buffer->writePosition;
    } else if (offset == 0) {
        ejsSetByteArrayPositions(ejs, buffer, 0, 0);
    }
    if (count < 0) {
        if (mprGetPathInfo(fp, fp->path, &info) == 0) {
            count = (int) info.size;
            count -= (int) mprGetFilePosition(fp->file);
        } else {
            count = MPR_BUFSIZE;
        }
        mprAssert(count >= 0);
    }
    if (fp->file == 0) {
        ejsThrowStateError(ejs, "File not open");
        return 0;
    }
    if (!(fp->mode & FILE_READ)) {
        ejsThrowStateError(ejs, "File not opened for reading");
        return 0;
    }
    totalRead = readData(ejs, fp, buffer, offset, count);
    if (totalRead < 0) {
        return 0;
    } else if (totalRead == 0) {
        return ejs->nullValue;
    }
    ejsSetByteArrayPositions(ejs, buffer, -1, offset + totalRead);
    return (EjsVar*) ejsCreateNumber(ejs, totalRead);
}


/*
 *  Get the size of the file associated with this File object.
 *
 *  override intrinsic function get size(): Number
 */
static EjsVar *getFileSize(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    MprPath     info;

    if (mprGetPathInfo(ejs, fp->path, &info) < 0) {
        return (EjsVar*) ejs->minusOneValue;
    }
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) info.size);
}


/*
 *  function truncate(size: Number): Void
 */
EjsVar *truncateFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    int     size;

    size = ejsGetInt(argv[0]);
    if (mprTruncatePath(ejs, fp->path, size) < 0) {
        ejsThrowIOError(ejs, "Cant truncate %s", fp->path);
    }
    return 0;
}


/*
 *  Write data to the file
 *
 *  function write(data: Object): Number
 */
EjsVar *writeFile(Ejs *ejs, EjsFile *fp, int argc, EjsVar **argv)
{
    EjsArray        *args;
    EjsByteArray    *ap;
    EjsVar          *vp;
    EjsString       *str;
    char            *buf;
    int             i, len, written;

    mprAssert(argc == 1 && ejsIsArray(argv[0]));

    args = (EjsArray*) argv[0];

    if (!(fp->mode & FILE_WRITE)) {
        ejsThrowStateError(ejs, "File not opened for writing");
        return 0;
    }

    written = 0;

    for (i = 0; i < args->length; i++) {
        vp = ejsGetProperty(ejs, (EjsVar*) args, i);
        mprAssert(vp);
        switch (vp->type->id) {
        case ES_ByteArray:
            ap = (EjsByteArray*) vp;
            buf = (char*) &ap->value[ap->readPosition];
            len = ap->writePosition - ap->readPosition;
            break;

        case ES_String:
            buf = ((EjsString*) vp)->value;
            len = ((EjsString*) vp)->length;
            break;

        default:
            str = ejsToString(ejs, vp);
            buf = ejsGetString(str);
            len = str->length;
            break;
        }
        if (mprWrite(fp->file, buf, len) != len) {
            ejsThrowIOError(ejs, "Can't write to %s", fp->path);
            return 0;
        }
        written += len;
    }
    return (EjsVar*) ejsCreateNumber(ejs, written);
}


/*********************************** Support **********************************/

static int readData(Ejs *ejs, EjsFile *fp, EjsByteArray *ap, int offset, int count)
{
    int     len, bytes;

    if (count <= 0) {
        return 0;
    }
    len = ap->length - offset;
    len = min(count, len);
    bytes = mprRead(fp->file, &ap->value[offset], len);
    if (bytes < 0) {
        ejsThrowIOError(ejs, "Error reading from %s", fp->path);
    }
    return bytes;
}


static int mapMode(cchar *mode)
{
    int     omode;

    omode = O_BINARY;
    if (strchr(mode, 'r')) {
        omode |= O_RDONLY;
    }
    if (strchr(mode, 'w')) {
        if (omode & O_RDONLY) {
            omode &= ~O_RDONLY;
            omode |= O_RDWR;
        } else {
            omode |= O_CREAT | O_WRONLY | O_TRUNC;
        }
    }
    if (strchr(mode, 'a')) {
        omode |= O_WRONLY | O_APPEND;
    }
    if (strchr(mode, '+')) {
        omode &= ~O_TRUNC;
    }
    if (strchr(mode, 't')) {
        omode &= ~O_BINARY;
    }
    return omode;
}


/*********************************** Factory **********************************/

EjsFile *ejsCreateFile(Ejs *ejs, cchar *path)
{
    EjsFile     *fp;
    EjsVar      *arg;

    mprAssert(path && *path);

    fp = (EjsFile*) ejsCreateVar(ejs, ejsGetType(ejs, ES_ejs_io_File), 0);
    if (fp == 0) {
        return 0;
    }
    arg = (EjsVar*) ejsCreateString(ejs, path);
    fileConstructor(ejs, fp, 1, (EjsVar**) &arg);
    return fp;
}


EjsFile *ejsCreateFileFromFd(Ejs *ejs, int fd, cchar *name, int mode)
{
    EjsFile     *fp;

    mprAssert(fd >= 0);
    mprAssert(name);

    fp = (EjsFile*) ejsCreateVar(ejs, ejsGetType(ejs, ES_ejs_io_File), 0);
    if (fp == 0) {
        return 0;
    }

    fp->perms = EJS_FILE_PERMS;
    fp->mode = FILE_OPEN;
    if (!(mode & O_WRONLY)) {
        fp->mode |= FILE_READ;
    }
    if (mode & (O_WRONLY | O_RDWR)) {
        fp->mode |= FILE_WRITE;
    }
    fp->file = mprAttachFd(fp, fd, name, mode);
    if (fp->file == 0) {
        return 0;
    }
    return fp;
}


void ejsCreateFileType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = ejsCreateCoreType(ejs, ejsName(&qname, "ejs.io", "File"), ejs->objectType, sizeof(EjsFile), ES_ejs_io_File,
        ES_ejs_io_File_NUM_CLASS_PROP, ES_ejs_io_File_NUM_INSTANCE_PROP, 
        EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_HAS_CONSTRUCTOR | EJS_ATTR_OBJECT_HELPERS);
    if (type == 0) {
        return;
    }

    /*
     *  Define the helper functions.
     */
    type->helpers->destroyVar = (EjsDestroyVarHelper) destroyFile;
    type->helpers->getProperty = (EjsGetPropertyHelper) getFileProperty;
    type->helpers->setProperty = (EjsSetPropertyHelper) setFileProperty;
    type->numericIndicies = 1;
}


void ejsConfigureFileType(Ejs *ejs)
{
    EjsType     *type;

    if ((type = ejsGetType(ejs, ES_ejs_io_File)) == 0) {
        return;
    }
    mprAssert(type);

    ejsBindMethod(ejs, type, ES_ejs_io_File_File, (EjsNativeFunction) fileConstructor);
    ejsBindMethod(ejs, type, ES_ejs_io_File_canRead, (EjsNativeFunction) canReadFile);
    ejsBindMethod(ejs, type, ES_ejs_io_File_canWrite, (EjsNativeFunction) canWriteFile);
    ejsBindMethod(ejs, type, ES_ejs_io_File_close, (EjsNativeFunction) closeFile);
    ejsBindMethod(ejs, type, ES_ejs_io_File_flush, (EjsNativeFunction) flushFile);
    ejsBindMethod(ejs, type, ES_Object_get, (EjsNativeFunction) getFileIterator);
    ejsBindMethod(ejs, type, ES_Object_getValues, (EjsNativeFunction) getFileValues);
    ejsBindMethod(ejs, type, ES_ejs_io_File_isOpen, (EjsNativeFunction) isFileOpen);
    ejsBindMethod(ejs, type, ES_ejs_io_File_open, (EjsNativeFunction) openFile);
    ejsBindMethod(ejs, type, ES_ejs_io_File_options, (EjsNativeFunction) getFileOptions);
    ejsBindMethod(ejs, type, ES_ejs_io_File_path, (EjsNativeFunction) getFilePath);
    ejsBindMethod(ejs, type, ES_ejs_io_File_position, (EjsNativeFunction) getFilePosition);
    ejsBindMethod(ejs, type, ES_ejs_io_File_set_position, (EjsNativeFunction) setFilePosition);
    ejsBindMethod(ejs, type, ES_ejs_io_File_readBytes, (EjsNativeFunction) readFileBytes);
    ejsBindMethod(ejs, type, ES_ejs_io_File_readString, (EjsNativeFunction) readFileString);
    ejsBindMethod(ejs, type, ES_ejs_io_File_read, (EjsNativeFunction) readFile);
    ejsBindMethod(ejs, type, ES_ejs_io_File_size, (EjsNativeFunction) getFileSize);
    ejsBindMethod(ejs, type, ES_ejs_io_File_truncate, (EjsNativeFunction) truncateFile);
    ejsBindMethod(ejs, type, ES_ejs_io_File_write, (EjsNativeFunction) writeFile);
}

#if 0 && !BLD_FEATURE_ROMFS
void ejsCreateFileType(Ejs *ejs) {}
void ejsConfigureFileType(Ejs *ejs) {}
#endif

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
