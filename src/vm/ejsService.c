/**
 *  ejsService.c - Ejscript interpreter factory
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if BLD_FEATURE_EJS_WEB
#include    "ejsWeb.h"
#endif

/*********************************** Forward **********************************/

static void allocNotifier(Ejs *ejs, uint size, uint total, bool granted);
static int  cloneMaster(Ejs *ejs, Ejs *master);
static int  configureEjsModule(Ejs *ejs, EjsModule *mp, cchar *path);
static int  defineTypes(Ejs *ejs);
static void defineHelpers(Ejs *ejs);
static int  destroyEjs(Ejs *ejs);
static int  runSpecificMethod(Ejs *ejs, cchar *className, cchar *methodName);
static int  searchForMethod(Ejs *ejs, cchar *methodName, EjsType **typeReturn);
static void setDefaultSearchPath(Ejs *ejs);

#if BLD_FEATURE_STATIC || BLD_FEATURE_EJS_ALL_IN_ONE
#if BLD_FEATURE_MYSQL
 #include "ejs.db.mysqlite.slots.h"
static int configureMysqlModule(Ejs *ejs, EjsModule *mp, cchar *path);
#endif
#if BLD_FEATURE_SQLITE
 #include "ejs.db.sqlite.slots.h"
static int configureSqliteModule(Ejs *ejs, EjsModule *mp, cchar *path);
#endif
#if BLD_FEATURE_EJS_WEB
static int configureWebModule(Ejs *ejs, EjsModule *mp, cchar *path);
#endif
#endif

/*
 *  Global singleton for the Ejs service
 */
EjsService *_globalEjsService;

/************************************* Code ***********************************/
/*
 *  Initialize the EJS subsystem
 */
EjsService *ejsCreateService(MprCtx ctx)
{
    EjsService  *sp;

    sp = mprAllocObjZeroed(ctx, EjsService);
    if (sp == 0) {
        return 0;
    }
    _globalEjsService = sp;
    sp->nativeModules = mprCreateHash(sp, 0);

    /*
     *  The native module callbacks are invoked after loading the module files. This allows the callback routines 
     *  to configure native methods and do other native type adjustments. configureEjsModule is always invoked even 
     *  if SHARED because it is never loaded from a shared library. Normally, when loading from a shared library, 
     *  the init routine is invoked immediately after loading the mod file and it should call the configuration routine.
     */
    ejsAddNativeModule(ctx, "ejs", configureEjsModule);

#if BLD_FEATURE_STATIC || BLD_FEATURE_EJS_ALL_IN_ONE
#if BLD_FEATURE_SQLITE
    ejsAddNativeModule(ctx, "ejs.db.sqlite", configureSqliteModule);
#endif
#if BLD_FEATURE_EJS_WEB
    ejsAddNativeModule(ctx, "ejs.web", configureWebModule);
#endif
#endif
    return sp;
}


Ejs *ejsCreate(MprCtx ctx, Ejs *master, cchar *searchPath, int flags)
{
    Ejs     *ejs;
    char    *env;

    /*
     *  Create interpreter structure
     */
    ejs = mprAllocObjWithDestructorZeroed(ctx, Ejs, destroyEjs);
    if (ejs == 0) {
        return 0;
    }
    mprSetAllocNotifier(ejs, (MprAllocNotifier) allocNotifier);
    ejs->service = _globalEjsService;

    /*
     *  Probably not necessary, but it keeps the objects in one place
     */
    ejs->heap = mprAllocHeap(ejs, "Ejs Object Heap", 1, 0, NULL);

    /*
     *  Flags may include COMPILER to pretend to be compiling. Ejsmod uses this to allow compiler-style access to
     *  getters and setters.
     */
    ejs->flags |= (flags & (EJS_FLAG_EMPTY | EJS_FLAG_COMPILER | EJS_FLAG_NO_EXE | EJS_FLAG_DOC));
    ejs->dispatcher = mprCreateDispatcher(ejs);

    if (ejsInitStack(ejs) < 0) {
        mprFree(ejs);
        return 0;
    }
    ejsCreateGCService(ejs);
    
    /*
     *  The search path consists of:
     *      searchPath : EJSPATH : APP_EXE_DIR : MOD_DIR : .
     */
    setDefaultSearchPath(ejs);
    env = getenv("EJSPATH");
    if (env && *env) {
        ejsPrependSearchPath(ejs, env);
    }
    if (searchPath) {
        ejsPrependSearchPath(ejs, searchPath);
    }
    ejsSetGeneration(ejs, EJS_GEN_ETERNAL);

    if (master == 0) {
        ejs->modules = mprCreateList(ejs);
        ejs->workers = mprCreateList(ejs);
        ejs->coreTypes = mprCreateHash(ejs, 0);
        ejs->standardSpaces = mprCreateHash(ejs, 0);

        defineHelpers(ejs);
        if (defineTypes(ejs) < 0) {
            mprFree(ejs);
            return 0;
        }
    } else {
        cloneMaster(ejs, master);
    }

#if BLD_FEATURE_MULTITHREAD
    ejs->mutex = mprCreateLock(ejs);
#endif

    if (mprHasAllocError(ejs)) {
        mprError(ejs, "Memory allocation error during initialization");
        mprFree(ejs);
        return 0;
    }
    ejs->initialized = 1;
    ejsCollectGarbage(ejs, EJS_GEN_ETERNAL);
    ejsSetGeneration(ejs, EJS_GEN_NEW);
    return ejs;
}


static int destroyEjs(Ejs *ejs)
{
    EjsState    *state;

    ejsDestroyGCService(ejs);
    state = ejs->masterState;
    if (state->stackBase) {
        mprMapFree(state->stackBase, state->stackSize);
    }
    mprFree(ejs->heap);
    return 0;
}


static void defineHelpers(Ejs *ejs)
{
    ejs->defaultHelpers = (EjsTypeHelpers*) mprAllocZeroed(ejs, sizeof(EjsTypeHelpers));
    ejsInitializeDefaultHelpers(ejs->defaultHelpers);

    /*
     *  Object inherits the default helpers. Block inherits the object helpers
     */
    ejs->objectHelpers = (EjsTypeHelpers*) mprMemdup(ejs, (void*) ejs->defaultHelpers, sizeof(EjsTypeHelpers));
    ejsInitializeObjectHelpers(ejs->objectHelpers);

    ejs->blockHelpers = (EjsTypeHelpers*) mprMemdup(ejs, (void*) ejs->objectHelpers, sizeof(EjsTypeHelpers));
    ejsInitializeBlockHelpers(ejs->blockHelpers);
}


/*
 *  Create the core language types. These are native types and are created prior to loading ejs.mod.
 *  The loader then matches these types to the loaded definitions.
 */
static int createTypes(Ejs *ejs)
{
    /*
     *  Create the essential bootstrap types: Object, Type and the global object, these are the foundation.
     *  All types are instances of Type. Order matters here.
     */
    ejsCreateObjectType(ejs);
    ejsCreateTypeType(ejs);
    ejsCreateBlockType(ejs);
    ejsCreateNamespaceType(ejs);
    ejsCreateFunctionType(ejs);
    ejsCreateGlobalBlock(ejs);
    ejsCreateNullType(ejs);

    /*
     *  Now create the rest of the native types.
     */
    ejsCreateArrayType(ejs);
    ejsCreateBooleanType(ejs);
    ejsCreateByteArrayType(ejs);
    ejsCreateDateType(ejs);
    ejsCreateErrorType(ejs);
    ejsCreateIteratorType(ejs);
    ejsCreateVoidType(ejs);
    ejsCreateNumberType(ejs);
    ejsCreateReflectType(ejs);
    ejsCreateStringType(ejs);
#if ES_XML && BLD_FEATURE_EJS_E4X
    ejsCreateXMLType(ejs);
    ejsCreateXMLListType(ejs);
#endif
#if ES_RegExp && BLD_FEATURE_REGEXP
    ejsCreateRegExpType(ejs);
#endif
    ejsCreateAppType(ejs);
    ejsCreateConfigType(ejs);
    ejsCreateGCType(ejs);
    ejsCreateMemoryType(ejs);
    ejsCreateSystemType(ejs);
    ejsCreateTimerType(ejs);
    ejsCreateFileType(ejs);
    ejsCreatePathType(ejs);
    ejsCreateFileSystemType(ejs);
#if ES_ejs_io_Http && BLD_FEATURE_HTTP_CLIENT
    ejsCreateHttpType(ejs);
#endif

    if (ejs->hasError || ejs->errorType == 0 || mprHasAllocError(ejs)) {
        return MPR_ERR;
    }
    return 0;
}


/*
 *  This will configure all the core types by defining native methods and properties
 */
static int configureEjsModule(Ejs *ejs, EjsModule *mp, cchar *path)
{
    EjsModule   *pp;
    
    if (ejs->flags & EJS_FLAG_EMPTY) {
        return 0;
    }
    if (mp->checksum != _ES_CHECKSUM_ejs) {
        ejsThrowIOError(ejs, "Module \"%s\" does not match native code", path);
        return EJS_ERR;
    }

    /*
     *  Order matters
     */
    ejsConfigureObjectType(ejs);
    ejsConfigureArrayType(ejs);
    ejsConfigureBlockType(ejs);
    ejsConfigureBooleanType(ejs);
    ejsConfigureByteArrayType(ejs);
    ejsConfigureDateType(ejs);
    ejsConfigureFunctionType(ejs);
    ejsConfigureGlobalBlock(ejs);
    ejsConfigureErrorType(ejs);
    ejsConfigureIteratorType(ejs);
    ejsConfigureMathType(ejs);
    ejsConfigureNamespaceType(ejs);
    ejsConfigureVoidType(ejs);
    ejsConfigureNumberType(ejs);
    ejsConfigureNullType(ejs);
    ejsConfigureReflectType(ejs);
    ejsConfigureStringType(ejs);
    ejsConfigureTypeType(ejs);
#if ES_XML && BLD_FEATURE_EJS_E4X
    ejsConfigureXMLType(ejs);
    ejsConfigureXMLListType(ejs);
#endif
#if ES_RegExp && BLD_FEATURE_REGEXP
    ejsConfigureRegExpType(ejs);
#endif

    ejsConfigureAppType(ejs);
    ejsConfigureConfigType(ejs);
#if BLD_FEATURE_MULTITHREAD
    ejsConfigureWorkerType(ejs);
#endif
    ejsConfigureGCType(ejs);
    ejsConfigureMemoryType(ejs);
    ejsConfigureSystemType(ejs);
    ejsConfigureTimerType(ejs);
    ejsConfigurePathType(ejs);
    ejsConfigureFileType(ejs);
    ejsConfigureFileSystemType(ejs);
#if ES_ejs_io_Http && BLD_FEATURE_HTTP_CLIENT
    ejsConfigureHttpType(ejs);
#endif

    if (ejs->hasError || ejs->errorType == 0 || mprHasAllocError(ejs)) {
        mprAssert(0);
        return MPR_ERR;
    }
    
    if ((pp = ejsLookupModule(ejs, "ejs.events", -1, -1)) != 0) {
        pp->configured = 1;
    }
    if ((pp = ejsLookupModule(ejs, "ejs.sys", -1, -1)) != 0) {
        pp->configured = 1;
    }
    if ((pp = ejsLookupModule(ejs, "ejs.io", -1, -1)) != 0) {
        pp->configured = 1;
    }

    /*
     *  This makes all essential values that have no references (like ejs->zeroValue) permanent
     */
    ejsMakeEternalPermanent(ejs);
    return 0;
}


#if BLD_FEATURE_STATIC || BLD_FEATURE_EJS_ALL_IN_ONE
#if BLD_FEATURE_MYSQL
static int configureMysqlModule(Ejs *ejs, EjsModule *mp, cchar *path)
{
    ejsConfigureMysqlTypes(ejs);
    if (ejs->hasError || ejs->errorType == 0 || mprHasAllocError(ejs)) {
        mprAssert(0);
        return MPR_ERR;
    }
    mp->configured = 1;
    return 0;
}
#endif


#if BLD_FEATURE_SQLITE
static int configureSqliteModule(Ejs *ejs, EjsModule *mp, cchar *path)
{
    if (ejs->flags & EJS_FLAG_EMPTY) {
        return 0;
    }
    ejsConfigureSqliteTypes(ejs);
    if (ejs->hasError || ejs->errorType == 0 || mprHasAllocError(ejs)) {
        mprAssert(0);
        return MPR_ERR;
    }
    if (mp->checksum != _ES_CHECKSUM_ejs_db_sqlite) {
        ejsThrowIOError(ejs, "Module \"%s\" does not match native code", path);
        return EJS_ERR;
    }
    mp->configured = 1;
    return 0;
}
#endif


#if BLD_FEATURE_EJS_WEB
static int configureWebModule(Ejs *ejs, EjsModule *mp, cchar *path)
{
    if (ejs->flags & EJS_FLAG_EMPTY) {
        return 0;
    }
    ejsConfigureWebTypes(ejs);

    if (ejs->hasError || ejs->errorType == 0 || mprHasAllocError(ejs)) {
        mprAssert(0);
        return MPR_ERR;
    }
    if (mp->checksum != _ES_CHECKSUM_ejs_web) {
        ejsThrowIOError(ejs, "Module \"%s\" does not match native code", path);
        return EJS_ERR;
    }
    mp->configured = 1;
    return 0;
}
#endif
#endif


/*
 *  Register a native module callback to be invoked when it it time to configure the module. This is used by loadable modules
 *  when they are built statically.
 */
int ejsAddNativeModule(MprCtx ctx, char *name, EjsNativeCallback callback)
{
    if (mprAddHash(_globalEjsService->nativeModules, name, callback) == 0) {
        return EJS_ERR;
    }
    return 0;
}


static int defineTypes(Ejs *ejs)
{
    /*
     *  Create all the builtin types. These are defined and hashed. Not defined in global.
     */
    if (createTypes(ejs) < 0 || ejs->hasError) {
        mprError(ejs, "Can't create core types");
        return EJS_ERR;
    }

    /*
     *  Load the builtin module. This will create all the type definitions and match with builtin native types.
     *  This will call the configure routines defined in moduleConfig and will run the module initializers.
     *  Modules are check summed and configureEjsModule will check that the sum matches.
     */
    if (! (ejs->flags & EJS_FLAG_EMPTY)) {
        if (ejsLoadModule(ejs, EJS_MOD, 0, 0, EJS_MODULE_BUILTIN, NULL) < 0) {
            mprError(ejs, "Can't load " EJS_MOD);
            return EJS_ERR;
        }
    }
    return 0;
}


/*
 *  When cloning the master interpreter, the new interpreter references the master's core types. The core types MUST
 *  be immutable for this to work.
 */
static int cloneMaster(Ejs *ejs, Ejs *master)
{
    EjsName     qname;
    EjsType     *type;
    EjsVar      *vp;
    EjsTrait    *trait;
    int         i, count;

    mprAssert(master);

    ejs->master = master;
    ejs->service = master->service;
    ejs->defaultHelpers = master->defaultHelpers;
    ejs->objectHelpers = master->objectHelpers;
    ejs->blockHelpers = master->blockHelpers;
    ejs->objectType = master->objectType;

    ejs->arrayType = master->arrayType;
    ejs->blockType = master->blockType;
    ejs->booleanType = master->booleanType;
    ejs->byteArrayType = master->byteArrayType;
    ejs->dateType = master->dateType;
    ejs->errorType = master->errorType;
    ejs->eventType = master->eventType;
    ejs->errorEventType = master->errorEventType;
    ejs->functionType = master->functionType;
    ejs->iteratorType = master->iteratorType;
    ejs->namespaceType = master->namespaceType;
    ejs->nullType = master->nullType;
    ejs->numberType = master->numberType;
    ejs->objectType = master->objectType;
    ejs->regExpType = master->regExpType;
    ejs->stringType = master->stringType;
    ejs->stopIterationType = master->stopIterationType;
    ejs->typeType = master->typeType;
    ejs->voidType = master->voidType;
    ejs->workerType = master->workerType;

#if BLD_FEATURE_EJS_E4X
    ejs->xmlType = master->xmlType;
    ejs->xmlListType = master->xmlListType;
#endif

    ejs->emptyStringValue = master->emptyStringValue;
    ejs->falseValue = master->falseValue;
    ejs->infinityValue = master->infinityValue;
    ejs->minusOneValue = master->minusOneValue;
    ejs->nanValue = master->nanValue;
    ejs->negativeInfinityValue = master->negativeInfinityValue;
    ejs->nullValue = master->nullValue;
    ejs->oneValue = master->oneValue;
    ejs->trueValue = master->trueValue;
    ejs->undefinedValue = master->undefinedValue;
    ejs->zeroValue = master->zeroValue;

    ejs->configSpace = master->configSpace;
    ejs->emptySpace = master->emptySpace;
    ejs->eventsSpace = master->eventsSpace;
    ejs->ioSpace = master->ioSpace;
    ejs->intrinsicSpace = master->intrinsicSpace;
    ejs->iteratorSpace = master->iteratorSpace;
    ejs->internalSpace = master->internalSpace;
    ejs->publicSpace = master->publicSpace;
    ejs->sysSpace = master->sysSpace;

    ejs->argv = master->argv;
    ejs->argc = master->argc;
    ejs->coreTypes = master->coreTypes;
    ejs->standardSpaces = master->standardSpaces;

    ejs->modules = mprDupList(ejs, master->modules);
    ejs->sqlite = master->sqlite;

    //  Push this code into ejsGlobal.c. Call ejsCloneGlobal
    ejs->globalBlock = ejsCreateBlock(ejs, master->globalBlock->obj.capacity);
    ejs->global = (EjsVar*) ejs->globalBlock; 
    ejsSetDebugName(ejs->global, "global");
    ejs->globalBlock->obj.numProp = master->globalBlock->obj.numProp;
    ejsGrowBlock(ejs, ejs->globalBlock, ejs->globalBlock->obj.numProp);
    
    ejsCopyList(ejs->globalBlock, &ejs->globalBlock->namespaces, &master->globalBlock->namespaces);

    count = ejsGetPropertyCount(master, master->global);
    // count = ES_global_NUM_CLASS_PROP;
    for (i = 0; i < count; i++) {
        vp = ejsGetProperty(ejs, master->global, i);
        if (vp) {
            ejsSetProperty(ejs, ejs->global, i, ejsGetProperty(master, master->global, i));
            qname = ejsGetPropertyName(master, master->global, i);
            ejsSetPropertyName(ejs, ejs->global, i, &qname);
            trait = ejsGetTrait(master->globalBlock, i);
            if (trait) {
                ejsSetTrait(ejs->globalBlock, i, trait->type, trait->attributes);
            }
        }
    }
    
    /*
     *  Clone some mutable types.
     */
    type = (EjsType*) ejsGetProperty(ejs, ejs->global, ES_XML);
    ejsSetProperty(ejs, ejs->global, ES_XML, ejsCloneVar(ejs, (EjsVar*) type, 0));

#if ES_ejs_db_Database
    type = (EjsType*) ejsGetProperty(ejs, ejs->global, ES_ejs_db_Database); 
    if (type) {
        ejsSetProperty(ejs, ejs->global, ES_ejs_db_Database, ejsCloneVar(ejs, (EjsVar*) type, 0));
    }
#else
    /*
     *  Building shared. The web framework preloads these modules before cloning interpreters.
     *  The ejs command will not have them defined.
     */
    type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.db", "Database"));
    if (type) {
        ejsSetPropertyByName(ejs, ejs->global, &qname, ejsCloneVar(ejs, (EjsVar*) type, 0));
    }
#endif

#if ES_ejs_web_GoogleConnector
    type = (EjsType*) ejsGetProperty(ejs, ejs->global, ES_ejs_web_GoogleConnector); 
    if (type) {
        ejsSetProperty(ejs, ejs->global, ES_ejs_web_GoogleConnector, ejsCloneVar(ejs, (EjsVar*) type, 0));
    }
#else
    /*
     *  Shared.
     */
    type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.web", "GoogleConnector")); 
    if (type) {
        ejsSetPropertyByName(ejs, ejs->global, &qname, ejsCloneVar(ejs, (EjsVar*) type, 0));
    }
#endif
    ejsSetProperty(ejs, ejs->global, ES_global, ejs->global);
    return 0;
}


/*
 *  Notifier callback function. Invoked by mprAlloc on allocation errors. This will prevent the allocation error
 *  bubbling up to the global memory failure handler.
 */
static void allocNotifier(Ejs *ejs, uint size, uint total, bool granted)
{
    MprAlloc    *alloc;
    EjsVar      *argv[2], *thisObj;
	va_list     dummy = VA_NULL;
    char        msg[MPR_MAX_STRING];

    if (!ejs->exception) {
        ejs->attention = 1;
        alloc = mprGetAllocStats(ejs);
        if (granted) {
            if (ejs->memoryCallback) {
                argv[0] = (EjsVar*) ejsCreateNumber(ejs, size);
                argv[1] = (EjsVar*) ejsCreateNumber(ejs, total);
                thisObj = ejs->memoryCallback->thisObj ? ejs->memoryCallback->thisObj : ejs->global; 
                ejsRunFunction(ejs, ejs->memoryCallback, thisObj, 2, argv);
            } else {
                /* Static to avoid a memory allocation */
                mprSprintf(msg, sizeof(msg), "Low memory condition. Total mem: %d. Request for %d bytes granted.", 
                    total, size);
                ejsCreateException(ejs, ES_MemoryError, msg, dummy);
            }

        } else {
            mprSprintf(msg, sizeof(msg), "Memory depleted. Total mem: %d. Request for %d bytes denied.", total, size);
            ejsCreateException(ejs, ES_MemoryError, msg, dummy);
        }
    }
}


/*
 *  Set the module search path
 */
void ejsSetSearchPath(Ejs *ejs, cchar *searchPath)
{
    mprAssert(ejs);
    mprAssert(searchPath && searchPath);

    setDefaultSearchPath(ejs);
    ejsPrependSearchPath(ejs, searchPath);

    mprLog(ejs, 4, "ejs: Set search path to %s", ejs->ejsPath);
}


/*
 *  Prepend a search path to the system defaults
 */
void ejsPrependSearchPath(Ejs *ejs, cchar *searchPath)
{
    char    *oldPath;

    mprAssert(ejs);
    mprAssert(searchPath && searchPath);
    mprAssert(ejs->ejsPath);

    oldPath = ejs->ejsPath;
    mprAssert(oldPath);
    ejs->ejsPath = mprAsprintf(ejs, -1, "%s" MPR_SEARCH_SEP "%s", searchPath, oldPath);
    mprFree(oldPath);
    mprLog(ejs, 3, "ejs: set search path to %s", ejs->ejsPath);
}


/*
 *  Create a default module search path. This is set to:
 *      APP_EXE_DIR : MOD_DIR : .
 *  Where MOD_DIR is either the installed application modules directory or the local source modules directory.
 */
static void setDefaultSearchPath(Ejs *ejs)
{
    char    *search;

/*
 *  Always use the same directory as the app executable. This is sufficient for windows. For linux and if 
 *  running installed, use BLD_MOD_PREFIX. If running locally, BLD_ABS_MOD_DIR.
 */
#if VXWORKS
    search = mprGetCurrentPath(ejs);
#elif WIN
    search = mprAsprintf(ejs, -1, "%s" MPR_SEARCH_SEP ".", mprGetAppDir(ejs));
#else
    search = mprAsprintf(ejs, -1, "%s" MPR_SEARCH_SEP "%s" MPR_SEARCH_SEP ".", mprGetAppDir(ejs),
        mprSamePath(ejs, BLD_BIN_PREFIX, mprGetAppDir(ejs)) ? BLD_MOD_PREFIX: BLD_ABS_MOD_DIR);
#endif
    mprFree(ejs->ejsPath);
    ejs->ejsPath = search;
    mprLog(ejs, 4, "ejs: set default search path to %s", ejs->ejsPath);
}


EjsVar *ejsGetGlobalObject(Ejs *ejs)
{
    return (EjsVar*) ejs->global;
}


void ejsSetHandle(Ejs *ejs, void *handle)
{
    ejs->handle = handle;
}


void *ejsGetHandle(Ejs *ejs)
{
    return ejs->handle;
}


/*
 *  Evaluate a module file
 */
int ejsEvalModule(cchar *path)
{
    EjsService      *vm;   
    Ejs             *ejs;
    Mpr             *mpr;

    mpr = mprCreate(0, NULL, NULL);
    if ((vm = ejsCreateService(mpr)) == 0) {
        mprFree(mpr);
        return MPR_ERR_NO_MEMORY;
    }
    if ((ejs = ejsCreate(vm, NULL, NULL, 0)) == 0) {
        mprFree(mpr);
        return MPR_ERR_NO_MEMORY;
    }
    if (ejsLoadModule(ejs, path, -1, -1, 0, NULL) < 0) {
        mprFree(mpr);
        return MPR_ERR_CANT_READ;
    }
    if (ejsRun(ejs) < 0) {
        mprFree(mpr);
        return EJS_ERR;
    }
    mprFree(mpr);
    return 0;
}


/*
 *  Run a program. This will run a program assuming that all the required modules are already loaded. It will
 *  optionally service events until instructed to exit.
 */
int ejsRunProgram(Ejs *ejs, cchar *className, cchar *methodName)
{
    /*
     *  Run all module initialization code. This includes plain old scripts.
     */
    if (ejsRun(ejs) < 0) {
        return EJS_ERR;
    }
    /*
     *  Run the requested method. This will block until completion
     */
    if (className || methodName) {
        if (runSpecificMethod(ejs, className, methodName) < 0) {
            return EJS_ERR;
        }
    }
    if (ejs->flags & EJS_FLAG_NOEXIT) {
        /*
         *  This will service events until App.exit() is called
         */
        mprServiceEvents(ejs->dispatcher, -1, MPR_SERVICE_EVENTS);
    }
    return 0;
}


/*
 *  Run the specified method in the named class. If methodName is null, default to "main".
 *  If className is null, search for the first class containing the method name.
 */
static int runSpecificMethod(Ejs *ejs, cchar *className, cchar *methodName)
{
    EjsType         *type;
    EjsFunction     *fun;
    EjsName         qname;
    EjsVar          *args;
    int             attributes, i;

    type = 0;
    if (className == 0 && methodName == 0) {
        return 0;
    }

    if (methodName == 0) {
        methodName = "main";
    }

    /*
     *  Search for the first class with the given name
     */
    if (className == 0) {
        if (searchForMethod(ejs, methodName, &type) < 0) {
            return EJS_ERR;
        }

    } else {
        ejsName(&qname, EJS_PUBLIC_NAMESPACE, className);
        type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, &qname);
    }

    if (type == 0 || !ejsIsType(type)) {
        mprError(ejs, "Can't find class \"%s\"", className);
        return EJS_ERR;
    }

    ejsName(&qname, EJS_PUBLIC_NAMESPACE, methodName);
    fun = (EjsFunction*) ejsGetPropertyByName(ejs, (EjsVar*) type, &qname);
    if (fun == 0) {
        return MPR_ERR_CANT_ACCESS;
    }
    if (! ejsIsFunction(fun)) {
        mprError(ejs, "Property \"%s\" is not a function");
        return MPR_ERR_BAD_STATE;
    }

    attributes = ejsGetTypePropertyAttributes(ejs, (EjsVar*) type, fun->slotNum);
    if (!(attributes & EJS_ATTR_STATIC)) {
        mprError(ejs, "Method \"%s\" is not declared static");
        return EJS_ERR;
    }

    args = (EjsVar*) ejsCreateArray(ejs, ejs->argc);
    for (i = 0; i < ejs->argc; i++) {
        ejsSetProperty(ejs, args, i, (EjsVar*) ejsCreateString(ejs, ejs->argv[i]));
    }

    if (ejsRunFunction(ejs, fun, 0, 1, &args) == 0) {
        return EJS_ERR;
    }
    return 0;
}


/*
 *  Search for the named method in all types.
 */
static int searchForMethod(Ejs *ejs, cchar *methodName, EjsType **typeReturn)
{
    EjsFunction *method;
    EjsType     *type;
    EjsName     qname;
    EjsVar      *global, *vp;
    int         globalCount, slotNum, methodCount;
    int         methodSlot;

    mprAssert(methodName && *methodName);
    mprAssert(typeReturn);

    global = ejs->global;
    globalCount = ejsGetPropertyCount(ejs, global);

    /*
     *  Search for the named method in all types
     */
    for (slotNum = 0; slotNum < globalCount; slotNum++) {
        vp = ejsGetProperty(ejs, global, slotNum);
        if (vp == 0 || !ejsIsType(vp)) {
            continue;
        }
        type = (EjsType*) vp;

        methodCount = ejsGetPropertyCount(ejs, (EjsVar*) type);

        for (methodSlot = 0; methodSlot < methodCount; methodSlot++) {
            method = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, methodSlot);
            if (method == 0) {
                continue;
            }

            qname = ejsGetPropertyName(ejs, (EjsVar*) type, methodSlot);
            if (qname.name && strcmp(qname.name, methodName) == 0) {
                *typeReturn = type;
            }
        }
    }
    return 0;
}


static void logHandler(MprCtx ctx, int flags, int level, const char *msg)
{
    Mpr         *mpr;
    MprFile     *file;
    char        *prefix;

    mpr = mprGetMpr(ctx);
    file = (MprFile*) mpr->logHandlerData;
    prefix = mpr->name;

    while (*msg == '\n') {
        mprFprintf(file, "\n");
        msg++;
    }

    if (flags & MPR_LOG_SRC) {
        mprFprintf(file, "%s: %d: %s\n", prefix, level, msg);

    } else if (flags & MPR_ERROR_SRC) {
        /*
         *  Use static printing to avoid malloc when the messages are small.
         *  This is important for memory allocation errors.
         */
        if (strlen(msg) < (MPR_MAX_STRING - 32)) {
            mprStaticPrintfError(file, "%s: Error: %s\n", prefix, msg);
        } else {
            mprFprintf(file, "%s: Error: %s\n", prefix, msg);
        }

    } else if (flags & MPR_FATAL_SRC) {
        mprFprintf(file, "%s: Fatal: %s\n", prefix, msg);
        
    } else if (flags & MPR_RAW) {
        mprFprintf(file, "%s", msg);
    }
}


int ejsStartLogging(Mpr *mpr, char *logSpec)
{
    MprFile     *file;
    char        *levelSpec;
    int         level;

    level = 0;
    logSpec = mprStrdup(mpr, logSpec);

    if ((levelSpec = strchr(logSpec, ':')) != 0) {
        *levelSpec++ = '\0';
        level = atoi(levelSpec);
    }

    if (strcmp(logSpec, "stdout") == 0) {
        file = mpr->fileSystem->stdOutput;

    } else if (strcmp(logSpec, "stderr") == 0) {
        file = mpr->fileSystem->stdError;

    } else {
        if ((file = mprOpen(mpr, logSpec, O_CREAT | O_WRONLY | O_TRUNC | O_TEXT, 0664)) == 0) {
            mprPrintfError(mpr, "Can't open log file %s\n", logSpec);
            mprFree(logSpec);
            return EJS_ERR;
        }
    }

    mprSetLogLevel(mpr, level);
    mprSetLogHandler(mpr, logHandler, (void*) file);

    mprFree(logSpec);
    return 0;
}


/*
 *  Global memory allocation handler. This is invoked when there is no notifier to handle an allocation failure.
 *  The interpreter has an allocNotifier (see ejsService: allocNotifier) and it will handle allocation errors.
 */
void ejsMemoryFailure(MprCtx ctx, int64 size, int64 total, bool granted)
{
    if (!granted) {
        mprPrintfError(ctx, "Can't allocate memory block of size %d\n", size);
        mprPrintfError(ctx, "Total memory used %d\n", total);
        exit(255);
    }
    mprPrintfError(ctx, "Memory request for %d bytes exceeds memory red-line\n", size);
    mprPrintfError(ctx, "Total memory used %d\n", total);
}


void ejsReportError(Ejs *ejs, char *fmt, ...)
{
    va_list     arg;
    const char  *msg;
    char        *buf;

    va_start(arg, fmt);
    
    /*
     *  Compiler error format is:
     *      program:line:errorCode:SEVERITY: message
     *  Where program is either "ec" or "ejs"
     *  Where SEVERITY is either "error" or "warn"
     */
    buf = mprVasprintf(ejs, 0, fmt, arg);
    msg = ejsGetErrorMsg(ejs, 1);
    
    mprError(ejs, "%s", (msg) ? msg: buf);
    mprFree(buf);
    va_end(arg);
}

#if BLD_FEATURE_MULTITHREAD
void ejsLockVm(Ejs *ejs)
{
    mprLock(ejs->mutex);
}

void ejsUnlockVm(Ejs *ejs)
{
    mprUnlock(ejs->mutex);
}
#endif

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
