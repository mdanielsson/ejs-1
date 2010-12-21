/*
 *  ejsVm.h - Virtual Machine header.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#ifndef _h_EJS_VM_h
#define _h_EJS_VM_h 1

#include    "mpr.h"
#include    "ejsTune.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************** Defines ***********************************/
#if !DOXYGEN
/*
 *  Forward declare types
 */
struct Ejs;
struct EjsBlock;
struct EjsList;
struct EjsFrame;
struct EjsFunction;
struct EjsModule;
struct EjsState;
struct EjsType;
struct EjsVar;
struct EjsWorker;
#endif

/*
 *  Language compliance levels
 */
#define EJS_SPEC_ECMA           0           /**< Run in strict ECMA-262 compliance mode */
#define EJS_SPEC_PLUS           1           /**< Run in enhanced ECMA-262 with non-breaking changes */
#define EJS_SPEC_FIXED          2           /**< Run with ECMA-262 plus enhancements and add breaking fixes */

#define LANG_ECMA(cp)           (cp->fileState->mode == PRAGMA_MODE_STANDARD)
#define LANG_PLUS(cp)           (cp->fileState->mode == PRAGMA_MODE_STRICT)
#define LANG_FIXED(cp)          (cp->fileState->mode == PRAGMA_MODE_STRICT)

/*
 *  Interpreter flags
 */
#define EJS_FLAG_EVENT          0x1         /**< Event pending */
#define EJS_FLAG_EMPTY          0x2         /**< Create an empty interpreter without native elements */
#define EJS_FLAG_COMPILER       0x4         /**< Running inside the compiler */
#define EJS_FLAG_NO_EXE         0x8         /**< VM will not execute code. Used by compiler without --run */
#define EJS_FLAG_MASTER         0x20        /**< Create a master interpreter */
#define EJS_FLAG_DOC            0x40        /**< Load documentation from modules */
#define EJS_FLAG_EXIT           0x80        /**< Interpreter should exit */
#define EJS_FLAG_NOEXIT         0x200       /**< App should service events and not exit */

#define EJS_FLAG_DYNAMIC        0x400       /* Make a type that is dynamic itself */
#define EJS_STACK_ARG           -1          /* Offset to locate first arg */

/**
 *  Qualified name structure
 *  @description All names in Ejscript consist of a property name and a name space. Namespaces provide discrete
 *      spaces to manage and minimize name conflicts. These names will soon be converted to unicode.
 *  @stability Prototype
 *  @defgroup EjsName EjsName
 *  @see EjsName ejsName ejsAllocName ejsDupName ejsCopyName
 */       
typedef struct EjsName {
    cchar       *name;                          /**< Property name */
    cchar       *space;                         /**< Property namespace */
} EjsName;


/**
 *  Initialize a Qualified Name structure
 *  @description Initialize the statically allocated qualified name structure using a name and namespace.
 *  @param qname Reference to an existing, uninitialized EjsName structure
 *  @param space Namespace string
 *  @param name Name string
 *  @return A reference to the qname structure
 *  @ingroup EjsName
 */
extern EjsName *ejsName(struct EjsName *qname, cchar *space, cchar *name);

#define EN(qname, name) ejsName(qname, "", name)

/**
 *  Allocate and Initialize  a Qualified Name structure
 *  @description Create and initialize a qualified name structure using a name and namespace.
 *  @param ctx Any memory context returned by mprAlloc
 *  @param space Namespace string
 *  @param name Name string
 *  @return A reference to an allocated EjsName structure. Caller must free.
 *  @ingroup EjsName
 */
extern EjsName *ejsAllocName(MprCtx ctx, cchar *space, cchar *name);

extern EjsName *ejsDupName(MprCtx ctx, EjsName *qname);
extern EjsName ejsCopyName(MprCtx ctx, EjsName *qname);

/**
 *  VM Evaluation state. 
 *  The VM Stacks grow forward in memory. A push is done by incrementing first, then storing. ie. *++top = value
 *  A pop is done by extraction then decrement. ie. value = *top--
 *  @ingroup EjsVm
 */
typedef struct EjsState {
    struct EjsFrame     *fp;                /* Current Frame function pointer */
    struct EjsBlock     *bp;                /* Current block pointer */
    struct EjsVar       **stack;            /* Top of stack (points to the last element pushed) */
    struct EjsVar       **stackBase;        /* Pointer to start of stack mem */
    struct EjsVar       **stackEnd;         /* Only used on non-virtual memory systems */
    int                 stackSize;          /* Stack size */
} EjsState;


/**
 *  Lookup State.
 *  @description Location information returned when looking up properties.
 *  @ingroup EjsVm
 */
typedef struct EjsLookup {
    struct EjsVar   *obj;                   /* Final object / Type containing the variable */
    int             slotNum;                /* Final slot in obj containing the variable reference */
    uint            nthBase;                /* Property on Nth super type -- count from the object */
    uint            nthBlock;               /* Property on Nth block in the scope chain -- count from the end */
    uint            useThis;                /* Property accessible via "this." */
    uint            instanceProperty;       /* Property is an instance property */
    uint            ownerIsType;            /* Original object owning the property is a type */
    uint            storing;                /* Lookup and then store a value */

    /*
     *  Just for the compiler
     */
    struct EjsVar   *originalObj;           /* Original object used for the search */
    struct EjsVar   *ref;                   /* Actual property reference */
    struct EjsTrait *trait;                 /* Property trait describing the property */
    struct EjsName  name;                   /* Name and namespace used to find the property */

} EjsLookup;


/*
 *  Default GC thresholds (not tunable)
 */
#define EJS_MIN_TIME_FOR_GC         300     /* Need 1/3 sec for GC */
#define EJS_GC_SHORT_WORK_QUOTA      50     /* Predict GC short of a full work quota */
    
/*
 *  GC Object generations
 */
#define EJS_GEN_NEW         0           /* New objects */
#define EJS_GEN_ETERNAL     1           /* Builtin objects that live forever */
#define EJS_MAX_GEN         2           /* Number of generations for object allocation */

/*
 *  GC Collection modes
 */
#define EJS_GC_ETERNAL      1           /* Collect eternal generation */

/*
 *  GC Per generation structure
 */
typedef struct EjsGen
{
    uint            totalReclaimed;     /* Total blocks reclaimed on sweeps */
    uint            totalSweeps;        /* Total sweeps */
} EjsGen;

/*
 *  GC Pool of free objects of a given type. Each type maintains a free pool for faster allocations.
 *  Types in the pool have a weak reference and may be reclaimed.
 */
typedef struct EjsPool
{
    struct EjsType  *type;              /* Owning type */
    int             allocated;          /* Count of instances created */
    int             peakAllocated;      /* High water mark for allocated */
    int             count;              /* Count in pool */
    int             peakCount;          /* High water mark for count */
    int             reuse;              /* Count of reuses */
} EjsPool;


/*
 *  Garbage collector control
 */
typedef struct EjsGC {

    EjsGen      *generations[EJS_MAX_GEN];
    EjsPool     *pools[EJS_MAX_TYPE];   /* Object pools */
    int         numPools;               /* Count of object pools */
    uint        allocGeneration;        /* Current generation accepting objects */
    uint        collectGeneration;      /* Current generation doing GC */
    uint        markGenRef;             /* Generation to mark objects */
    uint        firstGlobal;            /* First global slots to examine */
    bool        collecting;             /* Running garbage collection */
    bool        enabled;                /* GC is enabled */
    int         degraded;               /* Have exceeded redlineMemory */
    uint        allocatedTypes;         /* Count of types allocated */
    uint        peakAllocatedTypes;     /* Peak allocated types */ 
    uint        allocatedObjects;       /* Count of objects allocated */
    uint        peakAllocatedObjects;   /* Peak allocated */ 
    uint        peakMemory;             /* Peak memory usage */
    uint        totalAllocated;         /* Total count of allocation calls */
    uint        totalReclaimed;         /* Total blocks reclaimed on sweeps */
    uint        totalOverflows;         /* Total overflows  */
    uint        totalRedlines;          /* Total times redline limit exceeded */
    uint        totalSweeps;            /* Total sweeps */
#if BLD_DEBUG
    int         indent;                 /* Indent formatting in GC reports */
#endif
} EjsGC;

/******************************** Internal GC API ********************************/

extern int      ejsSetGeneration(struct Ejs *ejs, int generation);
extern void     ejsAnalyzeGlobal(struct Ejs *ejs);
extern int      ejsCreateGCService(struct Ejs *ejs);
extern void     ejsDestroyGCService(struct Ejs *ejs);
extern int      ejsIsTimeForGC(struct Ejs *ejs, int timeTillNextEvent);
extern void     ejsCollectEverything(struct Ejs *ejs);
extern void     ejsCollectGarbage(struct Ejs *ejs, int gen);
extern int      ejsEnableGC(struct Ejs *ejs, bool on);
extern void     ejsTraceMark(struct Ejs *ejs, struct EjsVar *vp);
extern void     ejsGracefulDegrade(struct Ejs *ejs);
extern void     ejsPrintAllocReport(struct Ejs *ejs);
extern void     ejsMakeEternalPermanent(struct Ejs *ejs);
extern void     ejsMakePermanent(struct Ejs *ejs, struct EjsVar *vp);
extern void     ejsMakeTransient(struct Ejs *ejs, struct EjsVar *vp);

#if BLD_DEBUG
extern void     ejsAddToGcStats(struct Ejs *ejs, struct EjsVar *vp, int id);
#else
#define ejsAddToGcStats(ejs, vp, id)
#endif

typedef struct EjsLoadState {
    MprList     *typeFixups;        /**< Loaded types to fixup */
    MprList     *modules;           /**< Modules being loaded */
} EjsLoadState;

typedef void (*EjsLoaderCallback)(struct Ejs *ejs, int kind, ...);

/**
 *  Ejsript Interperter Structure
 *  @description The Ejs structure contains the state for a single interpreter. The #ejsCreate routine may be used
 *      to create multiple interpreters and returns a reference to be used in subsequent Ejscript API calls.
 *  @stability Prototype.
 *  @defgroup Ejs Ejs
 *  @see ejsCreate, ejsCreateService, ejsAppendSearchPath, ejsSetSearchPath, ejsEvalFile, ejsEvalScript, ejsExit
 */
typedef struct Ejs {
    struct EjsVar       *exception;         /**< Pointer to exception object */
    struct EjsVar       *result;            /**< Last expression result */
    struct EjsState     *state;             /**< Current evaluation state and stack */
    struct EjsState     *masterState;       /**< Owns the eval stack */

    struct EjsService   *service;           /**< Back pointer to the service */
    struct Ejs          *master;            /**< Inherit builtin types from the master */
    EjsGC               gc;                 /**< Garbage collector state */
    EjsGen              *currentGeneration; /**< Current allocation generation */
    MprHeap             *heap;              /**< Allocation heap */
    char                *ejsPath;           /**< Module load search path */

    /*
     *  Essential types
     */
    struct EjsType      *arrayType;         /**< Array type */
    struct EjsType      *blockType;         /**< Block type */
    struct EjsType      *booleanType;       /**< Boolean type */
    struct EjsType      *byteArrayType;     /**< ByteArray type */
    struct EjsType      *dateType;          /**< Date type */
    struct EjsType      *errorType;         /**< Error type */
    struct EjsType      *errorEventType;    /**< ErrorEvent type */
    struct EjsType      *eventType;         /**< Event type */
    struct EjsType      *frameType;         /**< Frame type */
    struct EjsType      *functionType;      /**< Function type */
    struct EjsType      *iteratorType;      /**< Iterator type */
    struct EjsType      *mathType;          /**< Math type */
    struct EjsType      *namespaceType;     /**< Namespace type */
    struct EjsType      *nullType;          /**< Null type */
    struct EjsType      *numberType;        /**< Default numeric type */
    struct EjsType      *objectType;        /**< Object type */
    struct EjsType      *regExpType;        /**< RegExp type */
    struct EjsType      *stringType;        /**< String type */
    struct EjsType      *stopIterationType; /**< StopIteration type */
    struct EjsType      *typeType;          /**< Type type */
    struct EjsType      *voidType;          /**< Void type */
    struct EjsType      *workerType;        /**< Worker type */
    struct EjsType      *xmlType;           /**< XML type */
    struct EjsType      *xmlListType;       /**< XMLList type */

    /*
     *  Key values
     */
    struct EjsVar       *global;            /**< The "global" object as an EjsVar */
    struct EjsBlock     *globalBlock;       /**< The "global" object as an EjsBlock */

    struct EjsString    *emptyStringValue;  /**< "" value */
    struct EjsBoolean   *falseValue;        /**< The "false" value */
    struct EjsNumber    *infinityValue;     /**< The infinity number value */
    struct EjsNumber    *maxValue;          /**< Maximum number value */
    struct EjsNumber    *minValue;          /**< Minimum number value */
    struct EjsNumber    *minusOneValue;     /**< The -1 number value */
    struct EjsNumber    *nanValue;          /**< The "NaN" value if floating point numbers, else zero */
    struct EjsNumber    *negativeInfinityValue; /**< The negative infinity number value */
    struct EjsVar       *nullValue;         /**< The "null" value */
    struct EjsNumber    *oneValue;          /**< The 1 number value */
    struct EjsBoolean   *trueValue;         /**< The "true" value */
    struct EjsVar       *undefinedValue;    /**< The "void" value */
    struct EjsNumber    *zeroValue;         /**< The 0 number value */
    struct EjsFunction  *memoryCallback;    /**< Memory.readline callback */

    struct EjsNamespace *configSpace;       /**< CONFIG namespace */
    struct EjsNamespace *emptySpace;        /**< Empty namespace */
    struct EjsNamespace *intrinsicSpace;    /**< Intrinsic namespace */
    struct EjsNamespace *iteratorSpace;     /**< Iterator namespace */
    struct EjsNamespace *internalSpace;     /**< Internal namespace */
    struct EjsNamespace *publicSpace;       /**< Public namespace */
    struct EjsNamespace *eventsSpace;       /**< ejs.events namespace */
    struct EjsNamespace *ioSpace;           /**< ejs.io namespace */
    struct EjsNamespace *sysSpace;          /**< ejs.sys namespace */

    char                *castTemp;          /**< Temporary string for casting */
    char                *errorMsg;          /**< Error message */
    char                **argv;             /**< Command line args */
    int                 argc;               /**< Count of command line args */
    int                 flags;              /**< Execution flags */
    int                 exitStatus;         /**< Status to exit() */
    int                 serializeDepth;     /**< Serialization depth */
    int                 joining;            /**< In Worker.join */

    int                 workQuota;          /* Quota of work before GC */
    int                 workDone;           /**< Count of allocations to determining if GC needed */
    int                 gcRequired;         /**< Garbage collection is now required */

    uint                initialized: 1;     /**< Interpreter fully initialized */
    uint                hasError: 1;        /**< Interpreter has an initialization error */
    uint                noExceptions: 1;    /**< Suppress exceptions */
    uint                exiting: 1;         /**< VM should exit */

    struct EjsVar       *exceptionArg;      /**< Exception object for catch block */

    bool                attention;          /**< VM needs attention */

    struct EjsTypeHelpers *defaultHelpers;  /**< Default EjsVar helpers */
    struct EjsTypeHelpers *blockHelpers;    /**< EjsBlock helpers */
    struct EjsTypeHelpers *objectHelpers;   /**< EjsObject helpers */

    MprDispatcher       *dispatcher;        /**< Event dispatcher */
    MprList             *workers;           /**< Worker interpreters */
    MprList             *modules;           /**< Loaded modules */
    EjsLoadState        *loadState;         /**< State while loading modules */

    void                (*loaderCallback)(struct Ejs *ejs, int kind, ...);
    void                *userData;          /**< User data */
    void                *handle;            /**< Hosting environment handle */

    MprHashTable        *coreTypes;         /**< Core type instances */
    MprHashTable        *standardSpaces;    /**< Hash of standard namespaces (global namespaces) */
    MprHashTable        *doc;               /**< Documentation */
    void                *sqlite;            /**< Sqlite context information */

#if BLD_FEATURE_MULTITHREAD
    MprMutex            *mutex;             /**< Multithread synchronization */
#endif
} Ejs;


/**
 *  Ejscript Service structure
 *  @description The Ejscript service manages the overall language runtime. It 
 *      is the factory that creates interpreter instances via #ejsCreate.
 *  @ingroup Ejs
 */
typedef struct EjsService {
    MprHashTable        *nativeModules;     /**< Native module initialization callbacks */
    struct EjsVar       *(*loadScriptLiteral)(struct Ejs *ejs, cchar *script);
    struct EjsVar       *(*loadScriptFile)(struct Ejs *ejs, cchar *path);
} EjsService;

#define ejsGetAllocCtx(ejs) ejs->currentGeneration

/*********************************** Prototypes *******************************/
/**
 *  Open the Ejscript service
 *  @description One Ejscript service object is required per application. From this service, interpreters
 *      can be created.
 *  @param ctx Any memory context returned by mprAlloc
 *  @return An ejs service object
 *  @ingroup Ejs
 */
extern EjsService *ejsCreateService(MprCtx ctx);

/**
 *  Create an ejs interpreter
 *  @description Create an interpreter object to evalute Ejscript programs. Ejscript supports multiple interpreters.
 *      One interpreter can be designated as a master interpreter and then it can be cloned by supplying the master
 *      interpreter to this call. A master interpreter provides the standard system types and clone interpreters can
 *      quickly be created an utilize the master interpreter's types. This saves memory and speeds initialization.
 *  @param ctx Any memory context returned by mprAlloc
 *  @param master Optional master interpreter to clone.
 *  @param searchPath Module search path to use. Set to NULL for the default search path.
 *  @param flags Optional flags to modify the interpreter behavior. Valid flags are:
 *      @li    EJS_FLAG_COMPILER       - Interpreter will compile code from source
 *      @li    EJS_FLAG_NO_EXE         - Don't execute any code. Just compile.
 *      @li    EJS_FLAG_MASTER         - Create a master interpreter
 *      @li    EJS_FLAG_DOC            - Load documentation from modules
 *      @li    EJS_FLAG_NOEXIT         - App should service events and not exit unless explicitly instructed
 *  @return A new interpreter
 *  @ingroup Ejs
 */
extern Ejs *ejsCreate(MprCtx ctx, struct Ejs *master, cchar *searchPath, int flags);

/**
 *  Append to the module search path
 *  @description Append a path to the ejs module search path.
 *  @param ejs Ejs interpreter
 *  @param ejsPath Search path. This is a colon (or semicolon on Windows) separated string of directories.
 *  @ingroup Ejs
 */
extern void ejsAppendSearchPath(Ejs *ejs, cchar *ejsPath);

/**
 *  Set the module search path
 *  @description Set the ejs module search path. The search path is by default set to the value of the EJSPATH
 *      environment directory. Ejsript will search for modules by name. The search strategy is:
 *      Given a name "a.b.c", scan for:
 *      @li File named a.b.c
 *      @li File named a/b/c
 *      @li File named a.b.c in EJSPATH
 *      @li File named a/b/c in EJSPATH
 *      @li File named c in EJSPATH
 *
 *  Ejs will search for files with no extension and also search for modules with a ".mod" extension. If there is
 *  a shared library of the same name with a shared library extension (.so, .dll, .dylib) and the module requires 
 *  native code, then the shared library will also be loaded.
 *  @param ejs Ejs interpreter
 *  @param ejsPath Search path. This is a colon (or semicolon on Windows) separated string of directories.
 *  @ingroup Ejs
 */
extern void ejsSetSearchPath(Ejs *ejs, cchar *ejsPath);

/**
 *  Evaluate a file
 *  @description Evaluate a file containing an Ejscript. This requires linking with the Ejscript compiler library (libec). 
 *  @param path Filename of the script to evaluate
 *  @return Return zero on success. Otherwise return a negative Mpr error code.
 *  @ingroup Ejs
 */
extern int ejsEvalFile(cchar *path);

extern int ejsLoadScriptFile(Ejs *ejs, cchar *path, int flags);
extern int ejsLoadScriptLiteral(Ejs *ejs, cchar *script, int flags);

/**
 *  Evaluate a module
 *  @description Evaluate a module containing compiled Ejscript.
 *  @param path Filename of the module to evaluate.
 *  @return Return zero on success. Otherwise return a negative Mpr error code.
 *  @ingroup Ejs
 */
extern int ejsEvalModule(cchar *path);

/**
 *  Evaluate a script
 *  @description Evaluate a script. This requires linking with the Ejscript compiler library (libec). 
 *  @param script Script to evaluate
 *  @return Return zero on success. Otherwise return a negative Mpr error code.
 *  @ingroup Ejs
 */
extern int ejsEvalScript(cchar *script);

/**
 *  Instruct the interpreter to exit.
 *  @description This will instruct the interpreter to cease interpreting any further script code.
 *  @param ejs Interpeter object returned from #ejsCreate
 *  @param status Reserved and ignored
 *  @ingroup Ejs
 */
extern void ejsExit(Ejs *ejs, int status);

/**
 *  Get the hosting handle
 *  @description The interpreter can store a hosting handle. This is typically a web server object if hosted inside
 *      a web server
 *  @param ejs Interpeter object returned from #ejsCreate
 *  @return Hosting handle
 *  @ingroup Ejs
 */
extern void *ejsGetHandle(Ejs *ejs);

/**
 *  Run a script
 *  @description Run a script that has previously ben compiled by ecCompile
 *  @param ejs Interpeter object returned from #ejsCreate
 *  @return Zero if successful, otherwise a non-zero Mpr error code.
 */
extern int ejsRun(Ejs *ejs);

/**
 *  Throw an exception
 *  @description Throw an exception object 
 *  @param ejs Interpeter object returned from #ejsCreate
 *  @param error Exception argument object.
 *  @return The exception argument for chaining.
 *  @ingroup Ejs
 */
extern struct EjsVar *ejsThrowException(Ejs *ejs, struct EjsVar *error);
extern void ejsClearException(Ejs *ejs);

/**
 *  Report an error message using the MprLog error channel
 *  @description This will emit an error message of the format:
 *      @li program:line:errorCode:SEVERITY: message
 *  @param ejs Interpeter object returned from #ejsCreate
 *  @param fmt Is an alternate printf style format to emit if the interpreter has no valid error message.
 *  @param ... Arguments for fmt
 *  @ingroup Ejs
 */
extern void ejsReportError(Ejs *ejs, char *fmt, ...);

extern int ejsAddModule(Ejs *ejs, struct EjsModule *up);
extern struct EjsVar *ejsCastOperands(Ejs *ejs, struct EjsVar *lhs, int opcode,  struct EjsVar *rhs);
extern int ejsCheckModuleLoaded(Ejs *ejs, cchar *name);
extern void ejsClearExiting(Ejs *ejs);
extern struct EjsVar *ejsCreateException(Ejs *ejs, int slot, cchar *fmt, va_list fmtArgs);
extern MprList *ejsGetModuleList(Ejs *ejs);
extern struct EjsVar *ejsGetVarByName(Ejs *ejs, struct EjsVar *vp, EjsName *name, EjsLookup *lookup);
extern int ejsInitStack(Ejs *ejs);
extern void ejsLog(Ejs *ejs, cchar *fmt, ...);
extern int ejsLookupVar(Ejs *ejs, struct EjsVar *vp, EjsName *name, EjsLookup *lookup);
extern int ejsLookupVarWithNamespaces(Ejs *ejs, struct EjsVar *vp, EjsName *name, EjsLookup *lookup);
extern struct EjsModule *ejsLookupModule(Ejs *ejs, cchar *name, int minVersion, int maxVersion);
extern int ejsLookupScope(Ejs *ejs, EjsName *name, EjsLookup *lookup);
extern void ejsMemoryFailure(MprCtx ctx, int64 size, int64 total, bool granted);
extern int ejsRemoveModule(Ejs *ejs, struct EjsModule *up);
extern int ejsRunProgram(Ejs *ejs, cchar *className, cchar *methodName);
extern void ejsSetHandle(Ejs *ejs, void *handle);
extern void ejsShowCurrentScope(Ejs *ejs);
extern void ejsShowStack(Ejs *ejs, struct EjsFunction *fp);
extern void ejsShowBlockScope(Ejs *ejs, struct EjsBlock *block);
extern int ejsStartLogging(Mpr *mpr, char *logSpec);
extern struct EjsTypeHelpers *ejsGetDefaultHelpers(Ejs *ejs);
extern struct EjsTypeHelpers *ejsGetObjectHelpers(Ejs *ejs);
extern struct EjsTypeHelpers *ejsGetBlockHelpers(Ejs *ejs);

#if BLD_FEATURE_MULTITHREAD
extern void ejsLockVm(Ejs *ejs);
extern void ejsUnlockVm(Ejs *ejs);
#else
#define ejsLockVm(ejs)
#define ejsUnlockVm(ejs)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _h_EJS_VM_h */

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
