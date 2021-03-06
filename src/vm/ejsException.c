/**
 *  ejsException.c - Error Exception class
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/************************************ Code ************************************/
/*
 *  Throw an exception.
 */
EjsVar *ejsThrowException(Ejs *ejs, EjsVar *obj)
{
    mprAssert(obj);

    /*
     *  Set ejs->exception. The VM will notice this and initiate exception handling
     */
    ejs->exception = obj;
    ejs->attention = 1;
    return obj;
}

void ejsClearException(Ejs *ejs)
{
    ejs->exception = 0;
}


/*
 *  Create an exception object.
 */
static EjsVar *createException(Ejs *ejs, EjsType *type, cchar* fmt, va_list fmtArgs)
{
    EjsVar          *error, *argv[1];
    char            *msg;

    mprAssert(type);

    if (ejs->noExceptions) {
        return 0;
    }
    msg = mprVasprintf(ejs, -1, fmt, fmtArgs);
    argv[0] = (EjsVar*) ejsCreateString(ejs, msg);

    if (argv[0] == 0) {
        mprAssert(argv[0]);
        return 0;
    }
    error = (EjsVar*) ejsCreateInstance(ejs, type, 1, argv);
    if (error == 0) {
        mprAssert(error);
        return 0;
    }
    mprFree(msg);
    return error;
}


EjsVar *ejsCreateException(Ejs *ejs, int slot, cchar *fmt, va_list fmtArgs)
{
    EjsType     *type;
    EjsVar      *error;
    char        *buf;

    if (ejs->exception) {
        buf = mprVasprintf(ejs, 0, fmt, fmtArgs);
        mprError(ejs, "Double exception: %s", buf);
        mprFree(buf);
        return ejs->exception;
    }
    if (!ejs->initialized || (ejs->flags & EJS_FLAG_EMPTY)) {
        buf = mprVasprintf(ejs, 0, fmt, fmtArgs);
        mprError(ejs, "Exception: %s", buf);
        mprFree(buf);
        return ejs->exception;
    }
    type = (EjsType*) ejsGetProperty(ejs, ejs->global, slot);
    if (type == 0) {
        type = ejs->errorType;
    }
    error = createException(ejs, type, fmt, fmtArgs);
    if (error) {
        ejsThrowException(ejs, error);
    }
    return error;
}


EjsVar *ejsThrowArgError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_ArgError, fmt, fmtArgs);
}


EjsVar *ejsThrowArithmeticError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_ArithmeticError, fmt, fmtArgs);
}


EjsVar *ejsThrowAssertError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_AssertError, fmt, fmtArgs);
}


EjsVar *ejsThrowInstructionError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_InstructionError, fmt, fmtArgs);
}


EjsVar *ejsThrowError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_Error, fmt, fmtArgs);
}


EjsVar *ejsThrowIOError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_IOError, fmt, fmtArgs);
}


EjsVar *ejsThrowInternalError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_InternalError, fmt, fmtArgs);
}


EjsVar *ejsThrowMemoryError(Ejs *ejs)
{
    /*
        Don't do double exceptions for memory errors
     */
    if (ejs->exception == 0) {
		va_list dummy = VA_NULL;
        return ejsCreateException(ejs, ES_MemoryError, "Memory Error", dummy);
    }
    return ejs->exception;
}


EjsVar *ejsThrowOutOfBoundsError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_OutOfBoundsError, fmt, fmtArgs);
}


EjsVar *ejsThrowReferenceError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_ReferenceError, fmt, fmtArgs);
}


EjsVar *ejsThrowResourceError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_ResourceError, fmt, fmtArgs);
}


EjsVar *ejsThrowStateError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_StateError, fmt, fmtArgs);
}


EjsVar *ejsThrowSyntaxError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_SyntaxError, fmt, fmtArgs);
}


EjsVar *ejsThrowTypeError(Ejs *ejs, cchar *fmt, ...)
{
    va_list     fmtArgs;

    mprAssert(fmt);
    va_start(fmtArgs, fmt);
    return ejsCreateException(ejs, ES_TypeError, fmt, fmtArgs);
}


/*
 *  Format the stack backtrace
 */
char *ejsFormatStack(Ejs *ejs, EjsError *error)
{
    EjsType         *type;
    EjsFrame        *fp;
    cchar           *typeName, *functionName, *line, *typeSep, *codeSep;
    char            *backtrace, *traceLine;
    int             level, len, oldFlags;

    mprAssert(ejs);

    backtrace = 0;
    len = 0;
    level = 0;

    /*
     *  Pretend to be the compiler so we can access function frame names
     */
    oldFlags = ejs->flags;
    ejs->flags |= EJS_FLAG_COMPILER;

    for (fp = ejs->state->fp; fp; fp = fp->caller) {

        typeName = "";
        functionName = "global";

        if (fp->currentLine == 0) {
            line = "";
        } else {
            for (line = fp->currentLine; *line && isspace((int) *line); line++) {
                ;
            }
        }
        if (fp) {
            if (fp->function.owner && fp->function.slotNum >= 0) {
                functionName = ejsGetPropertyName(ejs, fp->function.owner, fp->function.slotNum).name;
            }
            if (ejsIsType(fp->function.owner)) {
                type = (EjsType*) fp->function.owner;
                if (type) {
                    typeName = type->qname.name;
                }
            }
        }
        typeSep = (*typeName) ? "." : "";
        codeSep = (*line) ? "->" : "";

        if (error && backtrace == 0) {
            error->filename = mprStrdup(error, fp->filename);
            error->lineNumber = fp->lineNumber;
        }
        if ((traceLine = mprAsprintf(ejs, MPR_MAX_STRING, " [%02d] %s, %s%s%s, line %d %s %s\n",
                level++, fp->filename ? fp->filename : "script", typeName, typeSep, functionName,
                fp->lineNumber, codeSep, line)) == NULL) {
            break;
        }
        backtrace = (char*) mprRealloc(ejs, backtrace, len + (int) strlen(traceLine) + 1);
        if (backtrace == 0) {
            return 0;
        }
        memcpy(&backtrace[len], traceLine, strlen(traceLine) + 1);
        len += (int) strlen(traceLine);
        mprFree(traceLine);
    }
    ejs->flags = oldFlags;
    if (error) {
        error->stack = backtrace;
    }
    return backtrace;
}


/*
 *  Public routine to set the error message. Caller MUST NOT free.
 */
char *ejsGetErrorMsg(Ejs *ejs, int withStack)
{
    EjsVar      *message, *stack, *error;
    cchar       *name;
    char        *buf;

    if (ejs->flags & EJS_FLAG_EMPTY) {
        return "";
    }

    error = (EjsVar*) ejs->exception;
    message = stack = 0;
    name = 0;

    if (error) {
        name = error->type->qname.name;
        if (ejsIsA(ejs, error, ejs->errorType)) {
            message = ejsGetProperty(ejs, error, ES_Error_message);
            stack = ejsGetProperty(ejs, error, ES_Error_stack);

        } else if (ejsIsString(error)) {
            name = "Error";
            message = error;

        } else if (ejsIsNumber(error)) {
            name = "Error";
            message = error;
            
        } else if (error == (EjsVar*) ejs->stopIterationType) {
            name = "StopIteration";
            message = (EjsVar*) ejsCreateString(ejs, "Uncaught StopIteration exception");
        }
    }
    if (!withStack) {
        stack = 0;
    }

    if (stack && ejsIsString(stack) && message && ejsIsString(message)){
        buf = mprAsprintf(ejs, -1, "%s Exception: %s\nStack:\n%s", name, ((EjsString*) message)->value, 
            ((EjsString*) stack)->value);

    } else if (message && ejsIsString(message)){
        buf = mprAsprintf(ejs, -1, "%s: %s", name, ((EjsString*) message)->value);

    } else if (message && ejsIsNumber(message)){
        buf = mprAsprintf(ejs, -1, "%s: %d", name, ((EjsNumber*) message)->value);
        
    } else {
        if (error) {
            buf = mprStrdup(ejs, "Unknown exception object type");
        } else {
            buf = mprStrdup(ejs, "");
        }
    }
    mprFree(ejs->errorMsg);
    ejs->errorMsg = buf;
    return buf;
}


bool ejsHasException(Ejs *ejs)
{
    return ejs->exception != 0;
}


EjsVar *ejsGetException(Ejs *ejs)
{
    return ejs->exception;
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
