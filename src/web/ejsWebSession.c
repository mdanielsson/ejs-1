/**
 *  ejsWebSession.c - Native code for the Session class.
 *
 *  The Session class serializes objects that are stored to the session object.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if BLD_FEATURE_EJS_WEB
/*********************************** Forwards *********************************/

static void sessionActivity(Ejs *ejs, EjsWebSession *sp);
static void sessionTimer(EjsWebControl *control, MprEvent *event);

/************************************* Code ***********************************/

static EjsVar *getSessionProperty(Ejs *ejs, EjsWebSession *sp, int slotNum)
{
    EjsVar      *vp;
    EjsWeb      *web;
    Ejs         *master;

    web = ejs->handle;
    if (web->session != sp) {
        return (EjsVar*) ejs->emptyStringValue;
    }

    master = ejs->master ? ejs->master : ejs;
    ejsLockVm(master);

    vp = ejs->objectHelpers->getProperty(ejs, (EjsVar*) sp, slotNum);
    if (vp) {
        vp = ejsDeserialize(ejs, (EjsString*) vp);
    }
    if (vp == ejs->undefinedValue) {
        vp = (EjsVar*) ejs->emptyStringValue;
    }
    sessionActivity(ejs, sp);
    ejsUnlockVm(master);
    return vp;
}


static EjsVar *getSessionPropertyByName(Ejs *ejs, EjsWebSession *sp, EjsName *qname)
{
    EjsVar      *vp;
    EjsWeb      *web;
    Ejs         *master;
    int         slotNum;

    web = ejs->handle;
    if (web->session != sp) {
        return (EjsVar*) ejs->emptyStringValue;
    }
    qname->space = EJS_EMPTY_NAMESPACE;

    master = ejs->master ? ejs->master : ejs;
    ejsLockVm(master);

    slotNum = ejs->objectHelpers->lookupProperty(ejs, (EjsVar*) sp, qname);
    if (slotNum < 0) {
        /*
         *  Return empty string so that web pages can access session values without having to test for null/undefined
         */
        vp = (EjsVar*) ejs->emptyStringValue;
    } else {
        vp = ejs->objectHelpers->getProperty(ejs, (EjsVar*) sp, slotNum);
        if (vp) {
            vp = ejsDeserialize(ejs, (EjsString*) vp);
        }
    }
    sessionActivity(ejs, sp);
    ejsUnlockVm(master);
    return vp;
}


static int setSessionProperty(Ejs *ejs, EjsWebSession *sp, int slotNum, EjsVar *value)
{
    Ejs     *master;
    EjsWeb  *web;
    
    web = ejs->handle;
    if (web->session != sp) {
        mprAssert(0);
        return EJS_ERR;
    }

    /*
     *  Allocate the serialized object using the master interpreter
     */
    master = ejs->master ? ejs->master : ejs;
    ejsLockVm(master);

    value = (EjsVar*) ejsSerialize(master, value, 0, 0, 0);
    slotNum = master->objectHelpers->setProperty(master, (EjsVar*) sp, slotNum, value);
    sessionActivity(ejs, sp);
    ejsUnlockVm(master);
    return slotNum;
}


/*
 *  Update the session expiration time due to activity
 */
static void sessionActivity(Ejs *ejs, EjsWebSession *sp)
{
    sp->expire = mprGetTime(ejs) + sp->timeout * MPR_TICKS_PER_SEC;
}


/*
 *  Check for expired sessions
 */
static void sessionTimer(EjsWebControl *control, MprEvent *event)
{
    Ejs             *master;
    EjsObject       *sessions;
    EjsWebSession   *session;
    MprTime         now;
    int             i, count, deleted;

    now = mprGetTime(control);

    sessions = control->sessions;
    master = control->master;
    if (master == 0) {
        mprAssert(master);
        return;
    }

    /*
     *  This could be on the primary event thread. Can't block long.
     */
    if (mprTryLock(master->mutex)) {
        count = ejsGetPropertyCount(master, (EjsVar*) sessions);
        deleted = 0;
        for (i = count - 1; i >= 0; i--) {
            session = (EjsWebSession*) ejsGetProperty(master, (EjsVar*) sessions, i);
            if (session->obj.var.type == control->sessionType) {
                if (session && session->expire <= now) {
                    ejsDeleteProperty(master, (EjsVar*) sessions, i);
                    ejsRemoveSlot(master, sessions, i, 1);
                    deleted++;
                }
            }
        }
        if (deleted) {
            ejsCollectGarbage(master, EJS_GEN_NEW);
        }
        if (count == 0) {
            control->sessionTimer = 0;
            mprFree(event);
        }
        mprUnlock(master->mutex);
    }
}


void ejsParseWebSessionCookie(EjsWeb *web)
{
    EjsName         qname;
    EjsWebControl   *control;
    char            *cookie, *id, *cp, *value;
    int             quoted, len;

    cookie = web->cookie;

    while (cookie && (value = strstr(cookie, EJS_SESSION)) != 0) {
        value += strlen(EJS_SESSION);
        while (isspace((int) *value) || *value == '=') {
            value++;
        }
        quoted = 0;
        if (*value == '"') {
            value++;
            quoted++;
        }
        for (cp = value; *cp; cp++) {
            if (quoted) {
                if (*cp == '"' && cp[-1] != '\\') {
                    break;
                }
            } else {
                if ((*cp == ',' || *cp == ';') && cp[-1] != '\\') {
                    break;
                }
            }
        }
        control = web->control;

        len = (int) (cp - value);
        id = mprMemdup(web, value, len + 1);
        id[len] = '\0';

        if (control->master) {
            ejsName(&qname, "", id);
            web->session = (EjsWebSession*) ejsGetPropertyByName(control->master, (EjsVar*) control->sessions, &qname);
        }
        mprFree(id);
        cookie = value;
    }
}

/*
 *  Create a new session object. This is created in the master interpreter and will persist past the life 
 *  of the current request. This will allocate a new session ID. Timeout is in seconds.
 */
EjsWebSession *ejsCreateSession(Ejs *ejs, int timeout, bool secure)
{
    Ejs             *master;
    EjsWeb          *web;
    EjsWebControl   *control;
    EjsWebSession   *session;
    EjsType         *sessionType;
    EjsName         qname;
    EjsVar          *vp;
    MprTime         now, expire;
    char            idBuf[64], *id;
    int             slotNum, next, count;

    master = ejs->master;
    if (master == 0) {
        return 0;
    }
    web = ejsGetHandle(ejs);
    control = web->control;

    if (timeout <= 0) {
        timeout = control->sessionTimeout;
    }

#if ES_ejs_web_Session
    sessionType = ejsGetType(ejs, ES_ejs_web_Session);
#else
    sessionType = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.web", "Session"));
#endif
    if (sessionType == 0) {
        mprAssert(0);
        return 0;
    }
    web->control->sessionType = sessionType;

    now = mprGetTime(ejs);
    expire = now + timeout * MPR_TICKS_PER_SEC;

    ejsLockVm(master);
    web->session = session = (EjsWebSession*) ejsCreateObject(master, sessionType, 0);
    if (session == 0) {
        ejsUnlockVm(master);
        return 0;
    }
    session->timeout = timeout;
    session->expire = mprGetTime(ejs) + timeout * MPR_TICKS_PER_SEC;

    /*
     *  Use an MD5 prefix of "x" to avoid the hash being interpreted as a numeric index.
     */
    next = control->nextSession++;
    mprSprintf(idBuf, sizeof(idBuf), "%08x%08x%d", PTOI(ejs) + PTOI(web) + PTOI(expire), (int) now, next);
    id = mprGetMD5Hash(session, idBuf, sizeof(idBuf), "x");
    if (id == 0) {
        mprFree(session);
        ejsUnlockVm(master);
        return 0;
    }
    session->id = mprStrdup(session, id);

    /*
        Scan for a null property or use next free slot
     */
    count = ejsGetPropertyCount(ejs, (EjsVar*) control->sessions);
    for (slotNum = 0; slotNum < count; slotNum++) {
        vp = ejsGetProperty(ejs, (EjsVar*) control->sessions, slotNum);
        if (vp == 0 || vp == ejs->nullValue) {
            break;
        }
    }
    ejsSetProperty(control->master, (EjsVar*) control->sessions, slotNum, (EjsVar*) session);
    ejsSetPropertyName(control->master, (EjsVar*) control->sessions, slotNum, EN(&qname, session->id));
    session->index = slotNum;

    if (control->sessionTimer == 0) {
        control->sessionTimer = mprCreateTimerEvent(mprGetDispatcher(ejs), (MprEventProc) sessionTimer, EJS_TIMER_PERIOD, 
            MPR_NORMAL_PRIORITY, control, MPR_EVENT_CONTINUOUS);
    }
    ejsUnlockVm(master);

    mprLog(ejs, 3, "Created new session %s", id);

    /*
     *  Create a cookie that will only live while the browser is not exited. (Set timeout to zero).
     */
    ejsSetCookie(ejs, EJS_SESSION, id, "/", NULL, 0, secure);
    return session;
}


bool ejsDestroySession(Ejs *ejs)
{
    EjsWeb          *web;
    EjsWebControl   *control;
    EjsName         qname;
    int             rc;

    web = ejs->handle;
    control = web->control;

    if (web->session == 0) {
        return 0;
    }
    rc = ejsDeletePropertyByName(control->master, (EjsVar*) control->sessions, ejsName(&qname, "", web->session->id));
    web->session = 0;
    return rc;
}


static void destroySession(Ejs *ejs, EjsWebSession *session)
{
    mprAssert(session);

    mprFree(session->id);
    session->id = 0;
    ejsFreeVar(ejs, (EjsVar*) session, -1);
}


void ejsConfigureWebSessionType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.web", "Session"));
    if (type == 0) {
        if (!(ejs->flags & EJS_FLAG_EMPTY)) {
            mprError(ejs, "Can't find web Session class");
            ejs->hasError = 1;
        }
        return;
    }
    type->instanceSize = sizeof(EjsWebSession);
    mprAssert(type->objectBased);

    /*
     *  Re-define the helper functions.
     */
    type->helpers->getProperty = (EjsGetPropertyHelper) getSessionProperty;
    type->helpers->getPropertyByName = (EjsGetPropertyByNameHelper) getSessionPropertyByName;
    type->helpers->setProperty = (EjsSetPropertyHelper) setSessionProperty;
    type->helpers->destroyVar = (EjsDestroyVarHelper) destroySession;
}

#endif /* BLD_FEATURE_EJS_WEB */

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
