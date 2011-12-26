/**
 *  ejsJSON.c - JSON encoding and decoding
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/*********************************** Locals ***********************************/

typedef struct JsonState {
    char    *data;
    char    *end;
    char    *next;
    char    *error;
} JsonState;

/***************************** Forward Declarations ***************************/

static EjsVar *parseLiteral(Ejs *ejs, JsonState *js);
static EjsVar *parseLiteralInner(Ejs *ejs, MprBuf *buf, JsonState *js);

/*********************************** Locals ***********************************/
/*
 *  Convert a string into an object.
 *
 *  function deserialize(obj: String): Object
 */
EjsVar *deserialize(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    mprAssert(argc == 1 && ejsIsString(argv[0]));
    return ejsDeserialize(ejs, (EjsString*) argv[0]);
}


EjsVar *ejsDeserialize(Ejs *ejs, EjsString *str)
{
    EjsVar      *obj;
    JsonState   js;
    char        *data;

    if (!ejsIsString(str)) {
        return 0;
    }
    data = ejsGetString(str);
    if (data == 0) {
        return 0;
    } else if (*data == '\0') {
        return (EjsVar*) ejs->emptyStringValue;
    }

    js.next = js.data = data;
    js.end = &js.data[str->length];
    js.error = 0;
    if ((obj = parseLiteral(ejs, &js)) == 0) {
        if (js.error) {
            ejsThrowSyntaxError(ejs, 
                "Can't parse object literal. Error at position %d.\n"
                "===========================\n"
                "Offending text: %s\n"
                "===========================\n"
                "In literal %s"
                "\n===========================\n",
                (int) (js.error - js.data), js.error, js.data);
        } else {
            ejsThrowSyntaxError(ejs, "Can't parse object literal. Undefined error");
        }
        return 0;
    }
    return obj;
}


static EjsVar *parseLiteral(Ejs *ejs, JsonState *js)
{
    MprBuf      *buf;
    EjsVar      *vp;

    mprAssert(js);

    buf = mprCreateBuf(ejs, 0, 0);
    vp = parseLiteralInner(ejs, buf, js);
    mprFree(buf);
    return vp;
}


typedef enum Token {
    TOK_ERR,            /* Error */
    TOK_EOF,            /* End of input */
    TOK_LBRACE,         /* { */
    TOK_LBRACKET,       /* [ */
    TOK_RBRACE,         /* } */
    TOK_RBRACKET,       /* ] */
    TOK_COLON,          /* : */
    TOK_COMMA,          /* , */
    TOK_ID,             /* Unquoted ID */
    TOK_QID,            /* Quoted ID */
} Token;


Token getNextJsonToken(MprBuf *buf, char **token, JsonState *js)
{
    uchar   *start, *cp, *end, *next;
    char    *src, *dest;
    int     quote, tid, c;

    next = (uchar*) js->next;
    end = (uchar*) js->end;

    if (buf) {
        mprFlushBuf(buf);
    }
    for (cp = next; cp < end && isspace((int) *cp); cp++) {
        ;
    }
    next = cp + 1;

    if (*cp == '\0') {
        tid = TOK_EOF;

    } else  if (*cp == '{') {
        tid = TOK_LBRACE;

    } else if (*cp == '[') {
        tid = TOK_LBRACKET;

    } else if (*cp == '}' || *cp == ']') {
        tid = *cp == '}' ? TOK_RBRACE: TOK_RBRACKET;
        while (*++cp && isspace((int) *cp)) ;
        if (*cp == ',' || *cp == ':') {
            cp++;
        }
        next = cp;

    } else {
        if (*cp == '"' || *cp == '\'') {
            tid = TOK_QID;
            quote = *cp++;
            for (start = cp; cp < end; cp++) {
                if (*cp == '\\') {
                    if (cp[1] == quote) {
                        cp++;
                    }
                    continue;
                }
                if (*cp == quote) {
                    break;
                }
            }
        } else {
            quote = -1;
            tid = TOK_ID;
            for (start = cp; cp < end; cp++) {
                if (*cp == '\\') {
                    continue;
                }
                /* Not an allowable character outside quotes */
                if (!(isalnum((int) *cp) || *cp == '_' || *cp == ' ' || *cp == '-' || *cp == '+' || *cp == '.')) {
                    break;
                }
            }
        }
        if (buf) {
            mprPutBlockToBuf(buf, (char*) start, (int) (cp - start));
            mprAddNullToBuf(buf);
        }
        if (quote > 0) {
            if (*cp == quote) {
                cp++;
            } else {
                js->error = (char*) cp;
                return TOK_ERR;
            }
        }
        if (*cp == ',' || *cp == ':') {
            cp++;
        } else if (*cp != '}' && *cp != ']' && *cp != '\0' && *cp != '\n' && *cp != '\r' && *cp != ' ') {
            js->error = (char*) cp;
            return TOK_ERR;
        }
        next = cp;

        if (buf) {
            for (dest = src = buf->start; src < buf->end; ) {
                c = *src++;
                if (c == '\\') {
                    c = *src++;
                    if (c == 'r') {
                        c = '\r';
                    } else if (c == 'n') {
                        c = '\n';
                    } else if (c == 'b') {
                        c = '\b';
                    }
                }
                *dest++ = c;
            }
            *dest = '\0';
            *token = mprGetBufStart(buf);
        }
    }
    js->next = (char*) next;
    return tid;
}


Token peekNextJsonToken(JsonState *js)
{
    JsonState   discard = *js;
    return getNextJsonToken(NULL, NULL, &discard);
}


/*
 *  Parse an object literal string pointed to by js->next into the given buffer. Update js->next to point
 *  to the next input token in the object literal. Supports nested object literals.
 */
static EjsVar *parseLiteralInner(Ejs *ejs, MprBuf *buf, JsonState *js)
{
    EjsName     qname;
    EjsVar      *obj, *vp;
    MprBuf      *valueBuf;
    char        *token, *key, *value;
    int         tid, isArray;

    isArray = 0;

    tid = getNextJsonToken(buf, &token, js);
    if (tid == TOK_ERR || tid == TOK_EOF) {
        return 0;
    }
    if (tid == TOK_LBRACKET) {
        isArray = 1;
        obj = (EjsVar*) ejsCreateArray(ejs, 0);
    } else if (tid == TOK_LBRACE) {
        obj = (EjsVar*) ejsCreateObject(ejs, ejs->objectType, 0);
    } else {
        return ejsParseVar(ejs, token, ES_String);
    }
    if (obj == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }

    while (1) {
        vp = 0;
        tid = peekNextJsonToken(js);
        if (tid == TOK_ERR) {
            return 0;
        } else if (tid == TOK_EOF) {
            break;
        } else if (tid == TOK_RBRACE || tid == TOK_RBRACKET) {
            getNextJsonToken(buf, &key, js);
            break;
        }
        if (tid == TOK_LBRACKET) {
            /* For array values */
            vp = parseLiteral(ejs, js);
            mprAssert(vp);
            
        } else if (tid == TOK_LBRACE) {
            /* For object values */
            vp = parseLiteral(ejs, js);
            mprAssert(vp);
            
        } else if (isArray) {
            tid = getNextJsonToken(buf, &value, js);
            vp = ejsParseVar(ejs, value, (tid == TOK_QID) ? ES_String : -1);
            mprAssert(vp);
            
        } else {
            getNextJsonToken(buf, &key, js);
            tid = peekNextJsonToken(js);
            if (tid == TOK_ERR) {
                mprAssert(0);
                return 0;
            } else if (tid == TOK_EOF) {
                break;
            } else if (tid == TOK_LBRACE || tid == TOK_LBRACKET) {
                vp = parseLiteral(ejs, js);
                mprAssert(vp);

            } else if (tid == TOK_ID || tid == TOK_QID) {
                valueBuf = mprCreateBuf(ejs, 0, 0);
                getNextJsonToken(valueBuf, &value, js);
                if (tid == TOK_QID) {
                    vp = (EjsVar*) ejsCreateString(ejs, value);
                } else {
                    if (strcmp(value, "null") == 0) {
                        vp = ejs->nullValue;
                    } else if (strcmp(value, "undefined") == 0) {
                        vp = ejs->undefinedValue;
                    } else {
                        vp = ejsParseVar(ejs, value, -1);
                    }
                }
                mprAssert(vp);
                mprFree(valueBuf);
            } else {
                getNextJsonToken(buf, &value, js);
                mprAssert(0);
                js->error = js->next;
                return 0;
            }
        }
        if (vp == 0) {
            js->error = js->next;
            return 0;
        }
        if (isArray) {
            if (ejsSetProperty(ejs, obj, -1, vp) < 0) {
                ejsThrowMemoryError(ejs);
                return 0;
            }
        } else {
            /*
             *  Must not pool this object otherwise the key allocation will be leak. Need the var to be freed.
             */
            key = mprStrdup(obj, key);
            ejsName(&qname, EJS_EMPTY_NAMESPACE, key);
            obj->noPool = 1;
            if (ejsSetPropertyByName(ejs, obj, &qname, vp) < 0) {
                ejsThrowMemoryError(ejs);
                return 0;
            }
        }
    }
    return obj;
}


EjsVar *ejsSerialize(Ejs *ejs, EjsVar *vp, int maxDepth, bool showAll, bool showBase)
{
    int   flags;

    if (maxDepth <= 0) {
        maxDepth = MAXINT;
    }
    flags = 0;
    if (showAll) {
        flags |= EJS_FLAGS_ENUM_ALL;
    }
    if (showBase) {
        flags |= EJS_FLAGS_ENUM_INHERITED;
    }
    return (EjsVar*) ejsToJson(ejs, vp); 
}


/*
 *  Convert the object to a source code string.
 *
 *  intrinsic function serialize(obj: Object, maxDepth: Number = 0, showAll: Boolean = false, 
 *      showBase: Boolean = false): String
 */
static EjsVar *serialize(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    int     maxDepth;
    bool    showBase, showAll;

    maxDepth = MAXINT;

    if (argc >= 2) {
        maxDepth = ejsGetInt(argv[1]);
    }
    showAll = (argc >= 3 && argv[2] == (EjsVar*) ejs->trueValue);
    showBase = (argc == 4 && argv[3] == (EjsVar*) ejs->trueValue);
    return ejsSerialize(ejs, argv[0], maxDepth, showAll, showBase);
}


void ejsConfigureJSON(Ejs *ejs)
{
    EjsBlock    *block;

    block = ejs->globalBlock;
    mprAssert(block);

#if ES_deserialize
    ejsBindFunction(ejs, block, ES_deserialize, deserialize);
#endif
#if ES_serialize
    ejsBindFunction(ejs, block, ES_serialize, serialize);
#endif
}


/*
 *  @copy   default
 *
 *  Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
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
