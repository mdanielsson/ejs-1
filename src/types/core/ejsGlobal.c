/**
 *  ejsGlobal.c - Global functions and variables
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/*********************************** Locals ***********************************/
/**
 *  Assert a condition is true.
 *
 *  static function assert(condition: Boolean): Boolean
 */
static EjsVar *assertMethod(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    EjsFrame        *fp;
    EjsBoolean      *b;

    mprAssert(argc == 1);

    if (! ejsIsBoolean(argv[0])) {
        b = (EjsBoolean*) ejsCastVar(ejs, argv[0], ejs->booleanType);
    } else {
        b = (EjsBoolean*) argv[0];
    }
    mprAssert(b);

    if (b == 0 || !b->value) {
        fp = ejs->state->fp;
        if (fp->currentLine) {
            mprLog(ejs, 0, "Assertion error: %s", fp->currentLine);
            ejsThrowAssertError(ejs, "Assertion error: %s", fp->currentLine);
        } else {
            ejsThrowAssertError(ejs, "Assertion error");
        }
        return 0;
    }
    return vp;
}


/**
 *  Trap to the debugger
 *
 *  static function breakpoint(): Void
 */
static EjsVar *breakpoint(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    mprBreakpoint();
    return 0;
}


/**
 *  Clone the base class. Used by Record.es
 *
 *  static function cloneBase(klass: Type): Void
 */
static EjsVar *cloneBase(Ejs *ejs, EjsVar *ignored, int argc, EjsVar **argv)
{
    EjsType     *type;
    
    mprAssert(argc == 1);
    
    type = (EjsType*) argv[0];
    type->baseType = (EjsType*) ejsCloneVar(ejs, (EjsVar*) type->baseType, 0);
    return 0;
}


/*
 *  Reverse www-urlencoding on a string
 *
 *  function decodeURI(str: String): String
 */
static EjsVar *decodeURI(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *str;

    str = (EjsString*) argv[0];
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprUrlDecode(ejs, str->value));
}


/**
 *  Print the arguments to the standard error with a new line.
 *
 *  static function error(...args): void
 */
static EjsVar *error(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *s;
    EjsVar      *args, *vp;
    int         i, count, junk;

    mprAssert(argc == 1 && ejsIsArray(argv[0]));

    args = argv[0];
    count = ejsGetPropertyCount(ejs, args);

    for (i = 0; i < count; i++) {
        if ((vp = ejsGetProperty(ejs, args, i)) != 0) {
            if (!ejsIsString(vp)) {
                vp = ejsSerialize(ejs, vp, -1, 0, 0);
            }
            if (ejs->exception) {
                return 0;
            }
            if (vp) {
                s = (EjsString*) vp;
                junk = write(2, s->value, s->length);
            }
        }
    }
    junk = write(2, "\n", 1);
    return 0;
}


/*
 *  Perform www-urlencoding on a string
 *
 *  function encodeURI(str: String): String
 */
static EjsVar *encodeURI(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *str;

    str = (EjsString*) argv[0];
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprUrlEncode(ejs, str->value));
}


/*
 *  HTML escape a string
 *
 *  function escape(str: String): String
 */
static EjsVar *escape(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *str;

    str = (EjsString*) argv[0];
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprEscapeHtml(ejs, str->value));
}


/*
 *  function eval(script: String): String
 */
static EjsVar *eval(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    cchar       *script;

    script = ejsGetString(argv[0]);
    if (ejs->service->loadScriptLiteral) {
        return (ejs->service->loadScriptLiteral)(ejs, script);
    }
    ejsThrowStateError(ejs, "Ability to compile scripts not available");
    return 0;
}


/*
 *  Format the stack
 *
 *  function formatStack(): String
 */
static EjsVar *formatStackMethod(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    return (EjsVar*) ejsCreateString(ejs, ejsFormatStack(ejs, NULL));
}


#if ES_hashcode
/*
 *  Get the hash code for the object.
 *
 *  intrinsic function hashcode(o: Object): Number
 */
static EjsVar *hashcode(Ejs *ejs, EjsVar *vp, int argc, EjsVar **argv)
{
    mprAssert(argc == 1);
    return (EjsVar*) ejsCreateNumber(ejs, (MprNumber) PTOL(argv[0]));
}
#endif


/**
 *  Read a line of input
 *
 *  static function input(): String
 */
static EjsVar *input(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    MprFileSystem   *fs;
    MprBuf          *buf;
    EjsVar          *result;
    int             c;

    fs = mprGetMpr(ejs)->fileSystem;

    buf = mprCreateBuf(ejs, -1, -1);
    while ((c = getchar()) != EOF) {
#if BLD_WIN_LIKE
        if (c == fs->newline[0]) {
            continue;
        } else if (c == fs->newline[1]) {
            break;
        }
#else
        if (c == fs->newline[0]) {
            break;
        }
#endif
        mprPutCharToBuf(buf, c);
    }
    if (c == EOF && mprGetBufLength(buf) == 0) {
        return (EjsVar*) ejs->nullValue;
    }
    mprAddNullToBuf(buf);
    result = (EjsVar*) ejsCreateString(ejs, mprGetBufStart(buf));
    mprFree(buf);
    return result;
}


/**
 *  Load a script or module. Name should have an extension. Name will be located according to the EJSPATH search strategy.
 *
 *  static function load(filename: String): void
 */
static EjsVar *load(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    cchar       *path, *cp;

    path = ejsGetString(argv[0]);

    if ((cp = strrchr(path, '.')) != NULL && strcmp(cp, EJS_MODULE_EXT) != 0) {
        if (ejs->service->loadScriptFile == 0) {
            ejsThrowIOError(ejs, "load: Compiling is not enabled for %s", path);
        } else {
            (ejs->service->loadScriptFile)(ejs, path);
        }
    } else {
        /* This will throw on errors */
        ejsLoadModule(ejs, path, -1, -1, 0, NULL);
    }
    return 0;
}


/**
 *  Compute an MD5 checksum
 *
 *  static function md5(name: String): void
 */
static EjsVar *md5(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *str;

    str = (EjsString*) argv[0];
    return (EjsVar*) ejsCreateStringAndFree(ejs, mprGetMD5Hash(ejs, str->value, str->length, NULL));
}


/**
 *  Parse the input and convert to a primitive type
 *
 *  static function parse(input: String, preferredType: Type = null): void
 */
static EjsVar *parse(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    cchar       *input;
    int         preferred;

    input = ejsGetString(argv[0]);

    if (argc == 2 && !ejsIsType(argv[1])) {
        ejsThrowArgError(ejs, "Argument is not a type");
        return 0;
    }
    preferred = (argc == 2) ? ((EjsType*) argv[1])->id : -1;
    while (isspace((int) *input)) {
        input++;
    }
    return ejsParseVar(ejs, input, preferred);
}


/**
 *  Print the arguments to the standard output with a new line.
 *
 *      static function output(...args): void
 *      static function print(...args): void
 */
static EjsVar *outputData(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *s;
    EjsVar      *args, *vp;
    char        *cp, *tmp;
    int         i, count, junk;

    mprAssert(argc == 1 && ejsIsArray(argv[0]));

    args = argv[0];
    count = ejsGetPropertyCount(ejs, args);

    for (i = 0; i < count; i++) {
        if ((vp = ejsGetProperty(ejs, args, i)) != 0) {
            if (ejsIsString(vp)) {
                s = (EjsString*) vp;
            } else {
                s  = (EjsString*) ejsToString(ejs, vp);
            }
            if (ejs->exception) {
                return 0;
            }
            if (vp && s) {
                if (ejsIsObject(vp) && s->length > 0 && s->value[0] == '"') {
                    tmp = mprStrdup(ejs, s->value);
                    cp = mprStrTrim(tmp, "\"");
                    junk = write(1, cp, strlen(cp));
                    mprFree(tmp);
                } else {
                    junk = write(1, s->value, s->length);
                }
            }
        }
    }
    junk = write(1, "\n", 1);
    return 0;
}


#if ES_printv && BLD_DEBUG
/**
 *  Print the named variables for debugging.
 *
 *  static function printv(...args): void
 */
static EjsVar *printv(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsString   *s;
    EjsVar      *args, *vp;
    int         i, count;

    mprAssert(argc == 1 && ejsIsArray(argv[0]));

    args = argv[0];
    count = ejsGetPropertyCount(ejs, args);

    for (i = 0; i < count; i++) {
        vp = ejsGetProperty(ejs, args, i);
        if (vp == 0) {
            continue;
        }
        if (vp->seq == 28092) {
            printf("GOT YOU\n");
        }
        
        s = (EjsString*) ejsToJson(ejs, vp);

        if (ejs->exception) {
            return 0;
        }

        mprAssert(s && ejsIsString(s));
        mprPrintf(ejs, "%s = %s\n", mprGetName(vp), s->value);
    }
    mprPrintf(ejs, "\n");
    return 0;
}
#endif


static EjsNamespace *addNamespace(Ejs *ejs, EjsBlock *block, cchar *space)
{
    EjsNamespace    *ns;

    ns = ejsDefineReservedNamespace(ejs, block, 0, space);
    mprAddHash(ejs->standardSpaces, space, ns);
    return ns;
}


void ejsCreateGlobalBlock(Ejs *ejs)
{
    EjsBlock    *block;

    /*
     *  Pre-create extra global slots
     */
    ejs->globalBlock = ejsCreateBlock(ejs, max(ES_global_NUM_CLASS_PROP, 256));
    ejs->global = (EjsVar*) ejs->globalBlock;
    ejsSetDebugName(ejs->global, "global");
    
    if (ejs->flags & EJS_FLAG_EMPTY) {
        ejs->globalBlock->obj.numProp = 0;
    } else {
        ejs->globalBlock->obj.numProp = ES_global_NUM_CLASS_PROP;
    }
    
    block = (EjsBlock*) ejs->global;
    
    /*
     *  Create the standard namespaces. Order matters here. This is the (reverse) order of lookup.
     *  Empty is first intrinsic is last.
     */
    ejs->configSpace =      addNamespace(ejs, block, EJS_CONFIG_NAMESPACE);
    ejs->iteratorSpace =    addNamespace(ejs, block, EJS_ITERATOR_NAMESPACE);
    ejs->intrinsicSpace =   addNamespace(ejs, block, EJS_INTRINSIC_NAMESPACE);
    ejs->eventsSpace =      addNamespace(ejs, block, EJS_EVENTS_NAMESPACE);
    ejs->ioSpace =          addNamespace(ejs, block, EJS_IO_NAMESPACE);
    ejs->sysSpace =         addNamespace(ejs, block, EJS_SYS_NAMESPACE);
    ejs->publicSpace =      addNamespace(ejs, block, EJS_PUBLIC_NAMESPACE);
    ejs->emptySpace =       addNamespace(ejs, block, EJS_EMPTY_NAMESPACE);
}


void ejsConfigureGlobalBlock(Ejs *ejs)
{
    EjsBlock    *block;

    block = ejs->globalBlock;
    mprAssert(block);

#if ES_assert
    ejsBindFunction(ejs, block, ES_assert, assertMethod);
#endif
#if ES_breakpoint
    ejsBindFunction(ejs, block, ES_breakpoint, breakpoint);
#endif
#if ES_cloneBase
    ejsBindFunction(ejs, block, ES_cloneBase, (EjsNativeFunction) cloneBase);
#endif
#if ES_decodeURI
    ejsBindFunction(ejs, block, ES_decodeURI, decodeURI);
#endif
#if ES_error
    ejsBindFunction(ejs, block, ES_error, error);
#endif
#if ES_encodeURI
    ejsBindFunction(ejs, block, ES_encodeURI, encodeURI);
#endif
#if ES_escape
    ejsBindFunction(ejs, block, ES_escape, escape);
#endif
#if ES_eval
    ejsBindFunction(ejs, block, ES_eval, eval);
#endif
#if ES_formatStack
    ejsBindFunction(ejs, block, ES_formatStack, formatStackMethod);
#endif
#if ES_hashcode
    ejsBindFunction(ejs, block, ES_hashcode, hashcode);
#endif
#if ES_input
    ejsBindFunction(ejs, block, ES_input, input);
#endif
#if ES_load
    ejsBindFunction(ejs, block, ES_load, load);
#endif
#if ES_md5
    ejsBindFunction(ejs, block, ES_md5, md5);
#endif
#if ES_output
    ejsBindFunction(ejs, block, ES_output, outputData);
#endif
#if ES_parse
    ejsBindFunction(ejs, block, ES_parse, parse);
#endif
#if ES_print
    /* Just uses output */
    ejsBindFunction(ejs, block, ES_print, outputData);
#endif
#if ES_printv && BLD_DEBUG
    ejsBindFunction(ejs, block, ES_printv, printv);
#endif
    ejsConfigureJSON(ejs);
    /*
     *  Update the global reference
     */
    ejsSetProperty(ejs, ejs->global, ES_global, ejs->global);
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
    vim: sw=8 ts=8 expandtab

    @end
 */
