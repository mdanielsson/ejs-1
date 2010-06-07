/**
 *  ejsHttp.c - Http class. This implements a HTTP client.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if ES_ejs_io_Http && BLD_FEATURE_HTTP_CLIENT
/**************************** Forward Declarations ****************************/

static EjsVar   *getDateHeader(Ejs *ejs, EjsHttp *hp, cchar *key);
static EjsVar   *getStringHeader(Ejs *ejs, EjsHttp *hp, cchar *key);
static void     httpCallback(EjsHttp *hp, int mask);
static void     prepForm(Ejs *ejs, EjsHttp *hp, char *prefix, EjsVar *data);
static char     *prepUri(MprCtx ctx, cchar *uri);
static EjsVar   *startRequest(Ejs *ejs, EjsHttp *hp, char *method, int argc, EjsVar **argv);
static bool     waitForResponse(EjsHttp *hp, int timeout);
static bool     waitForState(EjsHttp *hp, int state, int timeout, int throw);

/************************************ Methods *********************************/
/*
 *  Constructor
 *
 *  function Http(uri: String = null)
 */
static EjsVar *httpConstructor(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    hp->ejs = ejs;
    hp->http = mprCreateHttp(hp);
    if (hp->http == 0) {
        ejsThrowMemoryError(ejs);
    }
    mprAssert(hp->http->sock == 0);
    mprAssert(hp->http->state == MPR_HTTP_STATE_BEGIN);
    mprAssert(hp->http->request->chunked == -1);

    if (argc == 1 && argv[0] != ejs->nullValue) {
        hp->uri = prepUri(hp, ejsGetString(argv[0]));
    }
    hp->method = mprStrdup(hp, "GET");
    hp->responseContent = mprCreateBuf(hp, MPR_HTTP_BUFSIZE, -1);
#if BLD_FEATURE_MULTITHREAD
    hp->mutex = mprCreateLock(hp);
#endif
    return (EjsVar*) hp;
}


/*
 *  function addHeader(key: String, value: String, overwrite: Boolean = true): Void
 */
EjsVar *addHeader(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    char    *key, *value;
    bool    overwrite;

    mprAssert(argc >= 2);

    key = ejsGetString(argv[0]);
    value = ejsGetString(argv[1]);
    overwrite = (argc == 3) ? ejsGetBoolean(argv[2]) : 1;
    mprSetHttpHeader(hp->http, overwrite, key, value);
    return 0;
}


/*
 *  function get available(): Number
 */
EjsVar *httpAvailable(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    int     len;

    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    len = mprGetHttpContentLength(hp->http);
    if (len > 0) {
        return (EjsVar*) ejsCreateNumber(ejs, len);
    }
    return (EjsVar*) ejs->minusOneValue;
}


/*
 *  function setCallback(mask: Number cb: Function): Void
 */
EjsVar *setHttpCallback(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    int     mask;

    mprAssert(argc == 1 || argc == 2);

    mask = ejsGetInt(argv[0]);
    if (mask & ~(MPR_READABLE | MPR_WRITABLE)) {
        ejsThrowStateError(ejs, "Bad callback event mask:");
        return 0;
    }
    hp->callback = (EjsFunction*) argv[1];
    mprSetHttpCallback(hp->http, (MprHttpProc) httpCallback, hp, mask);
    return 0;
}


/*
 *  function close(graceful: Boolean = true): Void
 */
static EjsVar *closeHttp(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (hp->http) {
        mprFree(hp->http);
        hp->http = mprCreateHttp(hp);
    }
    return 0;
}


/*
 *  function connect(url = null, data ...): Void
 */
static EjsVar *connectHttp(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, NULL, argc, argv);
}


/**
 *  function get certificate(): String
 */
static EjsVar *getCertificate(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (hp->certFile) {
        return (EjsVar*) ejsCreateString(ejs, hp->certFile);
    }
    return ejs->nullValue;
}


/*
 *  function set setCertificate(value: String): Void
 */
static EjsVar *setCertificate(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprFree(hp->certFile);
    hp->certFile = mprStrdup(hp, ejsGetString(argv[0]));
    return 0;
}


#if ES_ejs_io_Http_chunked
/**
 *  function get chunked(): Boolean
 */
static EjsVar *getChunked(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    int     chunked;

    chunked = mprGetHttpChunked(hp->http);
    if (chunked == -1) {
        return ejs->undefinedValue;
    } else if (chunked == 0) {
        return (EjsVar*) ejs->falseValue;
    } else {
        return (EjsVar*) ejs->trueValue;
    }
}
#endif


#if ES_ejs_io_Http_set_chunked
/**
 *  function set chunked(value: Boolean): Void
 */
static EjsVar *setChunked(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprAssert(hp->http);
    mprAssert(hp->http->state == MPR_HTTP_STATE_BEGIN || hp->http->state == MPR_HTTP_STATE_COMPLETE);
    if (!(hp->http->state == MPR_HTTP_STATE_BEGIN || hp->http->state == MPR_HTTP_STATE_COMPLETE)) {
        printf("STATE IS %d\n", hp->http->state);
    }

    if (mprSetHttpChunked(hp->http, ejsGetBoolean(argv[0])) < 0) {
        ejsThrowStateError(ejs, "Can't change chunked setting in this state. Request has already started.");
    }
    return 0;
}
#endif


/*
 *  function get code(): Number
 */
static EjsVar *code(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    return (EjsVar*) ejsCreateNumber(ejs, mprGetHttpCode(hp->http));
}


/*
 *  function get codeString(): String
 */
static EjsVar *codeString(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    return (EjsVar*) ejsCreateString(ejs, mprGetHttpMessage(hp->http));
}


/*
 *  function get contentEncoding(): String
 */
static EjsVar *contentEncoding(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return getStringHeader(ejs, hp, "CONTENT-ENCODING");
}


/*
 *  function get contentLength(): Number
 */
static EjsVar *contentLength(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    int     length;

    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    length = mprGetHttpContentLength(hp->http);
    return (EjsVar*) ejsCreateNumber(ejs, length);
}


#if ES_ejs_io_Http_set_contentLength
/*
 *  function set contentLength(length: Number): Void
 */
static EjsVar *setContentLength(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    hp->contentLength = (int) ejsGetNumber(argv[0]);
    return 0;
}
#endif


/*
 *  function get contentType(): String
 */
static EjsVar *contentType(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    return getStringHeader(ejs, hp, "CONTENT-TYPE");
}


/**
 *  function get date(): Date
 */
static EjsVar *date(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return getDateHeader(ejs, hp, "DATE");
}


/*
 *  function del(uri: String = null): Void
 */
static EjsVar *del(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "DELETE", argc, argv);
}


/**
 *  function get expires(): Date
 */
static EjsVar *expires(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return getDateHeader(ejs, hp, "EXPIRES");
}


/**
 *  function get flush(graceful: Boolean = true): Void
 */
static EjsVar *flushHttp(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprFinalizeHttpWriting(hp->http);
    return 0;
}


/*
 *  function form(uri: String = null, formData: Object = null): Void
 *
 *  Issue a POST method with form data
 */
static EjsVar *form(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (argc == 2 && argv[1] != ejs->nullValue) {
        hp->requestContent = NULL;
        hp->contentLength = 0;
        prepForm(ejs, hp, NULL, argv[1]);
        if (hp->requestContent) {
            hp->contentLength = (int) strlen(hp->requestContent);
        }
        mprSetHttpHeader(hp->http, 1, "Content-Type", "application/x-www-form-urlencoded");
    }
    return startRequest(ejs, hp, "POST", argc, argv);
}


/*
 *
 *  static function get followRedirects(): Boolean
 */
static EjsVar *getFollowRedirects(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, hp->http->followRedirects);
}


/*
 *  function set followRedirects(flag: Boolean): Void
 */
static EjsVar *setFollowRedirects(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprSetHttpFollowRedirects(hp->http, ejsGetBoolean(argv[0]));
    return 0;
}


/*
 *  function get(uri: String = null, ...data): Void
 *
 *  The spec allows GET methods to have body data, but is rarely if ever used.
 */
static EjsVar *getMethod(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "GET", argc, argv);
}


/*
 *  function head(uri: String = null): Void
 */
static EjsVar *headMethod(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "HEAD", argc, argv);
}


/*
 *  function header(key: String): String
 */
static EjsVar *header(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    return (EjsVar*) ejsCreateString(ejs, mprGetHttpHeader(hp->http, ejsGetString(argv[0])));
}


/*
 *  function get headers(): Array
 */
static EjsVar *headers(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    MprHashTable    *hash;
    MprHash         *p;
    EjsVar          *results;
    char            *header;
    int             i;

    hash = mprGetHttpHeadersHash(hp->http);
    if (hash == 0) {
        return (EjsVar*) ejs->nullValue;
    }
    results = (EjsVar*) ejsCreateArray(ejs, mprGetHashCount(hash));
    for (i = 0, p = mprGetFirstHash(hash); p; p = mprGetNextHash(hash, p), i++) {
        header = mprAsprintf(results, -1, "%s=%s", p->key, p->data);
        ejsSetProperty(ejs, (EjsVar*) results, i, (EjsVar*) ejsCreateStringAndFree(ejs, header));
    }
    return (EjsVar*) results;
}


/*
 *  function get isSecure(): Boolean
 */
static EjsVar *isSecure(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateBoolean(ejs, hp->http->secure);
}


/*
 *  function get key(): String
 */
static EjsVar *getKey(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (hp->keyFile) {
        return (EjsVar*) ejsCreateString(ejs, hp->keyFile);
    }
    return ejs->nullValue;
}


/*
 *  function set key(keyFile: String): Void
 */
static EjsVar *setKey(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprFree(hp->keyFile);
    hp->keyFile = mprStrdup(hp, ejsGetString(argv[0]));
    return 0;
}


/*
 *  function get lastModified(): Date
 */
static EjsVar *lastModified(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return getDateHeader(ejs, hp, "LAST-MODIFIED");
}


/*
 *  function get method(): String
 */
static EjsVar *getMethodValue(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, hp->method);
}


/*
 *  function set method(value: String): Void
 */
static EjsVar *setMethodValue(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    cchar    *method;

    method = ejsGetString(argv[0]);
    if (strcmp(method, "DELETE") != 0 && strcmp(method, "GET") != 0 &&  strcmp(method, "HEAD") != 0 &&
            strcmp(method, "OPTIONS") != 0 && strcmp(method, "POST") != 0 && strcmp(method, "PUT") != 0 &&
            strcmp(method, "TRACE") != 0) {
        ejsThrowArgError(ejs, "Unknown HTTP method");
        return 0;
    }
    mprFree(hp->method);
    hp->method = mprStrdup(hp, ejsGetString(argv[0]));
    return 0;
}


#if ES_ejs_io_Http_mimeType
/*
 *  static function getMimeType(ext: String): String
 */
EjsVar *getMimeType(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, mprLookupMimeType(ejs, ejsGetString(argv[0])));
}
#endif

/*
 *  function options(uri: String = null, ...data): Void
 */
static EjsVar *optionsMethod(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "OPTIONS", argc, argv);
}


/*
 *  function post(uri: String = null, ...requestContent): Void
 */
static EjsVar *postMethod(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "POST", argc, argv);
}


/*
 *  function put(uri: String = null, form object): Void
 */
static EjsVar *putMethod(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "PUT", argc, argv);
}


/*
 *  Ensure the required number of bytes are read into the content buffer. If allowShortRead is true or if a callback is
 *  being used, then this routine will not block. When count is < 0 this means transfer the entire content.
 *  Returns the number of bytes read. If readTransfer returns zero, call mprIsHttpComplete to determine if the request
 *  has completed.
 */ 
static int readTransfer(Ejs *ejs, EjsHttp *hp, int count, int allowShortRead)
{
    MprBuf      *buf;
    int         nbytes, space, len;

    buf = hp->responseContent;
    while (count < 0 || mprGetBufLength(buf) < count) {
        if (count < 0) {
            nbytes = MPR_HTTP_BUFSIZE;
        } else {
            nbytes = count - mprGetBufLength(buf);
        }
        space = mprGetBufSpace(buf);
        if (space < nbytes) {
            mprGrowBuf(buf, nbytes - space);
        }
        if ((len = mprReadHttp(hp->http, mprGetBufEnd(buf), nbytes)) < 0) {
            if (hp->callback || allowShortRead) {
                break;
            }
            ejsThrowIOError(ejs, "Can't read required data");
            return MPR_ERR_CANT_READ;
        }
        mprAdjustBufEnd(buf, len);
        if (/* CHANGE count < 0 || */ (len == 0 && (mprIsHttpComplete(hp->http) || hp->callback))) {
            break;
        }
    }
    if (count < 0) {
        return mprGetBufLength(buf);
    }
    len = min(count, mprGetBufLength(buf));
    hp->received += len;
    return len;
}


/*
 *  function read(buffer: ByteArray, offset: Number = 0, count: Number = -1): Number
 *
 *  Returns a count of bytes read. Non-blocking if a callback is defined. Otherwise, blocks.
 */
static EjsVar *readHttpData(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    EjsByteArray    *buffer;
    int             offset, count;

    buffer = (EjsByteArray*) argv[0];
    offset = (argc >= 2) ? ejsGetInt(argv[1]) : 0;
    count = (argc >= 3) ? ejsGetInt(argv[2]): -1;

    if (offset < 0) {
        offset = buffer->writePosition;
    } else if (offset >= buffer->length) {
        ejsThrowOutOfBoundsError(ejs, "Bad read offset value");
        return 0;
    } else {
        ejsSetByteArrayPositions(ejs, buffer, 0, 0);
    }
    if (!waitForResponse(hp, -1)) {
        mprAssert(ejs->exception);
        return 0;
    }
    lock(hp);
    if ((count = readTransfer(ejs, hp, count, 0)) < 0) {
        mprAssert(ejs->exception);
        unlock(hp);
        return 0;
    }
    ejsCopyToByteArray(ejs, buffer, buffer->writePosition, (char*) mprGetBufStart(hp->responseContent), count);
    ejsSetByteArrayPositions(ejs, buffer, -1, buffer->writePosition + count);
    mprAdjustBufStart(hp->responseContent, count);
    unlock(hp);
    return (EjsVar*) ejsCreateNumber(ejs, count);
}


/*
 *  function readString(count: Number = -1): String
 *
 *  Read count bytes (default all) of content as a string. This always starts at the first character of content.
 */
static EjsVar *readStringHttp(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    EjsVar  *result;
    int     count;
    
    count = (argc == 1) ? ejsGetInt(argv[0]) : -1;

    if (!waitForState(hp, MPR_HTTP_STATE_COMPLETE, hp->http->timeoutPeriod, 0)) {
        return 0;
    }
    lock(hp);
    if ((count = readTransfer(ejs, hp, count, 0)) < 0) {
        return 0;
    }
    result = (EjsVar*) ejsCreateStringWithLength(ejs, mprGetBufStart(hp->responseContent), count);
    mprAdjustBufStart(hp->responseContent, count);
    unlock(hp);
    return result;
}


/*
 *  function readLines(count: Number = -1): Array
 *
 *  Read count lines (default all) of content as an array of strings.
 */
static EjsVar *readLines(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    EjsArray    *results;
    EjsVar      *str;
    MprBuf      *buf;
    cchar       *start, *cp;
    int         lineCount, count, nextIndex, len, lineCreated;

    lineCount = (argc == 1) ? ejsGetInt(argv[0]) : MAXINT;
    results = ejsCreateArray(ejs, 0);
    count = 0;

    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    lock(hp);
    buf = hp->responseContent;
    for (nextIndex = 0; nextIndex < lineCount; nextIndex++) {
        lineCreated = 0;
        for (cp = start = mprGetBufStart(buf); cp < buf->end; cp++) {
            if (*cp == '\r' || *cp == '\n') {
                len = cp - start;
                str = (EjsVar*) ejsCreateStringWithLength(ejs, start, len);
                mprAdjustBufStart(buf, len);
                ejsSetProperty(ejs, (EjsVar*) results, nextIndex, str);
                for (start = cp; *cp == '\r' || *cp == '\n'; ) {
                    cp++;
                }
                mprAdjustBufStart(buf, cp - start);
                lineCreated++;
                break;
            }
        }
        if (!lineCreated) {
            if ((count = readTransfer(ejs, hp, MPR_BUFSIZE, 1)) < 0) {
                return 0;
            } else if (count == 0) {
                if (!mprIsHttpComplete(hp->http)) {
                    if (!hp->callback) {
                        ejsThrowIOError(ejs, "Can't read required data");
                        return 0;
                    }
                }
                break;
            }
        }
    }
    if (mprGetBufLength(buf) > 0 && (nextIndex == 0 || count < 0)) {
        /*
         *  Partial line
         */
        str = (EjsVar*) ejsCreateStringWithLength(ejs, mprGetBufStart(buf), mprGetBufLength(buf));
        ejsSetProperty(ejs, (EjsVar*) results, nextIndex++, str);
        mprFlushBuf(buf);
    }
    unlock(hp);
    return (EjsVar*) results;
}


#if BLD_FEATURE_EJS_E4X && UNUSED
/*
 *  function readXml(): XML
 */
static EjsVar *readXml(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    EjsXML  *xml;
    int     count;

    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    xml = ejsCreateXML(ejs, 0, NULL, NULL, NULL);
    lock(hp);
    if ((count = readTransfer(ejs, hp, -1, 0)) < 0) {
        return 0;
    }
    mprAddNullToBuf(hp->responseContent);
    ejsLoadXMLString(ejs, xml, mprGetBufStart(hp->responseContent));
    mprFlushBuf(hp->responseContent);
    unlock(hp);
    return (EjsVar*) xml;
}
#endif


/*
 *  function response(): Stream
 */
static EjsVar *httpResponse(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    if (hp->responseCache) {
        return hp->responseCache;
    }
    hp->responseCache = readStringHttp(ejs, hp, argc, argv);
    return (EjsVar*) hp->responseCache;
}


/*
 *  function setCredentials(username: String, password: String): Void
 */
static EjsVar *setCredentials(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprSetHttpCredentials(hp->http, ejsGetString(argv[0]), ejsGetString(argv[1]));
    return 0;
}


/*
 *  function get timeout(): Number
 */
static EjsVar *getTimeout(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateNumber(ejs, hp->http->timeoutPeriod);
}


/*
 *  function set timeout(value: Number): Void
 */
static EjsVar *setTimeout(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprSetHttpTimeout(hp->http, (int) ejsGetNumber(argv[0]));
    return 0;
}


/*
 *  function trace(uri: String = null, ...data): Void
 */
static EjsVar *traceMethod(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return startRequest(ejs, hp, "TRACE", argc, argv);
}


/*
 *  function get uri(): String
 */
static EjsVar *getUri(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, hp->uri);
}


/*
 *  function set uri(uri: String): Void
 */
static EjsVar *setUri(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    mprFree(hp->uri);
    hp->uri = prepUri(hp, ejsGetString(argv[0]));
    return 0;
}


#if ES_ejs_io_Http_wait
/**
 *  Wait for a request to complete. This call will only have effect if an async request is underway.
 *
 *  function wait(timeout: Number = -1): Boolean
 */
static EjsVar *httpWait(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    MprTime     mark;
    int         timeout;

    timeout = (argc == 1) ? ejsGetInt(argv[0]) : MPR_TIMEOUT_HTTP;
    if (timeout < 0) {
        timeout = MAXINT;
    }
    mark = mprGetTime(ejs);

    if (!hp->requestStarted && startRequest(ejs, hp, NULL, 0, NULL) < 0) {
        return 0;
    }
    if (!waitForState(hp, MPR_HTTP_STATE_COMPLETE, timeout, 0)) {
        return (EjsVar*) ejs->falseValue;
    }
    return (EjsVar*) ejs->trueValue;
}
#endif


/*
 *  Write post data to the request stream. Connection must be in async mode.
 *
 *  function write(...data): Void
 */
static EjsVar *httpWrite(Ejs *ejs, EjsHttp *hp, int argc, EjsVar **argv)
{
    EjsByteArray    *data;
    EjsNumber       *written;

    mprAssert(hp->http->request);
    mprAssert(hp->http->sock);

    if (!hp->requestStarted && startRequest(hp->ejs, hp, NULL, 0, NULL) < 0) {
        return 0;
    }
    data = ejsCreateByteArray(ejs, -1);
    written = ejsWriteToByteArray(ejs, data, 1, &argv[0]);

    if (mprWriteHttp(hp->http, (char*) data->value, (int) written->value) != (int) written->value) {
        ejsThrowIOError(ejs, "Can't write post data");
    }
    return 0;
}


/*********************************** Support **********************************/
/*
 *  Issue a request
 *      startRequest(uri = null, ...data)
 */
static EjsVar *startRequest(Ejs *ejs, EjsHttp *hp, char *method, int argc, EjsVar **argv)
{
    EjsArray        *args;
    EjsByteArray    *data;
    EjsNumber       *written;

    hp->responseCache = 0;

    if (argc >= 1 && argv[0] != ejs->nullValue) {
        mprFree(hp->uri);
        hp->uri = prepUri(hp, ejsGetString(argv[0]));
    }
    if (argc == 2 && ejsIsArray(argv[1])) {
        args = (EjsArray*) argv[1];
        if (args->length > 0) {
            data = ejsCreateByteArray(ejs, -1);
            written = ejsWriteToByteArray(ejs, data, 1, &argv[1]);
            hp->requestContent = (char*) data->value;
            hp->contentLength = (int) written->value;
        }
    }

    if (hp->uri == 0) {
        ejsThrowArgError(ejs, "URL is not defined");
        return 0;
    }

#if BLD_FEATURE_SSL
    if (strncmp(hp->uri, "https", 5) == 0) {
        if (!mprLoadSsl(ejs, 0)) {
            ejsThrowIOError(ejs, "Can't load SSL provider");
            return 0;
        }
    }
#endif
    if (method && strcmp(hp->method, method) != 0) {
        mprFree(hp->method);
        hp->method = mprStrdup(hp, method);
    }
    if (hp->method == 0) {
        ejsThrowArgError(ejs, "HTTP Method is not defined");
        return 0;
    }
    mprFlushBuf(hp->responseContent);
    if (hp->contentLength > 0) {
        mprSetHttpBody(hp->http, hp->requestContent, hp->contentLength);
    }
    hp->requestStarted = 1;
    hp->gotResponse = 0;
    hp->requestContent = 0;
    hp->contentLength = 0;

    /*
     *  Block if a callback has not been defined
     */
    if (mprStartHttpRequest(hp->http, hp->method, hp->uri) < 0) {
        ejsThrowIOError(ejs, "Can't issue request for \"%s\"", hp->uri);
        return 0;
    }
    return 0;
}


static bool waitForState(EjsHttp *hp, int state, int timeout, int throw)
{
    Ejs         *ejs;
    MprHttp     *http;
    MprTime     mark;
    char        *url;
    int         count, transCount;

    ejs = hp->ejs;
    if (timeout < 0) {
        timeout = MAXINT;
    }

    http = hp->http;
    count = -1;
    transCount = 0;
    mark = mprGetTime(hp);

    if (state >= MPR_HTTP_STATE_CONTENT) {
        if (mprFinalizeHttpWriting(http) < 0) {
            if (throw) {
                ejsThrowIOError(ejs, "Can't write request data");
            }
            return 0;
        }
    }
    while (++count < http->retries && transCount < 4 && !ejs->exiting && !mprIsExiting(http) && 
            mprGetElapsedTime(hp, mark) < timeout) {
        if (!hp->requestStarted && startRequest(ejs, hp, NULL, 0, NULL) < 0) {
            return 0;
        }
        if (mprWaitForHttp(http, state, timeout) < 0) {
            continue;
        }
        if (http->state >= MPR_HTTP_STATE_CONTENT && mprNeedHttpRetry(http, &url)) {
            if (url) {
                mprFree(hp->uri);
                hp->uri = prepUri(http, url);
            }
            count--;
            transCount++;
            hp->requestStarted = 0;
            continue;
        }
        break;
    }
    if (http->state < state) {
        if (throw && ejs->exception == 0) {
            ejsThrowIOError(ejs, "Http has not received a response: timeout %d, retryCount %d/%d, trans %d/4",
                timeout, count, http->retries, transCount);
        }
        return 0;
    }
    return 1;
}


static bool waitForResponse(EjsHttp *hp, int timeout)
{
    if (hp->gotResponse) {
        return 1;
    }
    if (!waitForState(hp, MPR_HTTP_STATE_CONTENT, timeout, 1)) {
        return 0;
    }
    hp->gotResponse = 1;
    return 1;
}


static void httpCallback(EjsHttp *hp, int eventMask)
{
    MprHttp     *http;
    Ejs         *ejs;
    EjsVar      *arg;
    EjsType     *eventType;
    EjsObject   *event;

    ejs = hp->ejs;
    http = hp->http;
    if (http->error) {
        eventType = ejsGetType(ejs, ES_ejs_io_HttpErrorEvent);
        arg = (EjsVar*) ejsCreateString(ejs, http->error);
    } else {
        eventType = ejsGetType(ejs, ES_ejs_io_HttpDataEvent);
        arg = (EjsVar*) ejs->nullValue;
    }
    event = (EjsObject*) ejsCreateInstance(ejs, eventType, 1, (EjsVar**) &arg);
    if (event) {
        /*
         *  Invoked as:  callback(e: Event)  where e.data == http
         */
        ejsSetProperty(ejs, (EjsVar*) event, ES_ejs_events_Event_data, (EjsVar*) hp);
        if (!http->error) {
            ejsSetProperty(ejs, (EjsVar*) event, ES_ejs_io_HttpDataEvent_eventMask, 
                (EjsVar*) ejsCreateNumber(ejs, eventMask));
        }
        arg = (EjsVar*) event;
        ejsRunFunction(hp->ejs, hp->callback, 0, 1, &arg);
    }
}


static EjsVar *getDateHeader(Ejs *ejs, EjsHttp *hp, cchar *key)
{
    MprTime     when;
    cchar       *value;

    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    value = mprGetHttpHeader(hp->http, key);
    if (value == 0) {
        return (EjsVar*) ejs->nullValue;
    }
    if (mprParseTime(ejs, &when, value, MPR_UTC_TIMEZONE, NULL) < 0) {
        value = 0;
    }
    return (EjsVar*) ejsCreateDate(ejs, when);
}


static EjsVar *getStringHeader(Ejs *ejs, EjsHttp *hp, cchar *key)
{
    cchar       *value;

    if (!waitForResponse(hp, -1)) {
        return 0;
    }
    value = mprGetHttpHeader(hp->http, key);
    if (value == 0) {
        return (EjsVar*) ejs->nullValue;
    }
    return (EjsVar*) ejsCreateString(ejs, mprGetHttpHeader(hp->http, key));
}


/*
 *  Prepare form data as a series of key-value pairs. Data is an object with properties becoming keys in a 
 *  www-url-encoded string. Objects are flattened into a one level key/value pairs by using JSON encoding. 
 *  E.g.  name=value&address=77%20Park%20Lane
 */
static void prepForm(Ejs *ejs, EjsHttp *hp, char *prefix, EjsVar *data)
{
    EjsName     qname;
    EjsVar      *vp;
    EjsString   *value;
    cchar       *key, *sep;
    char        *encodedKey, *encodedValue, *newPrefix, *newKey;
    int         i, count;

    count = ejsGetPropertyCount(ejs, data);
    for (i = 0; i < count; i++) {
        qname = ejsGetPropertyName(ejs, data, i);
        key = qname.name;

        vp = ejsGetProperty(ejs, data, i);
        if (vp == 0) {
            continue;
        }
        if (ejsGetPropertyCount(ejs, vp) > 0 && !ejsIsArray(vp)) {
            if (prefix) {
                newPrefix = mprAsprintf(hp, -1, "%s.%s", prefix, qname.name);
                prepForm(ejs, hp, newPrefix, vp);
                mprFree(newPrefix);
            } else {
                prepForm(ejs, hp, (char*) qname.name, vp);
            }
        } else {
            //  MOB -- need a better C API for JSON. Should be non-pretty json.
            value = ejsToJson(ejs, vp);
            sep = (hp->requestContent) ? "&" : "";
            if (prefix) {
                newKey = mprStrcat(hp, -1, prefix, ".", key, NULL);
                encodedKey = mprUrlEncode(hp, newKey); 
                mprFree(newKey);
            } else {
                encodedKey = mprUrlEncode(hp, key); 
            }
            encodedValue = mprUrlEncode(hp, value->value);
            hp->requestContent = mprReallocStrcat(hp, -1, hp->requestContent, sep, encodedKey, "=", encodedValue, NULL);
        }
    }
}


static bool isPort(cchar *name)
{
    cchar   *cp;
    
    for (cp = name; *cp && *cp != '/'; cp++) {
        if (!isdigit((int) *cp) || *cp == '.') {
            return 0;
        }
    }
    return 1;
}


/*
 *  Prepare a URL by adding http:// as required
 */
static char *prepUri(MprCtx ctx, cchar *uri) 
{
    char    *newUri;

    if (*uri == '/') {
        newUri = mprStrcat(ctx, MPR_MAX_URL, "http://127.0.0.1", uri, NULL);
    } else if (strstr(uri, "http://") == 0 && strstr(uri, "https://") == 0) {
        if (isPort(uri)) {
            newUri = mprStrcat(ctx, MPR_MAX_URL, "http://127.0.0.1:", uri, NULL);
        } else {
            newUri = mprStrcat(ctx, MPR_MAX_URL, "http://", uri, NULL);
        }
    } else {
        newUri = mprStrdup(ctx, uri);
    }
    return newUri;
}
    

/*
 *  Mark the object properties for the garbage collector
 */
void markHttpVar(Ejs *ejs, EjsVar *parent, EjsHttp *http)
{
    ejsMarkObject(ejs, parent, (EjsObject*) http);
    if (http->responseCache) {
        ejsMarkVar(ejs, parent, (EjsVar*) http->responseCache);
    }
    if (http->callback) {
        ejsMarkVar(ejs, parent, (EjsVar*) http->callback);
    }
}

/*********************************** Factory **********************************/

void ejsCreateHttpType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = ejsCreateCoreType(ejs, ejsName(&qname, "ejs.io", "Http"), ejs->objectType, sizeof(EjsHttp), ES_ejs_io_Http,
        ES_ejs_io_Http_NUM_CLASS_PROP, ES_ejs_io_Http_NUM_INSTANCE_PROP, 
        EJS_ATTR_NATIVE | EJS_ATTR_OBJECT | EJS_ATTR_HAS_CONSTRUCTOR | EJS_ATTR_OBJECT_HELPERS);
    type->dontPool = 1;
    type->helpers->markVar = (EjsMarkVarHelper) markHttpVar;
}


void ejsConfigureHttpType(Ejs *ejs)
{
    EjsType     *type;

    if ((type = ejsGetType(ejs, ES_ejs_io_Http)) == 0) {
        return;
    }
    ejsBindMethod(ejs, type, ES_ejs_io_Http_Http, (EjsNativeFunction) httpConstructor);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_addHeader, (EjsNativeFunction) addHeader);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_available, (EjsNativeFunction) httpAvailable);
#if ES_ejs_io_Http_setCallback
    ejsBindMethod(ejs, type, ES_ejs_io_Http_setCallback, (EjsNativeFunction) setHttpCallback);
#endif
#if ES_ejs_io_Http_chunked
    ejsBindMethod(ejs, type, ES_ejs_io_Http_chunked, (EjsNativeFunction) getChunked);
#endif
#if ES_ejs_io_Http_set_chunked
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_chunked, (EjsNativeFunction) setChunked);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Http_close, (EjsNativeFunction) closeHttp);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_connect, (EjsNativeFunction) connectHttp);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_certificate, (EjsNativeFunction) getCertificate);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_certificate, (EjsNativeFunction) setCertificate);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_code, (EjsNativeFunction) code);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_codeString, (EjsNativeFunction) codeString);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_contentEncoding, (EjsNativeFunction) contentEncoding);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_contentLength, (EjsNativeFunction) contentLength);
#if ES_ejs_io_Http_set_contentLength
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_contentLength, (EjsNativeFunction) setContentLength);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Http_contentType, (EjsNativeFunction) contentType);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_date, (EjsNativeFunction) date);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_del, (EjsNativeFunction) del);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_expires, (EjsNativeFunction) expires);
#if ES_ejs_io_Http_flush
    ejsBindMethod(ejs, type, ES_ejs_io_Http_flush, (EjsNativeFunction) flushHttp);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Http_followRedirects, (EjsNativeFunction) getFollowRedirects);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_followRedirects, (EjsNativeFunction) setFollowRedirects);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_form, (EjsNativeFunction) form);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_get, (EjsNativeFunction) getMethod);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_head, (EjsNativeFunction) headMethod);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_header, (EjsNativeFunction) header);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_headers, (EjsNativeFunction) headers);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_isSecure, (EjsNativeFunction) isSecure);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_key, (EjsNativeFunction) getKey);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_key, (EjsNativeFunction) setKey);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_lastModified, (EjsNativeFunction) lastModified);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_method, (EjsNativeFunction) getMethodValue);
#if ES_ejs_io_Http_mimeType
    ejsBindMethod(ejs, type, ES_ejs_io_Http_mimeType, (EjsNativeFunction) getMimeType);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_method, (EjsNativeFunction) setMethodValue);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_post, (EjsNativeFunction) postMethod);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_put, (EjsNativeFunction) putMethod);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_read, (EjsNativeFunction) readHttpData);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_readString, (EjsNativeFunction) readStringHttp);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_readLines, (EjsNativeFunction) readLines);
#if BLD_FEATURE_EJS_E4X && UNUSED
    ejsBindMethod(ejs, type, ES_ejs_io_Http_readXml, (EjsNativeFunction) readXml);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Http_response, (EjsNativeFunction) httpResponse);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_options, (EjsNativeFunction) optionsMethod);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_setCredentials, (EjsNativeFunction) setCredentials);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_timeout, (EjsNativeFunction) getTimeout);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_timeout, (EjsNativeFunction) setTimeout);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_trace, (EjsNativeFunction) traceMethod);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_uri, (EjsNativeFunction) getUri);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_set_uri, (EjsNativeFunction) setUri);
    ejsBindMethod(ejs, type, ES_ejs_io_Http_write, (EjsNativeFunction) httpWrite);
#if ES_ejs_io_Http_wait
    ejsBindMethod(ejs, type, ES_ejs_io_Http_wait, (EjsNativeFunction) httpWait);
#endif
    ejsBindMethod(ejs, type, ES_ejs_io_Http_write, (EjsNativeFunction) httpWrite);
}


#endif /* ES_Http */

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
