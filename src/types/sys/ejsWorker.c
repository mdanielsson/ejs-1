/*
 *  ejsWorker - VM Worker thread classes
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if BLD_FEATURE_MULTITHREAD
/*********************************** Locals ***********************************/

typedef struct Message {
    EjsWorker   *worker;
    cchar       *callback;
    char        *data;
    char        *message;
    char        *filename;
    char        *stack;
    int         lineNumber;
    int         callbackSlot;
} Message;

/*********************************** Forwards *********************************/

static void addWorker(Ejs *ejs, EjsWorker *worker);
static int join(Ejs *ejs, EjsVar *workers, int timeout);
static void handleError(Ejs *ejs, EjsWorker *worker, EjsVar *exception);
static void loadFile(EjsWorker *insideWorker, cchar *filename);
static void removeWorker(Ejs *ejs, EjsWorker *worker);
static void workerMain(EjsWorker *worker, MprWorker *mprWorker);
static EjsVar *workerPreload(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv);

/************************************ Methods *********************************/
/*
 *  function Worker(script: String = null, options: Object = null)
 *
 *  Script is optional. If supplied, the script is run immediately by a worker thread. This call
 *  does not block. Options are: search and name.
 */
static EjsVar *workerConstructor(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{
    Ejs             *wejs;
    EjsVar          *options, *value;
    EjsName         qname;
    EjsWorker       *self;
    EjsNamespace    *ns;
    cchar           *search, *name;

    worker->ejs = ejs;
    worker->state = EJS_WORKER_BEGIN;

    options = (argc == 2) ? (EjsVar*) argv[1]: NULL;
    name = 0;

    search = ejs->ejsPath;
    if (options) {
        value = ejsGetPropertyByName(ejs, options, ejsName(&qname, "", "search"));
        if (ejsIsString(value)) {
            search = ejsGetString(value);
        }
        value = ejsGetPropertyByName(ejs, options, ejsName(&qname, "", "name"));
        if (ejsIsString(value)) {
            name = ejsGetString(value);
        }
    }

    if (name) {
        worker->name = mprStrdup(worker, name);
    } else {
        worker->name = mprAsprintf(worker, -1, "worker-%d", mprGetListCount(ejs->workers));
    }

    /*
     *  Create a new interpreter and an "inside" worker object and pair it with the current "outside" worker.
     */
    wejs = ejsCreate(ejs->service, NULL, search, 0);
    if (wejs == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    worker->pair = self = ejsCreateWorker(wejs);
    self->state = EJS_WORKER_BEGIN;
    self->ejs = wejs;
    self->inside = 1;
    self->pair = worker;
    self->name = mprStrcat(self, -1, "inside-", worker->name, NULL);

    ejsSetProperty(ejs,  (EjsVar*) worker, ES_ejs_sys_Worker_name, (EjsVar*) ejsCreateString(ejs, self->name));
    ejsSetProperty(wejs, (EjsVar*) self,   ES_ejs_sys_Worker_name, (EjsVar*) ejsCreateString(wejs, self->name));
    ejsSetProperty(wejs, wejs->global, ES_ejs_sys_worker_self, (EjsVar*) self);

    /*
     *  Workers have a dedicated namespace to enable viewing of the worker globals (self, onmessage, postMessage...)
     */
    ns = ejsDefineReservedNamespace(wejs, wejs->globalBlock, 0, EJS_WORKER_NAMESPACE);

    /*
     *  Make the inside worker permanent so we don't need to worry about whether worker->pair->ejs is valid
     */
    self->obj.var.permanent = 1;
    
    if (argc > 0 && ejsIsPath(argv[0])) {
        addWorker(ejs, worker);
        worker->scriptFile = mprStrdup(worker, ((EjsPath*) argv[0])->path);
        worker->state = EJS_WORKER_STARTED;
        worker->obj.var.permanent = 1;
        if (mprStartWorker(ejs, (MprWorkerProc) workerMain, (void*) worker, MPR_NORMAL_PRIORITY) < 0) {
            ejsThrowStateError(ejs, "Can't start worker");
            worker->obj.var.permanent = 0;
            return 0;
        }
    }
    return (EjsVar*) worker;
}


/*
 *  Add a worker object to the list of workers for this interpreter
 */
static void addWorker(Ejs *ejs, EjsWorker *worker) 
{
    mprAssert(ejs == worker->ejs);
    mprAssert(worker);
    mprAssert(!worker->inside);
    mprAssert(worker->state == EJS_WORKER_BEGIN);

    lock(ejs);
    mprAddItem(ejs->workers, worker);
    unlock(ejs);
}


static void removeWorker(Ejs *ejs, EjsWorker *worker) 
{
    mprAssert(ejs == worker->ejs);
    mprAssert(!worker->inside);
    mprAssert(worker);

    lock(ejs);
    mprRemoveItem(ejs->workers, worker);
    if (ejs->joining) {
        mprWakeDispatcher(ejs->dispatcher);
    }
    unlock(ejs);
}


/*
 *  Start a worker thread. This is called by eval() and load(). Not by preload() or by Worker()
 *  It always joins.
 */
static EjsVar *startWorker(Ejs *ejs, EjsWorker *worker, int timeout)
{
    Ejs     *inside;
    EjsVar  *result;

    mprAssert(ejs);
    mprAssert(worker);
    mprAssert(worker->state == EJS_WORKER_BEGIN);

    if (worker->state > EJS_WORKER_BEGIN) {
        ejsThrowStateError(ejs, "Worker has already started");
        return 0;
    }

    mprAssert(worker->pair->state == EJS_WORKER_BEGIN);

    addWorker(ejs, worker);
    worker->state = EJS_WORKER_STARTED;

    worker->obj.var.permanent = 1;
    if (mprStartWorker(ejs, (MprWorkerProc) workerMain, (void*) worker, MPR_NORMAL_PRIORITY) < 0) {
        ejsThrowStateError(ejs, "Can't start worker");
        worker->obj.var.permanent = 0;
        return 0;
    }
    if (timeout == 0) {
        return ejs->undefinedValue;
    } 
    if (timeout < 0) {
        timeout = MAXINT;
    }
    if (join(ejs, (EjsVar*) worker, timeout) < 0) {
        ejsThrowStateError(ejs, "Timeout (%d)", timeout);
        return ejs->undefinedValue;
    }
    mprAssert(worker->pair);
    mprAssert(worker->pair->ejs);
    inside = worker->pair->ejs;
    result = ejsSerialize(ejs, inside->result, -1, 0, 0);
    if (result == 0) {
        return ejs->nullValue;
    }
    return ejsDeserialize(ejs, (EjsString*) result);
}


/*
 *  function eval(script: String, timeout: Boolean = -1): String
 */
static EjsVar *workerEval(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{
    int     timeout;

    mprAssert(ejsIsString(argv[0]));

    worker->scriptLiteral = mprStrdup(worker, ejsGetString(argv[0]));
    timeout = argc == 2 ? ejsGetInt(argv[1]): MAXINT;
    return startWorker(ejs, worker, timeout);
}


/*
 *  static function exit()
 */
static EjsVar *workerExit(Ejs *ejs, EjsWorker *unused, int argc, EjsVar **argv)
{
    ejs->exiting = 1;
    ejs->attention = 1;
    return 0;
}


static int reapJoins(Ejs *ejs, EjsVar *workers)
{
    EjsWorker   *worker;
    EjsArray    *set;
    int         i, joined, completed;

    joined = 0;

    lock(ejs);
    if (workers == 0 || workers == ejs->nullValue) {
        completed = 0;
        for (i = 0; i < mprGetListCount(ejs->workers); i++) {
            worker = mprGetItem(ejs->workers, i);
            if (worker->state >= EJS_WORKER_COMPLETE) {
                completed++;
            }
        }
        if (completed == mprGetListCount(ejs->workers)) {
            unlock(ejs);
            return 1;
        }
    } else if (ejsIsArray(workers)) {
        set = (EjsArray*) workers;
        for (i = 0; i < set->length; i++) {
            worker = (EjsWorker*) set->data[i];
            if (worker->state < EJS_WORKER_COMPLETE) {
                break;
            }
        }
        if (i >= set->length) {
            unlock(ejs);
            return 1;
        }
    } else if (workers->type == ejs->workerType) {
        worker = (EjsWorker*) workers;
        if (worker->state >= EJS_WORKER_COMPLETE) {
            unlock(ejs);
            return 1;
        }
    }
    unlock(ejs);
    return 0;
}


static int join(Ejs *ejs, EjsVar *workers, int timeout)
{
    MprTime     mark, remaining;
    int         result, count, total;

    mark = mprGetTime(ejs);
    ejs->joining = 1;

    do {
        /*
         *  Must process all pending messages
         */
        total = 0;
        while ((count = mprServiceEvents(ejs->dispatcher, 0, MPR_SERVICE_EVENTS)) > 0) { 
            total += count;
        }
        ejs->joining = !reapJoins(ejs, workers);
        if (total == 0 && ejs->joining) {
            mprWaitForCond(ejs->dispatcher->cond, timeout);
        }
        remaining = mprGetRemainingTime(ejs, mark, timeout);
    } while (ejs->joining && remaining > 0 && !ejs->exception);

    if (ejs->exception) {
        return 0;
    }
    result = (ejs->joining) ? MPR_ERR_TIMEOUT: 0;
    ejs->joining = 0;
    return result;
}


/*
 *  static function join(workers: Object = null, timeout: Number = -1): Boolean
 */
static EjsVar *workerJoin(Ejs *ejs, EjsWorker *unused, int argc, EjsVar **argv)
{
    EjsVar      *workers;
    int         timeout;

    workers = (argc > 0) ? argv[0] : NULL;
    timeout = (argc == 2) ? ejsGetInt(argv[1]) : MAXINT;

    return (join(ejs, workers, timeout) == 0) ? (EjsVar*) ejs->trueValue: (EjsVar*) ejs->falseValue;
}


/*
 *  Load a file into the worker. This can be a script file or a module. This runs on the inside interpreter
 */
static void loadFile(EjsWorker *worker, cchar *path)
{
    Ejs         *ejs;
    EjsVar      *result;
    cchar       *cp;

    mprAssert(worker->inside);
    mprAssert(worker->pair && worker->pair->ejs);

    ejs = worker->ejs;
    result = 0;

    if ((cp = strrchr(path, '.')) != NULL && strcmp(cp, EJS_MODULE_EXT) != 0) {
        if (ejs->service->loadScriptFile == 0) {
            ejsThrowIOError(ejs, "load: Compiling is not enabled for %s", path);
            return;
        }
        (ejs->service->loadScriptFile)(ejs, path);

    } else {
        /* This will throw on errors */
        ejsLoadModule(ejs, path, -1, -1, 0, NULL);
    }
}


/*
 *  function load(script: Path, timeout: Number = 0): Void
 */
static EjsVar *workerLoad(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{
    int     timeout;

    mprAssert(argc == 0 || ejsIsPath(argv[0]));

    worker->scriptFile = mprStrdup(worker, ((EjsPath*) argv[0])->path);
    timeout = argc == 2 ? ejsGetInt(argv[1]): 0;
    return startWorker(ejs, worker, timeout);
}


/*
 *  static function lookup(name: String): Worker
 */
static EjsVar *workerLookup(Ejs *ejs, EjsVar *unused, int argc, EjsVar **argv)
{
    EjsWorker   *worker;
    cchar       *name;
    int         next;

    name = ejsGetString(argv[0]);
    lock(ejs);
    for (next = 0; (worker = mprGetNextItem(ejs->workers, &next)) != NULL; ) {
        if (worker->name && strcmp(name, worker->name) == 0) {
            unlock(ejs);
            return (EjsVar*) worker;
        }
    }
    unlock(ejs);
    return ejs->nullValue;
}


/*
 *  Process a message sent from postMessage. This may run inside the worker or outside in the parent depending on the
 *  direction of the message. But it ALWAYS runs in the appropriate thread for the interpreter.
 */
static void doMessage(Message *msg, MprEvent *mprEvent)
{
    Ejs         *ejs;
    EjsVar      *event;
    EjsWorker   *worker;
    EjsFunction *callback;
    EjsVar      *argv[1];

    worker = msg->worker;
    ejs = worker->ejs;

    callback = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) worker, msg->callbackSlot);

    switch (msg->callbackSlot) {
    case ES_ejs_sys_Worker_onclose:
        event = ejsCreateVar(ejs, ejs->eventType, 0);
        break;
    case ES_ejs_sys_Worker_onerror:
        event = ejsCreateVar(ejs, ejs->errorEventType, 0);
        break;
    case ES_ejs_sys_Worker_onmessage:
        event = ejsCreateVar(ejs, ejs->eventType, 0);
        break;
    default:
        mprAssert(msg->callbackSlot == 0);
        mprFree(mprEvent);
        return;
    }
    if (msg->data) {
        ejsSetProperty(ejs, event, ES_ejs_events_Event_data, (EjsVar*) ejsCreateStringAndFree(ejs, msg->data));
    }
    if (msg->message) {
        ejsSetProperty(ejs, event, ES_ejs_events_ErrorEvent_message, (EjsVar*) ejsCreateStringAndFree(ejs, msg->message));
    }
    if (msg->filename) {
        ejsSetProperty(ejs, event, ES_ejs_events_ErrorEvent_filename, (EjsVar*) ejsCreateStringAndFree(ejs, msg->filename));
        ejsSetProperty(ejs, event, ES_ejs_events_ErrorEvent_lineno, (EjsVar*) ejsCreateNumber(ejs, msg->lineNumber));
    }
    if (msg->stack) {
        ejsSetProperty(ejs, event, ES_ejs_events_ErrorEvent_stack, (EjsVar*) ejsCreateStringAndFree(ejs, msg->stack));
    }

    if (callback == 0 || (EjsVar*) callback == ejs->nullValue) {
        if (msg->callbackSlot == ES_ejs_sys_Worker_onmessage) {
            mprLog(ejs, 1, "Discard message as no onmessage handler defined for worker");
            
        } else if (msg->callbackSlot == ES_ejs_sys_Worker_onerror) {
            ejsThrowError(ejs, "Exception in Worker: %s", ejsGetErrorMsg(worker->pair->ejs, 1));

        } else {
            /* Ignore onclose message */
        }

    } else if (!ejsIsFunction(callback)) {
        ejsThrowTypeError(ejs, "Worker callback %s is not a function", msg->callback);

    } else {
        argv[0] = event;
        ejsRunFunction(ejs, callback, (EjsVar*) worker, 1, argv);
    }

    if (msg->callbackSlot == ES_ejs_sys_Worker_onclose) {
        mprAssert(!worker->inside);
        worker->state = EJS_WORKER_COMPLETE;
        removeWorker(ejs, worker);
        /*
         *  Now that the inside worker is complete, the outside worker does not need to be protected from GC
         */
        worker->obj.var.permanent = 0;
    }
    mprFree(msg);
    mprFree(mprEvent);
}


/*
 *  function preload(path: Path): String
 *  NOTE: this blocks. 
 */
static EjsVar *workerPreload(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{
    Ejs         *inside;
    EjsWorker   *insideWorker;
    EjsVar      *result;

    mprAssert(argc > 0 && ejsIsPath(argv[0]));
    mprAssert(!worker->inside);

    if (worker->state > EJS_WORKER_BEGIN) {
        ejsThrowStateError(ejs, "Worker has already started");
        return 0;
    }
    insideWorker = worker->pair;
    inside = insideWorker->ejs;

    loadFile(worker->pair, ((EjsPath*) argv[0])->path);
    if (inside->exception) {
        handleError(ejs, worker, inside->exception);
        return 0;
    }
    result = ejsSerialize(ejs, inside->result, -1, 0, 0);
    if (result == 0) {
        return ejs->nullValue;
    }
    return ejsDeserialize(ejs, (EjsString*) result);
}


/*
 *  Post a message to this worker. Note: the worker is the destination worker which may be the parent.
 *
 *  function postMessage(data: Object, ports: Array = null): Void
 */
static EjsVar *workerPostMessage(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{
    EjsVar          *data;
    EjsWorker       *target;
    MprDispatcher   *dispatcher;
    Message         *msg;

    if (worker->state >= EJS_WORKER_CLOSED) {
        ejsThrowStateError(ejs, "Worker has completed");
        return 0;
    }

    /*
     *  Create the event with serialized data in the originating interpreter. It owns the data.
     */
    if ((data = ejsSerialize(ejs, argv[0], -1, 0, 0)) == 0) {
        ejsThrowArgError(ejs, "Can't serialize message data");
        return 0;
    }
    if ((msg = mprAllocObjZeroed(ejs, Message)) == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    target = worker->pair;
    msg->data = mprStrdup(target->ejs, ejsGetString(data));
    msg->worker = target;
    msg->callback = "onmessage";
    msg->callbackSlot = ES_ejs_sys_Worker_onmessage;

    dispatcher = target->ejs->dispatcher;
    mprCreateEvent(dispatcher, (MprEventProc) doMessage, 0, MPR_NORMAL_PRIORITY, msg, 0);
    mprSignalCond(dispatcher->cond);
    return 0;
}


/*
 *  Worker thread main procedure
 */
static void workerMain(EjsWorker *worker, MprWorker *mprWorker)
{
    Ejs             *ejs, *inside;
    EjsWorker       *insideWorker;
    MprDispatcher   *dispatcher;
    Message         *msg;

    mprAssert(!worker->inside);
    insideWorker = worker->pair;
    mprAssert(insideWorker->state == EJS_WORKER_BEGIN);

    ejs = worker->ejs;
    inside = insideWorker->ejs;
    insideWorker->state = EJS_WORKER_STARTED;
    
    /*
     *  Run the script or file
     */
    if (worker->scriptFile) {
        loadFile(insideWorker, worker->scriptFile);

    } else if (worker->scriptLiteral) {
        if (ejs->service->loadScriptLiteral == 0) {
            ejsThrowIOError(ejs, "worker: Compiling is not enabled");
            return;
        }
        (ejs->service->loadScriptLiteral)(inside, worker->scriptLiteral);
    }

    /*
     *  Check for exceptions
     */
    if (inside->exception) {
        handleError(ejs, worker, inside->exception);
    }
    if ((msg = mprAllocObjZeroed(ejs, Message)) == 0) {
        ejsThrowMemoryError(ejs);
        return;
    }

    /*
     *  Post "onclose" finalization message
     */
    msg->worker = worker;
    msg->callback = "onclose";
    msg->callbackSlot = ES_ejs_sys_Worker_onclose;

    insideWorker->state = EJS_WORKER_CLOSED;
    worker->state = EJS_WORKER_CLOSED;
    insideWorker->obj.var.permanent = 0;
    dispatcher = worker->ejs->dispatcher;
    mprCreateEvent(dispatcher, (MprEventProc) doMessage, 0, MPR_NORMAL_PRIORITY, msg, 0);
    mprSignalCond(dispatcher->cond);
}


/*
 *  function terminate()
 */
static EjsVar *workerTerminate(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{    
    if (worker->state == EJS_WORKER_BEGIN) {
        ejsThrowStateError(ejs, "Worker has not yet started");
        return 0;
    }
    if (worker->state >= EJS_WORKER_COMPLETE) {
        return 0;
    }
  
    /*
     *  Switch to the inside worker if called from outside
     */
    mprAssert(worker->pair && worker->pair->ejs);
    ejs = (!worker->inside) ? worker->pair->ejs : ejs;
    worker->terminated = 1;
    ejs->exiting = 1;
    mprWakeDispatcher(ejs->dispatcher);
    return 0;
}


/*
 *  function waitForMessage(timeout: Number = -1): Boolean
 */
static EjsVar *workerWaitForMessage(Ejs *ejs, EjsWorker *worker, int argc, EjsVar **argv)
{
    MprTime     mark, remaining;
    int         timeout;

    timeout = (argc > 0) ? ejsGetInt(argv[0]): MAXINT;
    if (timeout < 0) {
        timeout = MAXINT;
    }
    mark = mprGetTime(ejs);
    do {
        if (mprServiceEvents(ejs->dispatcher, timeout, MPR_SERVICE_EVENTS | MPR_SERVICE_ONE_THING) > 0) {
            return (EjsVar*) ejs->trueValue;
        }
        remaining = mprGetRemainingTime(ejs, mark, timeout);
    } while (remaining > 0 && !mprIsExiting(ejs) && !ejs->exiting);
    return (EjsVar*) ejs->falseValue;
}


/*
 *  WARNING: the inside interpreter owns the exception object. Must fully extract all fields
 */
static void handleError(Ejs *ejs, EjsWorker *worker, EjsVar *exception)
{
    EjsError        *error;
    MprDispatcher   *dispatcher;
    Message         *msg;

    mprAssert(!worker->inside);
    mprAssert(exception);
    mprAssert(ejs == worker->ejs);

    if ((msg = mprAllocObjZeroed(ejs, Message)) == 0) {
        ejsThrowMemoryError(ejs);
        return;
    }
    msg->worker = worker;
    msg->callback = "onerror";
    msg->callbackSlot = ES_ejs_sys_Worker_onerror;
    
    /*
     *  Inside interpreter owns the exception object, so must fully extract all exception. 
     *  Allocate into the outside worker's interpreter.
     */
    if (ejsIsError(exception)) {
        error = (EjsError*) exception;
        msg->message = mprStrdup(ejs, error->message);
        msg->filename = mprStrdup(ejs, error->filename ? error->filename : "script");
        msg->lineNumber = error->lineNumber;
        msg->stack = mprStrdup(ejs, error->stack);

    } else if (ejsIsString(exception)) {
        msg->message = mprStrdup(ejs, ejsGetString(exception));

    } else {
        msg->message = mprStrdup(ejs, ejsGetString(ejsToString(ejs, exception)));
    }
    dispatcher = ejs->dispatcher;
    mprCreateEvent(dispatcher, (MprEventProc) doMessage, 0, MPR_NORMAL_PRIORITY, msg, 0);
    mprSignalCond(dispatcher->cond);
}


EjsWorker *ejsCreateWorker(Ejs *ejs)
{
    return (EjsWorker*) ejsCreateVar(ejs, ejs->workerType, 0);
}


static void destroyWorker(Ejs *ejs, EjsWorker *worker)
{
    if (!worker->inside) {
        removeWorker(ejs, worker);
        mprAssert(worker->pair);
        mprFree(worker->pair->ejs);
        worker->pair = 0;
    }
    ejsFreeVar(ejs, (EjsVar*) worker, -1);
}


static void markWorker(Ejs *ejs, EjsVar *parent, EjsWorker *worker)
{
    ejsMarkObject(ejs, parent, (EjsObject*) worker);
}


void ejsConfigureWorkerType(Ejs *ejs)
{
    EjsType     *type;
    EjsName     qname;

    type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.sys", "Worker"));
    if (type) {
        type->instanceSize = sizeof(EjsWorker);
        type->dontPool = 1;
        type->needFinalize = 1;
        type->helpers->destroyVar = (EjsDestroyVarHelper) destroyWorker;
        type->helpers->markVar = (EjsMarkVarHelper) markWorker;
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_Worker, (EjsNativeFunction) workerConstructor);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_eval, (EjsNativeFunction) workerEval);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_exit, (EjsNativeFunction) workerExit);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_join, (EjsNativeFunction) workerJoin);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_load, (EjsNativeFunction) workerLoad);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_lookup, (EjsNativeFunction) workerLookup);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_preload, (EjsNativeFunction) workerPreload);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_postMessage, (EjsNativeFunction) workerPostMessage);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_terminate, (EjsNativeFunction) workerTerminate);
        ejsBindMethod(ejs, type, ES_ejs_sys_Worker_waitForMessage, (EjsNativeFunction) workerWaitForMessage);
        ejs->workerType = type;
    }
}
#endif /* BLD_FEATURE_MULTITHREAD */

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

