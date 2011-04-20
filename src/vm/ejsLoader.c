/**
 *  ejsLoader.c - Ejscript module file file loader
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/****************************** Forward Declarations **************************/

static int  addFixup(Ejs *ejs, int kind, EjsVar *target, int slotNum, EjsTypeFixup *fixup);
static EjsTypeFixup *createFixup(Ejs *ejs, EjsName *qname, int slotNum);
static int  fixupTypes(Ejs *ejs, MprList *list);
static int  initializeModule(Ejs *ejs, EjsModule *mp, cchar *path);
static int  loadBlockSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadClassSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadDependencySection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadEndBlockSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadEndFunctionSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadEndClassSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadEndModuleSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadExceptionSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static int  loadFunctionSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static EjsModule *loadModuleSection(Ejs *ejs, MprFile *file, EjsModuleHdr *hdr, int *created, int flags);
static int  loadSections(Ejs *ejs, MprFile *file, EjsModuleHdr *hdr, int flags);
static int  loadPropertySection(Ejs *ejs, MprFile *file, EjsModule *mp, int sectionType);
static int  loadScriptModule(Ejs *ejs, MprFile *file, cchar *path, int flags);
static char *makeModuleName(MprCtx ctx, cchar *name);
static int  readNumber(Ejs *ejs, MprFile *file, int *number);
static int  readWord(Ejs *ejs, MprFile *file, int *number);
static char *search(Ejs *ejs, char *filename, int minVersion, int maxVersion);
static double swapDoubleWord(Ejs *ejs, double a);
static int  swapWord(Ejs *ejs, int word);
static char *tokenToString(EjsModule *mp, int   token);

#if !BLD_FEATURE_STATIC
static int  loadNativeLibrary(Ejs *ejs, EjsModule *mp, cchar *path);
#endif

#if BLD_FEATURE_EJS_DOC
static int  loadDocSection(Ejs *ejs, MprFile *file, EjsModule *mp);
static void setDoc(Ejs *ejs, EjsModule *mp, EjsVar *block, int slotNum);
#endif

/******************************************************************************/
/**
 *  Load a module file and return a list of the loaded modules. This is used to load scripted module files with
 *  optional native (shared / DLL) implementations. If loading a scripted module that has native declarations, a
 *  search for the corresponding native DLL will be performed and both scripted and native module files will be loaded.
 *  NOTE: this may recursively call itself as it loads dependent modules.
 *
 *  @param ejs Ejs handle
 *  @param nameArg Module name to load. May be "." separated path. May include or omit the ".mod" extension.
 *  @param minVersion Minimum acceptable version (inclusive). Set to zero for unversioned.
 *  @param maxVersion Maximum acceptable version (inclusive). Set to -1 for all versions.
 *  @param flags Reserved. Must be set to zero.
 *  @param modulesArg List of modules loaded. Will only return a list if successful and doing a top level load. 
 *      When ejsLoadModule is called to load dependant modules, not list of modules will be returned.
 *      The final list of modules aggregates all modules loaded including those from dependant modules.
 *  @return Returns the last loaded module.
 */
int ejsLoadModule(Ejs *ejs, cchar *nameArg, int minVersion, int maxVersion, int flags, MprList **modulesArg)
{
    MprFile         *file;
    MprList         *modules;
    MprCtx          ctx;
    EjsLoadState    *ls;
    EjsModule       *mp;
    char            *cp, *path, *name, *base;
    int             next, status;

    mprAssert(nameArg && *nameArg);

    path = 0;
    status = 0;
    modules = 0;
    ctx = mprAlloc(ejs, 0);

    name = makeModuleName(ctx, nameArg);
    base = mprGetPathBase(ctx, nameArg);
    if ((cp = strrchr(base, '.')) != 0 && strcmp(cp, EJS_MODULE_EXT) == 0) {
        *cp = '\0';
    }

    if ((mp = ejsLookupModule(ejs, base, minVersion, maxVersion)) != 0) {
        if (mp->compiling && strcmp(name, EJS_DEFAULT_MODULE) != 0) {
            ejsThrowStateError(ejs, "Attempt to load module \"%s\" that is currently being compiled.", name);
            mprFree(ctx);
            return MPR_ERR_ALREADY_EXISTS;
        }
        if (modulesArg && ejs->loadState == 0) {
            modules = mprCreateList(ejs);
            mprAddItem(modules, mp);
        }
        mprFree(ctx);
        return 0;
    }
    if ((path = search(ejs, name, minVersion, maxVersion)) == 0) {
        mprFree(ctx);
        return MPR_ERR_CANT_ACCESS;
    }
    mprLog(ejs, 3, "Loading module %s", path);

    if ((file = mprOpen(ctx, path, O_RDONLY | O_BINARY, 0666)) == 0) {
        ejsThrowIOError(ejs, "Can't open module file %s", path);
        mprFree(path);
        mprFree(ctx);
        return MPR_ERR_CANT_OPEN;
    }
    mprEnableFileBuffering(file, 0, 0);

    if (ejs->loadState == 0) {
        ls = ejs->loadState = mprAllocObjZeroed(ejs, EjsLoadState);
        ls->typeFixups = mprCreateList(ls);
        modules = ls->modules = mprCreateList(ls);

        status = loadScriptModule(ejs, file, path, flags);
        ejs->loadState = 0;

        /*
         *  Delay doing fixups and running initializers until all modules are loaded. Then do all fixups first prior to
         *  running the initializers. This solves the forward type reference problem.
         */
        if (status == 0 && fixupTypes(ejs, ls->typeFixups) == 0) {
            for (next = 0; (mp = mprGetNextItem(modules, &next)) != 0; ) {
                if ((status = initializeModule(ejs, mp, path)) < 0) {
                    break;
                }
            }
        }
        if (modulesArg) {
            *modulesArg = modules;
            mprStealBlock(ejs, modules);
            mprAddItem(modules, mp);
        }
        mprFree(ls);

    } else {
        status = loadScriptModule(ejs, file, path, flags);
    }

    mprFree(path);
    mprFree(ctx);
    return status;
}


static int initializeModule(Ejs *ejs, EjsModule *mp, cchar *path)
{
    EjsNativeCallback   moduleCallback;

    if (mp->hasNative && !mp->configured) {
        /*
         *  See if a native module initialization routine has been registered. If so, use that. Otherwise, look
         *  for a backing DSO.
         */
        if ((moduleCallback = (EjsNativeCallback) mprLookupHash(ejs->service->nativeModules, mp->name)) != 0) {
            if ((moduleCallback)(ejs, mp, path) < 0) {
                return MPR_ERR_CANT_INITIALIZE;
            }
            
        } else {
#if !BLD_FEATURE_STATIC
            int     rc;
            char *dir = mprGetPathDir(ejs, path);
            rc = loadNativeLibrary(ejs, mp, dir);
            mprFree(dir);
            if (rc < 0) {
                if (ejs->exception == 0) {
                    ejsThrowIOError(ejs, "Can't load the native module file \"%s\"", path);
                }
                return MPR_ERR_CANT_INITIALIZE;
            }
#endif
        }
    }
    mp->configured = 1;
    if (!(ejs->flags & EJS_FLAG_NO_EXE) && ejsRunInitializer(ejs, mp) == 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    return 0;
}


/*
 *  Strip existing extension and replace with .mod 
 */
static char *makeModuleName(MprCtx ctx, cchar *nameArg)
{
    char        *name, *cp, *filename;

    name = mprAlloc(ctx, (int) (strlen(nameArg) + strlen(EJS_MODULE_EXT) + 1));
    strcpy(name, nameArg);
    if ((cp = strrchr(name, '.')) != 0 && strcmp(cp, EJS_MODULE_EXT) == 0) {
        *cp = '\0';
    }
    filename = mprStrcat(ctx, -1, name, EJS_MODULE_EXT, NULL);
    mprFree(name);
    return filename;
}



static char *search(Ejs *ejs, char *filename, int minVersion, int maxVersion) 
{
    char        *path;

    if ((path = ejsSearchForModule(ejs, filename, minVersion, maxVersion)) == 0) {
        mprLog(ejs, 2, "Can't find module file \"%s\" in search path \"%s\"", filename, 
            ejs->ejsPath ? ejs->ejsPath : "");
        ejsThrowReferenceError(ejs,  "Can't find module file \"%s\", min version %d.%d.%d, max version %d.%d.%d", filename, 
            EJS_MAJOR(minVersion), EJS_MINOR(minVersion), EJS_PATCH(minVersion),
            EJS_MAJOR(maxVersion), EJS_MINOR(maxVersion), EJS_PATCH(maxVersion));
        return 0;
    }
    return path;
}


/*
 *  Load the sections: classes, properties and functions. Return the first module loaded in pup.
 */
static int loadSections(Ejs *ejs, MprFile *file, EjsModuleHdr *hdr, int flags)
{
    EjsModule   *mp, *firstModule;
    int         rc, sectionType, created;

    created = 0;

    firstModule = mp = 0;

    while ((sectionType = mprGetc(file)) >= 0) {

        if (sectionType < 0 || sectionType >= EJS_SECT_MAX) {
            mprError(ejs, "Bad section type %d in %s", sectionType, mp->name);
            return EJS_ERR;
        }
        mprLog(ejs, 9, "Load section type %d", sectionType);

        rc = 0;
        switch (sectionType) {

        case EJS_SECT_BLOCK:
            rc = loadBlockSection(ejs, file, mp);
            break;

        case EJS_SECT_BLOCK_END:
            rc = loadEndBlockSection(ejs, file, mp);
            break;

        case EJS_SECT_CLASS:
            rc = loadClassSection(ejs, file, mp);
            break;

        case EJS_SECT_CLASS_END:
            rc = loadEndClassSection(ejs, file, mp);
            break;

        case EJS_SECT_DEPENDENCY:
            rc = loadDependencySection(ejs, file, mp);
            break;

        case EJS_SECT_EXCEPTION:
            rc = loadExceptionSection(ejs, file, mp);
            break;

        case EJS_SECT_FUNCTION:
            rc = loadFunctionSection(ejs, file, mp);
            break;

        case EJS_SECT_FUNCTION_END:
            rc = loadEndFunctionSection(ejs, file, mp);
            break;

        case EJS_SECT_MODULE:
            mp = loadModuleSection(ejs, file, hdr, &created, flags);
            if (mp == 0) {
                return 0;
            }
            if (firstModule == 0) {
                firstModule = mp;
            }
            ejsAddModule(ejs, mp);
            mprAddItem(ejs->loadState->modules, mp);
            break;

        case EJS_SECT_MODULE_END:
            rc = loadEndModuleSection(ejs, file, mp);
            break;

        case EJS_SECT_PROPERTY:
            rc = loadPropertySection(ejs, file, mp, sectionType);
            break;

#if BLD_FEATURE_EJS_DOC
        case EJS_SECT_DOC:
            rc = loadDocSection(ejs, file, mp);
            break;
#endif

        default:
            mprAssert(0);
            return EJS_ERR;
        }

        if (rc < 0) {
            if (mp && mp->name && created) {
                ejsRemoveModule(ejs, mp);
                mprRemoveItem(ejs->loadState->modules, mp);
                mprFree(mp);
            }
            return rc;
        }
    }
    return 0;
}


/*
 *  Load a module section and constant pool.
 */
static EjsModule *loadModuleSection(Ejs *ejs, MprFile *file, EjsModuleHdr *hdr, int *created, int flags)
{
    EjsModule   *mp;
    char        *pool, *name;
    int         rc, version, checksum, poolSize, nameToken;

    mprAssert(created);

    *created = 0;
    checksum = 0;

    /*
     *  We don't have the constant pool yet so we cant resolve the name yet.
     */
    rc = 0;
    rc += readNumber(ejs, file, &nameToken);
    rc += readNumber(ejs, file, &version);
    rc += readWord(ejs, file, &checksum);
    rc += readNumber(ejs, file, &poolSize);
    if (rc < 0 || poolSize <= 0 || poolSize > EJS_MAX_POOL) {
        return 0;
    }

    /*
     *  Read the string constant pool. The pool calls Steal when the module is created.
     */
    pool = (char*) mprAlloc(file, poolSize);
    if (pool == 0) {
        return 0;
    }
    if (mprRead(file, pool, poolSize) != poolSize) {
        mprFree(pool);
        return 0;
    }

    /*
     *  Convert module token into a name
     */
    if (nameToken < 0 || nameToken >= poolSize) {
        mprAssert(0);
        return 0;
    }
    name = &pool[nameToken];
    if (name == 0) {
        mprAssert(name);
        mprFree(pool);
        return 0;
    }

    mp = ejsCreateModule(ejs, name, version);
    if (mp == 0) {
        mprFree(pool);
        return 0;
    }
    ejsSetModuleConstants(ejs, mp, pool, poolSize);
    mp->scopeChain = ejs->globalBlock;
    mp->checksum = checksum;
    *created = 1;

    if (strcmp(name, EJS_DEFAULT_MODULE) != 0) {
        /*
         *  Signify that loading the module has begun. We allow multiple loads into the default module.
         */
        mp->loaded = 1;
        mp->constants->locked = 1;
    }
    mp->file = file;
    mp->flags = flags;
    mp->firstGlobalSlot = ejsGetPropertyCount(ejs, ejs->global);

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_MODULE, mp);
    }
    mprLog(ejs, 9, "Load module section %s", name);
    return mp;
}


static int loadEndModuleSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    mprLog(ejs, 9, "End module section %s", mp->name);

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_MODULE_END, mp);
    }
    return 0;
}


static int loadDependencySection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsModule   *module;
    void        *saveCallback;
    char        *name;
    int         rc, next, minVersion, maxVersion, checksum, nextModule;

    mprAssert(ejs);
    mprAssert(file);
    mprAssert(mp);

    name = ejsModuleReadString(ejs, mp);
    ejsModuleReadNumber(ejs, mp, &checksum);
    ejsModuleReadNumber(ejs, mp, &minVersion);
    ejsModuleReadNumber(ejs, mp, &maxVersion);
    if (mp->hasError) {
        return MPR_ERR_CANT_READ;
    }
    
    saveCallback = ejs->loaderCallback;
    ejs->loaderCallback = NULL;
    nextModule = mprGetListCount(ejs->loadState->modules);

    mprLog(ejs, 5, "    Load dependency section %s", name);
    rc = ejsLoadModule(ejs, name, minVersion, maxVersion, mp->flags, NULL);

    ejs->loaderCallback = saveCallback;
    if (rc < 0) {
        return rc;
    }
    if ((module = ejsLookupModule(ejs, name, minVersion, maxVersion)) != 0) {
        if (checksum != module->checksum) {
            ejsThrowIOError(ejs, "Can't load module %s.\n"
                "It was compiled using a different version of module %s.", 
                mp->name, name);
            return MPR_ERR_BAD_STATE;
        }
    }

    if (mp->dependencies == 0) {
        mp->dependencies = mprCreateList(mp);
    }
    for (next = nextModule; (module = mprGetNextItem(ejs->loadState->modules, &next)) != 0; ) {
        mprAddItem(mp->dependencies, module);
        if (ejs->loaderCallback) {
            (ejs->loaderCallback)(ejs, EJS_SECT_DEPENDENCY, mp, module);
        }
    }
    /*
     *  Note the first free global slot after loading the module.
     */
    mp->firstGlobalSlot = ejsGetPropertyCount(ejs, ejs->global);
    return 0;
}


static int loadBlockSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsBlock    *block;
    EjsVar      *owner;
    EjsName     qname;
    int         slotNum, numSlot;

    qname.space = EJS_BLOCK_NAMESPACE;
    qname.name = ejsModuleReadString(ejs, mp);
    ejsModuleReadNumber(ejs, mp, &slotNum);
    ejsModuleReadNumber(ejs, mp, &numSlot);

    if (mp->hasError) {
        return MPR_ERR_CANT_READ;
    }
    
    block = ejsCreateBlock(ejs, numSlot);
    ejsSetDebugName(block, qname.name);
    owner = (EjsVar*) mp->scopeChain;

    if (ejsLookupProperty(ejs, owner, &qname) >= 0) {
        ejsThrowReferenceError(ejs, "Block \"%s\" already loaded", qname.name);
        return MPR_ERR_CANT_CREATE;
    }

    slotNum = ejsDefineProperty(ejs, owner, slotNum, &qname, ejs->blockType, 0, (EjsVar*) block);
    if (slotNum < 0) {
        return MPR_ERR_CANT_WRITE;
    }

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_BLOCK, mp, owner, slotNum, qname.name, numSlot, block);
    }
    block->scopeChain = mp->scopeChain;
    mp->scopeChain = block;
    return 0;
}


static int loadEndBlockSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    mprLog(ejs, 9, "    End block section %s", mp->name);

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_BLOCK_END, mp);
    }
    mp->scopeChain = mp->scopeChain->scopeChain;
    return 0;
}


static int loadClassSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsType         *type, *baseType, *iface, *nativeType;
    EjsTypeFixup    *fixup, *ifixup;
    EjsName         qname, baseClassName, ifaceClassName;
    EjsBlock        *block;
    int             attributes, numTypeProp, numInstanceProp, slotNum, numInterfaces, i;

    fixup = 0;
    ifixup = 0;
    
    qname.name = ejsModuleReadString(ejs, mp);
    qname.space = ejsModuleReadString(ejs, mp);
    
    ejsModuleReadNumber(ejs, mp, &attributes);
    ejsModuleReadNumber(ejs, mp, &slotNum);
    ejsModuleReadType(ejs, mp, &baseType, &fixup, &baseClassName, 0);
    ejsModuleReadNumber(ejs, mp, &numTypeProp);
    ejsModuleReadNumber(ejs, mp, &numInstanceProp);
    ejsModuleReadNumber(ejs, mp, &numInterfaces);

    if (mp->hasError) {
        return MPR_ERR_CANT_READ;
    }
    if (ejsLookupProperty(ejs, ejs->global, &qname) >= 0) {
        ejsThrowReferenceError(ejs, "Class \"%s\" already loaded", qname.name);
        return MPR_ERR_CANT_CREATE;
    }
    if (fixup || (baseType && baseType->needFixup)) {
        attributes |= EJS_ATTR_SLOTS_NEED_FIXUP;
    }

    /*
     *  Find pre-existing native types.
     */
    if (attributes & EJS_ATTR_NATIVE) {
        type = nativeType = (EjsType*) mprLookupHash(ejs->coreTypes, qname.name);
        if (type == 0) {
            mprLog(ejs, 1, "WARNING: can't find native type \"%s\"", qname.name);
        }
    } else {
        type = nativeType = 0;
#if BLD_DEBUG
        if (mprLookupHash(ejs->coreTypes, qname.name)) {
            mprError(ejs, "WARNING: type \"%s\" defined as a native type but not declared as native", qname.name);
        }
#endif
    }

    if (mp->flags & EJS_MODULE_BUILTIN) {
        attributes |= EJS_ATTR_BUILTIN;
    }
    if (attributes & EJS_ATTR_SLOTS_NEED_FIXUP) {
        baseType = 0;
        if (fixup == 0) {
            fixup = createFixup(ejs, (baseType) ? &baseType->qname : &ejs->objectType->qname, -1);
        }
    }
    
    mprLog(ejs, 9, "    Load %s class %s for module %s at slot %d", qname.space, qname.name, mp->name, slotNum);

    /*
     *  If the module is fully bound, then we install the type at the prescribed slot number.
     */
    if (slotNum < 0) {
        slotNum = ejs->globalBlock->obj.numProp;
    }
    
    if (type == 0) {
        attributes |= EJS_ATTR_OBJECT | EJS_ATTR_OBJECT_HELPERS;
        type = ejsCreateType(ejs, &qname, mp, baseType, sizeof(EjsObject), slotNum, numTypeProp, numInstanceProp, 
            attributes, 0);
        if (type == 0) {
            ejsThrowInternalError(ejs, "Can't create class %s", qname.name);
            return MPR_ERR_BAD_STATE;
        }

    } else {
        mp->hasNative = 1;
        if (!type->block.obj.var.native) {
            mprError(ejs, "WARNING: type not defined as native: \"%s\"", type->qname.name);
        }
    }
    
    /*
     *  Read implemented interfaces. Add to type->implements. Create fixup record if the interface type is not yet known.
     */
    if (numInterfaces > 0) {
        type->implements = mprCreateList(type);
        for (i = 0; i < numInterfaces; i++) {
            if (ejsModuleReadType(ejs, mp, &iface, &ifixup, &ifaceClassName, 0) < 0) {
                return MPR_ERR_CANT_READ;
            }
            if (iface) {
                mprAddItem(type->implements, iface);
            } else {
                if (addFixup(ejs, EJS_FIXUP_INTERFACE_TYPE, (EjsVar*) type, -1, ifixup) < 0) {
                    ejsThrowMemoryError(ejs);
                    return MPR_ERR_NO_MEMORY;
                }
            }
        }
    }

    if (mp->flags & EJS_MODULE_BUILTIN) {
        type->block.obj.var.builtin = 1;
    }
    if (attributes & EJS_ATTR_HAS_STATIC_INITIALIZER) {
        type->hasStaticInitializer = 1;
    }
    if (attributes & EJS_ATTR_DYNAMIC_INSTANCE) {
        type->block.dynamicInstance = 1;
    }

    slotNum = ejsDefineProperty(ejs, ejs->global, slotNum, &qname, ejs->typeType, attributes, (EjsVar*) type);
    if (slotNum < 0) {
        ejsThrowMemoryError(ejs);
        return MPR_ERR_NO_MEMORY;
    }
    type->module = mp;

    if (fixup) {
        if (addFixup(ejs, EJS_FIXUP_BASE_TYPE, (EjsVar*) type, -1, fixup) < 0) {
            ejsThrowMemoryError(ejs);
            return MPR_ERR_NO_MEMORY;
        }
        
    } else {
        if (ejs->flags & EJS_FLAG_EMPTY) {
            if (attributes & EJS_ATTR_NATIVE) {
                /*
                 *  When empty, native types are created with no properties and with numTraits equal to zero. 
                 *  This is so the compiler can compile the core ejs module. For ejsmod which may also run in 
                 *  empty mode, we set numInherited here to the correct value for native types.
                 */
                if (type->block.numInherited == 0 && type->baseType) {
                    type->block.numInherited = type->baseType->block.numTraits;
                }
            }
        }
    }

#if BLD_FEATURE_EJS_DOC
    setDoc(ejs, mp, ejs->global, slotNum);
#endif

    block = (EjsBlock*) type;
    block->scopeChain = mp->scopeChain;
    mp->scopeChain = block;

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_CLASS, mp, slotNum, qname, type, attributes);
    }
    return 0;
}


static int loadEndClassSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsType     *type;

    mprLog(ejs, 9, "    End class section");

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_CLASS_END, mp, mp->scopeChain);
    }
    type = (EjsType*) mp->scopeChain;
    if (type->block.hasScriptFunctions && type->baseType) {
        ejsDefineTypeNamespaces(ejs, type);
    }
    mp->scopeChain = mp->scopeChain->scopeChain;
    return 0;
}


static int loadFunctionSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsType         *returnType;
    EjsTypeFixup    *fixup;
    EjsFunction     *fun;
    EjsName         qname, returnTypeName;
    EjsBlock        *block;
    uchar           *code;
    int             slotNum, numArgs, codeLen, numLocals, numExceptions, attributes, nextSlot, lang;

    lang = 0;

    qname.name = ejsModuleReadString(ejs, mp);
    qname.space = ejsModuleReadString(ejs, mp);
    ejsModuleReadNumber(ejs, mp, &nextSlot);
    ejsModuleReadNumber(ejs, mp, &attributes);
    ejsModuleReadByte(ejs, mp, &lang);
 
    ejsModuleReadType(ejs, mp, &returnType, &fixup, &returnTypeName, 0);
    ejsModuleReadNumber(ejs, mp, &slotNum);
    ejsModuleReadNumber(ejs, mp, &numArgs);
    ejsModuleReadNumber(ejs, mp, &numLocals);
    ejsModuleReadNumber(ejs, mp, &numExceptions);
    ejsModuleReadNumber(ejs, mp, &codeLen);

    if (mp->hasError) {
        return MPR_ERR_CANT_READ;
    }

    block = (EjsBlock*) mp->scopeChain;
    mprAssert(block);
    mprAssert(numArgs >= 0 && numArgs < EJS_MAX_ARGS);
    mprAssert(numLocals >= 0 && numLocals < EJS_MAX_LOCALS);
    mprAssert(numExceptions >= 0 && numExceptions < EJS_MAX_EXCEPTIONS);

    mprLog(ejs, 9, "Loading function %s:%s at slot %d", qname.space, qname.name, slotNum);

    /*
     *  Read the code. We pass ownership of the code to createMethod i.e. don't free.
     */
    if (codeLen > 0) {
        code = (uchar*) mprAlloc(ejsGetAllocCtx(ejs), codeLen);
        if (code == 0) {
            return MPR_ERR_NO_MEMORY;
        }
        if (mprRead(file, code, codeLen) != codeLen) {
            mprFree(code);
            return MPR_ERR_CANT_READ;
        }
        block->hasScriptFunctions = 1;
    } else {
        code = 0;
    }

    if (attributes & EJS_ATTR_NATIVE) {
        mp->hasNative = 1;
    }
    if (attributes & EJS_ATTR_INITIALIZER) {
        mp->hasInitializer = 1;
    }
    if (mp->flags & EJS_MODULE_BUILTIN) {
        attributes |= EJS_ATTR_BUILTIN;
    }

    if (ejsLookupProperty(ejs, (EjsVar*) block, &qname) >= 0 && !(attributes & EJS_ATTR_OVERRIDE)) {
        if (ejsIsType(block)) {
            ejsThrowReferenceError(ejs,
                "function \"%s\" already defined in type \"%s\". Try adding \"override\" to the function declaration.", 
                qname.name, ((EjsType*) block)->qname.name);
        } else {
            ejsThrowReferenceError(ejs,
                "function \"%s\" already defined. Try adding \"override\" to the function declaration.", qname.name);
        }
        return MPR_ERR_CANT_CREATE;
    }

    /*
     *  Create the function using the current scope chain. Non-methods revise this scope chain via the 
     *  DefineFunction op code.
     */
    fun = ejsCreateFunction(ejs, code, codeLen, numArgs, numExceptions, returnType, attributes, mp->constants, 
        mp->scopeChain, lang);
    if (fun == 0) {
        mprFree(code);
        return MPR_ERR_NO_MEMORY;
    }
    if (code) {
        mprStealBlock(fun, code);
    }

    ejsSetDebugName(fun, qname.name);

    if (block == (EjsBlock*) ejs->global && slotNum < 0) {
        if (attributes & EJS_ATTR_OVERRIDE) {
            slotNum = ejsLookupProperty(ejs, (EjsVar*) block, &qname);
            if (slotNum < 0) {
                mprError(ejs, "Can't find method \"%s\" to override", qname.name);
                return MPR_ERR_NO_MEMORY;
            }

        } else {
            slotNum = -1;
        }
    }

    if (mp->flags & EJS_MODULE_BUILTIN) {
        fun->block.obj.var.builtin = 1;
    }

    if (attributes & EJS_ATTR_INITIALIZER && block == (EjsBlock*) ejs->global) {
        mp->initializer = fun;
        slotNum = -1;

    } else {
        slotNum = ejsDefineProperty(ejs, (EjsVar*) block, slotNum, &qname, ejs->functionType, attributes, (EjsVar*) fun);
        if (slotNum < 0) {
            return MPR_ERR_NO_MEMORY;
        }
    }
    
    ejsSetNextFunction(fun, nextSlot);

    if (fixup) {
        mprAssert(returnType == 0);
        if (addFixup(ejs, EJS_FIXUP_RETURN_TYPE, (EjsVar*) fun, -1, fixup) < 0) {
            ejsThrowMemoryError(ejs);
            return MPR_ERR_NO_MEMORY;
        }
    }

#if BLD_FEATURE_EJS_DOC
    setDoc(ejs, mp, (EjsVar*) block, slotNum);
#endif

    mp->currentMethod = fun;
    fun->block.scopeChain = mp->scopeChain;
    mp->scopeChain = &fun->block;
    fun->loading = 1;

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_FUNCTION, mp, block, slotNum, qname, fun, attributes);
    }
    return 0;
}


static int loadEndFunctionSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsTrait            *trait;
    EjsFunction         *fun;
    int                 i;

    mprLog(ejs, 9, "    End function section");

    fun = (EjsFunction*) mp->scopeChain;

    for (i = 0; i < (int) fun->numArgs; i++) {
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) fun, i);
        if (trait && trait->attributes & EJS_ATTR_DEFAULT) {
            fun->numDefault++;
        }
    }
    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_FUNCTION_END, mp, fun);
    }
    mp->scopeChain = mp->scopeChain->scopeChain;
    fun->loading = 0;

    return 0;
}


static int loadExceptionSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    EjsFunction         *fun;
    EjsType             *catchType;
    EjsTypeFixup        *fixup;
    EjsCode             *code;
    EjsEx               *ex;
    int                 tryStart, tryEnd, handlerStart, handlerEnd, numBlocks, numStack, flags, i;

    fun = mp->currentMethod;
    mprAssert(fun);

    flags = 0;
    code = &fun->body.code;

    for (i = 0; i < code->numHandlers; i++) {
        ejsModuleReadByte(ejs, mp, &flags);
        ejsModuleReadNumber(ejs, mp, &tryStart);
        ejsModuleReadNumber(ejs, mp, &tryEnd);
        ejsModuleReadNumber(ejs, mp, &handlerStart);
        ejsModuleReadNumber(ejs, mp, &handlerEnd);
        ejsModuleReadNumber(ejs, mp, &numBlocks);
        ejsModuleReadNumber(ejs, mp, &numStack);
        ejsModuleReadType(ejs, mp, &catchType, &fixup, 0, 0);

        if (mp->hasError) {
            return MPR_ERR_CANT_READ;
        }
        ex = ejsAddException(fun, tryStart, tryEnd, catchType, handlerStart, handlerEnd, numBlocks, numStack, flags, i);
        if (fixup) {
            mprAssert(catchType == 0);
            if (addFixup(ejs, EJS_FIXUP_EXCEPTION, (EjsVar*) ex, 0, fixup) < 0) {
                mprAssert(0);
                return MPR_ERR_NO_MEMORY;
            }
        }
    }
    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_EXCEPTION, mp, fun);
    }
    return 0;
}

/*
 *  Define a global, class or block property. Not used for function locals or args.
 */
static int loadPropertySection(Ejs *ejs, MprFile *file, EjsModule *mp, int sectionType)
{
    EjsType         *type;
    EjsTypeFixup    *fixup;
    EjsName         qname, propTypeName;
    EjsVar          *block, *value;
    cchar           *str;
    int             slotNum, attributes, fixupKind;

    value = 0;
    block = (EjsVar*) mp->scopeChain;
    mprAssert(block);
    
    qname.name = ejsModuleReadString(ejs, mp);
    qname.space = ejsModuleReadString(ejs, mp);
    
    ejsModuleReadNumber(ejs, mp, &attributes);
    ejsModuleReadNumber(ejs, mp, &slotNum);
    ejsModuleReadType(ejs, mp, &type, &fixup, &propTypeName, 0);

    if (attributes & EJS_ATTR_HAS_VALUE) {
        if ((str = ejsModuleReadString(ejs, mp)) == 0) {
            return MPR_ERR_CANT_READ;
        }
        /*  Only doing for namespaces currently */
        value = (EjsVar*) ejsCreateNamespace(ejs, str, str);
    }

    mprLog(ejs, 9, "Loading property %s:%s at slot %d", qname.space, qname.name, slotNum);

    if (attributes & EJS_ATTR_NATIVE) {
        mp->hasNative = 1;
    }
    if (mp->flags & EJS_MODULE_BUILTIN) {
        attributes |= EJS_ATTR_BUILTIN;
    }

    if (ejsLookupProperty(ejs, block, &qname) >= 0) {
        ejsThrowReferenceError(ejs, "property \"%s\" already loaded", qname.name);
        return MPR_ERR_CANT_CREATE;
    }

    if (ejsIsFunction(block)) {
        fixupKind = EJS_FIXUP_LOCAL;

    } else if (ejsIsType(block) && !(attributes & EJS_ATTR_STATIC) && block != ejs->global) {
        mprAssert(((EjsType*) block)->instanceBlock);
        block = (EjsVar*) ((EjsType*) block)->instanceBlock;
        fixupKind = EJS_FIXUP_INSTANCE_PROPERTY;

    } else {
        fixupKind = EJS_FIXUP_TYPE_PROPERTY;
    }

    slotNum = ejsDefineProperty(ejs, block, slotNum, &qname, type, attributes, value);
    if (slotNum < 0) {
        return MPR_ERR_CANT_WRITE;
    }

    if (fixup) {
        mprAssert(type == 0);
        if (addFixup(ejs, fixupKind, block, slotNum, fixup) < 0) {
            ejsThrowMemoryError(ejs);
            return MPR_ERR_NO_MEMORY;
        }
    }

#if BLD_FEATURE_EJS_DOC
    setDoc(ejs, mp, block, slotNum);
#endif

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_PROPERTY, mp, block, slotNum, qname, attributes, propTypeName);
    }
    return 0;
}


#if BLD_FEATURE_EJS_DOC
static int loadDocSection(Ejs *ejs, MprFile *file, EjsModule *mp)
{
    char        *doc;

    mprLog(ejs, 9, "    Documentation section");

    doc = ejsModuleReadString(ejs, mp);

    if (ejs->flags & EJS_FLAG_DOC) {
        mp->doc = doc;
        if (ejs->loaderCallback) {
            (ejs->loaderCallback)(ejs, EJS_SECT_DOC, doc);
        }
    }
    return 0;
}
#endif



#if !BLD_FEATURE_STATIC
/*
 *  Check if a native module exists at the given path. If so, load it. If the path is a scripted module
 *  but has a corresponding native module, then load that. Return 1 if loaded, -1 for errors, 0 if no
 *  native module found.
 */
static int loadNativeLibrary(Ejs *ejs, EjsModule *mp, cchar *dir)
{
    char    path[MPR_MAX_PATH], initName[MPR_MAX_PATH], moduleName[MPR_MAX_PATH], *cp;

    if (ejs->flags & EJS_FLAG_NO_EXE) {
        return 0;
    }

    mprSprintf(path, sizeof(path), "%s/%s%s", dir, mp->name, BLD_SHOBJ);
    if (! mprPathExists(ejs, path, R_OK)) {
        mprError(ejs, "Native library not found %s", path);
        return 0;
    }

    /*
     *  Build the DSO entry point name. Format is "NameModuleInit" where Name has "." converted to "_"
     */
    mprStrcpy(moduleName, sizeof(moduleName), mp->name);
    moduleName[0] = tolower((int) moduleName[0]);
    mprSprintf(initName, sizeof(initName), "%sModuleInit", moduleName);
    for (cp = initName; *cp; cp++) {
        if (*cp == '.') {
            *cp = '_';
        }
    }
    if (mprLookupModule(ejs, mp->name) != 0) {
        mprLog(ejs, 1, "Native module \"%s\" is already loaded", path);
        return 0;
    }
    mprLog(ejs, 4, "Loading native library %s", path);
    if (mprLoadModule(ejs, path, initName) == 0) {
        return MPR_ERR_CANT_OPEN;
    }
    return 1;
}
#endif


/*
 *  Load a scripted module file. Return a modified list of modules.
 */
static int loadScriptModule(Ejs *ejs, MprFile *file, cchar *path, int flags)
{
    EjsModuleHdr    hdr;
    int             status;

    mprAssert(path);

    /*
     *  Read module file header
     */
    if ((mprRead(file, &hdr, sizeof(hdr))) != sizeof(hdr)) {
        ejsThrowIOError(ejs, "Error reading module file %s, corrupt header", path);
        return EJS_ERR;
    }
    if ((int) swapWord(ejs, hdr.magic) != EJS_MODULE_MAGIC) {
        ejsThrowIOError(ejs, "Bad module file format in %s", path);
        return EJS_ERR;
    }
    if ((int) swapWord(ejs, hdr.fileVersion) != EJS_MODULE_VERSION) {
        ejsThrowIOError(ejs, "Incompatible module file format in %s", path);
        return EJS_ERR;
    }

    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_START, path, &hdr);
    }

    /*
     *  Load the sections: classes, properties and functions. This may load multiple modules.
     */
    if ((status = loadSections(ejs, file, &hdr, flags)) < 0) {
        if (ejs->exception == 0) {
            ejsThrowReferenceError(ejs, "Can't load module file %s", path);
        }
        return status;
    }
    if (ejs->loaderCallback) {
        (ejs->loaderCallback)(ejs, EJS_SECT_END, ejs->loadState->modules, 0);
    }
    return 0;
}


static int fixupTypes(Ejs *ejs, MprList *list)
{
    EjsTypeFixup    *fixup;
    EjsModule       *mp;
    EjsType         *type, *targetType;
    EjsBlock        *instanceBlock;
    EjsTrait        *trait;
    EjsFunction     *targetFunction;
    EjsEx           *targetException;
    int             next;

    for (next = 0; (fixup = (EjsTypeFixup*) mprGetNextItem(list, &next)) != 0; ) {
        mp = 0;
        type = 0;
        if (fixup->typeSlotNum >= 0) {
            type = (EjsType*) ejsGetProperty(ejs, ejs->global, fixup->typeSlotNum);

        } else if (fixup->typeName.name) {
            mprAssert(fixup->typeSlotNum < 0);
            type = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, &fixup->typeName);
            
        } else {
            continue;
        }
        if (type == 0) {
            ejsThrowReferenceError(ejs, "Can't fixup forward type reference for %s. Fixup kind %d", 
                fixup->typeName.name, fixup->kind);
            mprError(ejs, "Can't fixup forward type reference for %s. Fixup kind %d", fixup->typeName.name, fixup->kind);
            return EJS_ERR;
        }

        switch (fixup->kind) {
        case EJS_FIXUP_BASE_TYPE:
            mprAssert(fixup->target);
            targetType = (EjsType*) fixup->target;
            targetType->needFixup = 1;
            ejsFixupClass(ejs, targetType, type, targetType->implements, 0);
            instanceBlock = targetType->instanceBlock;
            if (instanceBlock && type) {
                ejsFixupBlock(ejs, instanceBlock, type->instanceBlock, targetType->implements, 0);
            }
            if (targetType->block.namespaces.length == 0 && type->block.hasScriptFunctions) {
                ejsDefineTypeNamespaces(ejs, targetType);
            }
            break;

        case EJS_FIXUP_INTERFACE_TYPE:
            targetType = (EjsType*) fixup->target;
            mprAddItem(targetType->implements, type);
            break;

        case EJS_FIXUP_RETURN_TYPE:
            mprAssert(fixup->target);
            targetFunction = (EjsFunction*) fixup->target;
            targetFunction->resultType = type;
            break;

        case EJS_FIXUP_TYPE_PROPERTY:
            mprAssert(fixup->target);
            trait = ejsGetPropertyTrait(ejs, fixup->target, fixup->slotNum);
            mprAssert(trait);
            if (trait) {
                trait->type = type;
            }
            break;

        case EJS_FIXUP_INSTANCE_PROPERTY:
            mprAssert(fixup->target);
            mprAssert(ejsIsBlock(fixup->target));
            mprAssert(fixup->target->isInstanceBlock);
            trait = ejsGetPropertyTrait(ejs, fixup->target, fixup->slotNum);
            mprAssert(trait);
            if (trait) {
                trait->type = type;
            }
            break;

        case EJS_FIXUP_LOCAL:
            mprAssert(fixup->target);
            trait = ejsGetPropertyTrait(ejs, fixup->target, fixup->slotNum);
            mprAssert(trait);
            if (trait) {
                trait->type = type;
            }
            break;

        case EJS_FIXUP_EXCEPTION:
            mprAssert(fixup->target);
            targetException = (EjsEx*) fixup->target;
            targetException->catchType = type;
            break;

        default:
            mprAssert(0);
        }
    }
    return 0;
}


/*
 *  Search for a file. If found, Return the path where the file was located. Otherwise return null.
 */
static char *probe(Ejs *ejs, cchar *path, int minVersion, int maxVersion)
{
    MprDirEntry     *dp, *best;
    MprList         *files;
    char            *dir, *base, *vp, *tok, *ext, *result;
    int             nameLen, version, next, major, minor, patch, bestVersion;

    mprAssert(ejs);
    mprAssert(path);

    mprLog(ejs, 7, "Probe for file %s", path);

    if (maxVersion == 0) {
        if (mprPathExists(ejs, path, R_OK)) {
            return mprStrdup(ejs, path);
        }
        return 0;
    }

    dir = mprGetPathDir(ejs, path);
    base = mprGetPathBase(ejs, path);
    if ((ext = strrchr(base, '.')) != 0) {
        *ext = '\0';
    }
    files = mprGetPathFiles(ejs, dir, 0);
    nameLen = (int) strlen(base);
    bestVersion = -1;
    best = 0;

    for (next = 0; (dp = mprGetNextItem(files, &next)) != 0; ) {
        if (strncmp(dp->name, base, nameLen) != 0) {
            continue;
        }
        if ((ext = strrchr(dp->name, '.')) == 0 || strcmp(ext, EJS_MODULE_EXT) != 0) {
            continue;
        }
        if (dp->name[nameLen] == '-') {
            vp = &dp->name[nameLen + 1];
            minor = patch = 0;
            major = (int) mprAtoi(vp, 10);
            if ((tok = strchr(vp, '.')) != 0) {
                minor = (int) mprAtoi(++tok, 10);
            }
            if ((tok = strchr(vp, '.')) != 0) {
                patch = (int) mprAtoi(++tok, 10);
            }
            version = EJS_MAKE_VERSION(major, minor, patch);
        } else if (&dp->name[nameLen] == ext) {
            version = 0;
        } else {
            continue;
        }
        if (minVersion <= version && version <= maxVersion) {
            if (best == 0 || bestVersion < version) {
                bestVersion = version;
                best = dp;
            }
        }
    }
    if (best == 0) {
        result = 0;
    } else {
        result = mprJoinPath(ejs, dir, best->name);
    }
    mprFree(files);
    return result;
}


/*
 *  Search for a module. 
 *  The search strategy is: Given a name "a.b.c", scan for:
 *
 *      1. File named a.b.c
 *      2. File named a/b/c
 *      3. File named a.b.c in EJSPATH
 *      4. File named a/b/c in EJSPATH
 *      5. File named c in EJSPATH
 */
char *ejsSearchForModule(Ejs *ejs, cchar *nameArg, int minVersion, int maxVersion)
{
    MprCtx  ctx;
    char    *path, *ejsPath, *fileName, *baseName, *searchPath, *dir, *tok, *cp, *slashName, *name;

    slashName = 0;
    ejsPath = ejs->ejsPath;
    mprAssert(ejsPath);

    if (maxVersion <= 0) {
        maxVersion = MAXINT;
    }
    ctx = name = mprGetNormalizedPath(ejs, nameArg);

    mprLog(ejs, 5, "Search for module \"%s\" in ejspath %s", name, ejsPath);

    /*
     *  1. Search for path directly
     */
    if ((path = probe(ejs, name, minVersion, maxVersion)) != 0) {
        mprLog(ejs, 5, "Found %s at %s", name, path);
        mprFree(name);
        return path;
    }

    /*
     *  2. Search for "a/b/c"
     */
    slashName = mprStrdup(ejs, name);
    for (cp = slashName; *cp; cp++) {
        if (*cp == '.') {
            *cp = mprGetPathSeparator(ejs, name);
        }
    }
    if ((path = probe(ejs, slashName, minVersion, maxVersion)) != 0) {
        mprLog(ejs, 5, "Found %s at %s", name, path);
        mprFree(name);
        return path;
    }

    /*
     *  3. Search for "a.b.c" in EJSPATH
     */
    searchPath = mprStrdup(ejs, ejsPath);
    dir = mprStrTok(searchPath, MPR_SEARCH_SEP, &tok);
    while (dir && *dir) {
        fileName = mprStrcat(ctx, -1, dir, "/", name, NULL);
        if ((path = probe(ejs, fileName, minVersion, maxVersion)) != 0) {
            mprLog(ejs, 5, "Found %s at %s", name, path);
            mprFree(ctx);
            return path;
        }
        dir = mprStrTok(NULL, MPR_SEARCH_SEP, &tok);
    }
    mprFree(searchPath);

    /*
     *  4. Search for "a/b/c" in EJSPATH
     */
    searchPath = mprStrdup(ejs, ejsPath);
    dir = mprStrTok(searchPath, MPR_SEARCH_SEP, &tok);
    while (dir && *dir) {
        fileName = mprStrcat(ctx, -1, dir, "/", slashName, NULL);
        if ((path = probe(ejs, fileName, minVersion, maxVersion)) != 0) {
            mprLog(ejs, 5, "Found %s at %s", name, path);
            mprFree(ctx);
            return path;
        }
        dir = mprStrTok(NULL, MPR_SEARCH_SEP, &tok);
    }
    mprFree(searchPath);

    /*
     *  5. Search for "c" in EJSPATH
     */
    baseName = mprGetPathBase(ctx, slashName);
    searchPath = mprStrdup(ctx, ejsPath);
    dir = mprStrTok(searchPath, MPR_SEARCH_SEP, &tok);
    while (dir && *dir) {
        fileName = mprStrcat(ctx, -1, dir, "/", baseName, NULL);
        if ((path = probe(ejs, fileName, minVersion, maxVersion)) != 0) {
            mprLog(ejs, 5, "Found %s at %s", name, path);
            mprFree(ctx);
            return path;
        }
        dir = mprStrTok(NULL, MPR_SEARCH_SEP, &tok);
    }
    mprFree(ctx);
    return 0;
}


/*
 *  Read a string constant. String constants are stored as token offsets into
 *  the constant pool. The pool contains null terminated UTF-8 strings.
 */
char *ejsModuleReadString(Ejs *ejs, EjsModule *mp)
{
    int     t;

    mprAssert(mp);

    if (ejsModuleReadNumber(ejs, mp, &t) < 0) {
        return 0;
    }
    return tokenToString(mp, t);
}


/*
 *  Read a type reference. Types are stored as either global property slot numbers or as strings (token offsets into the 
 *  constant pool). The lowest bit is set if the reference is a string. The type and name arguments are optional and may 
 *  be set to null. Return EJS_ERR for errors, otherwise 0. Return the 0 if successful, otherwise return EJS_ERR. If the 
 *  type could not be resolved, allocate a fixup record and return in *fixup. The caller should then call addFixup.
 */
int ejsModuleReadType(Ejs *ejs, EjsModule *mp, EjsType **typeRef, EjsTypeFixup **fixup, EjsName *typeName, int *slotNum)
{
    EjsType         *type;
    EjsName         qname;
    int             t, slot;

    mprAssert(mp);
    mprAssert(typeRef);
    mprAssert(fixup);

    *typeRef = 0;
    *fixup = 0;

    if (typeName) {
        typeName->name = 0;
        typeName->space = 0;
    }

    if (ejsModuleReadNumber(ejs, mp, &t) < 0) {
        mprAssert(0);
        return EJS_ERR;
    }

    slot = -1;
    qname.name = 0;
    qname.space = 0;
    type = 0;

    switch (t & EJS_ENCODE_GLOBAL_MASK) {
    default:
        mp->hasError = 1;
        mprAssert(0);
        return EJS_ERR;

    case EJS_ENCODE_GLOBAL_NOREF:
        return 0;

    case EJS_ENCODE_GLOBAL_SLOT:
        /*
         *  Type is a builtin primitive type or we are binding globals.
         */
        slot = t >> 2;
        if (0 <= slot && slot < ejsGetPropertyCount(ejs, ejs->global)) {
            type = (EjsType*) ejsGetProperty(ejs, ejs->global, slot);
            if (type) {
                qname = type->qname;
            }
        }
        break;

    case EJS_ENCODE_GLOBAL_NAME:
        /*
         *  Type was unbound at compile time
         */
        qname.name = tokenToString(mp, t >> 2);
        if (qname.name == 0) {
            mp->hasError = 1;
            mprAssert(0);
            return EJS_ERR;
        }
        if ((qname.space = ejsModuleReadString(ejs, mp)) == 0) {
            mp->hasError = 1;
            mprAssert(0);
            return EJS_ERR;
        }
        if (qname.name) {
            slot = ejsLookupProperty(ejs, ejs->global, &qname);
            if (slot >= 0) {
                type = (EjsType*) ejsGetProperty(ejs, ejs->global, slot);
            }
        }
        break;
    }

    if (type) {
        if (!ejsIsType(type)) {
            mp->hasError = 1;
            mprAssert(0);
            return EJS_ERR;
        }
        *typeRef = type;

    } else if (type == 0 && fixup) {
        *fixup = createFixup(ejs, &qname, slot);
    }

    if (typeName) {
        *typeName = qname;
    }
    if (slotNum) {
        *slotNum = slot;
    }

    return 0;
}


static EjsTypeFixup *createFixup(Ejs *ejs, EjsName *qname, int slotNum)
{
    EjsTypeFixup    *fixup;

    mprAssert(ejs->loadState->typeFixups);

    fixup = mprAllocZeroed(ejs->loadState->typeFixups, sizeof(EjsTypeFixup));
    if (fixup == 0) {
        return 0;
    }
    fixup->typeName = *qname;
    fixup->typeSlotNum = slotNum;
    return fixup;
}


static int addFixup(Ejs *ejs, int kind, EjsVar *target, int slotNum, EjsTypeFixup *fixup)
{
    int     index;

    mprAssert(ejs);
    mprAssert(fixup);
    mprAssert(ejs->loadState->typeFixups);

    fixup->kind = kind;
    fixup->target = target;
    fixup->slotNum = slotNum;

    index = mprAddItem(ejs->loadState->typeFixups, fixup);
    if (index < 0) {
        mprAssert(0);
        return EJS_ERR;
    }
    return 0;
}


/*
 *  Convert a token index into a string.
 */
static char *tokenToString(EjsModule *mp, int token)
{
    if (token < 0 || token >= mp->constants->len) {
        mprAssert(0);
        return 0;
    }
    mprAssert(mp->constants);
    if (mp->constants == 0) {
        mprAssert(0);
        return 0;
    }
    return &mp->constants->pool[token];
}


/*
 *  Decode an encoded 32-bit word
 */
int ejsDecodeWord(uchar **pp)
{
    uchar   *start;
    int     value;

    start = *pp;
    value = (int) ejsDecodeNum(pp);
    *pp = start + 4;
    return value;
}


/*
 *  Get an encoded 64 bit number. Variable number of bytes.
 */
int64 ejsDecodeNum(uchar **pp)
{
    uchar   *pos;
    uint64  t;
    uint    c;
    int     sign, shift;

    pos = *pp;
    c = (uint) *pos++;

    /*
     *  Map sign bit (0,1) to 1,-1
     */
    sign = 1 - ((c & 0x1) << 1);
    t = (c >> 1) & 0x3f;
    shift = 6;

    while (c & 0x80) {
        c = *pos++;
        t |= (c & 0x7f) << shift;
        shift += 7;
    }
    *pp = pos;
    return t * sign;
}


/*
 *  Decode a 4 byte number from a file
 */
static int readWord(Ejs *ejs, MprFile *file, int *number)
{
    uchar   buf[4], *pp;

    mprAssert(file);
    mprAssert(number);

    if (mprRead(file, buf, 4) != 4) {
        return MPR_ERR_CANT_READ;
    }
    pp = buf;
    *number = ejsDecodeWord(&pp);
    return 0;
}


/*
 *  Decode a number from a file. Same as ejsDecodeNum but reading from a file.
 */
static int readNumber(Ejs *ejs, MprFile *file, int *number)
{
    uint    t;
    int     c, sign, shift;

    mprAssert(file);
    mprAssert(number);

    if ((c = mprGetc(file)) < 0) {
        return MPR_ERR_CANT_READ;
    }

    /*
     *  Map sign bit (0,1) to 1,-1
     */
    sign = 1 - ((c & 0x1) << 1);
    t = (c >> 1) & 0x3f;
    shift = 6;
    
    while (c & 0x80) {
        if ((c = mprGetc(file)) < 0) {
            return MPR_ERR_CANT_READ;
        }
        t |= (c & 0x7f) << shift;
        shift += 7;
    }
    *number = (int) (t * sign);
    return 0;
}


#if BLD_FEATURE_FLOATING_POINT
double ejsDecodeDouble(Ejs *ejs, uchar **pp)
{
    double   value;

    memcpy(&value, *pp, sizeof(double));
    value = swapDoubleWord(ejs, value);
    *pp += sizeof(double);
    return value;
}
#endif


/*
 *  Encode a number in a RLL encoding. Encoding is:
 *      Bit     0:  Sign
 *      Bits  1-6:  Low 6 bits (0-64)
 *      Bit     7:  Extension bit
 *      Bits 8-15:  Next 7 bits
 *      Bits   16:  Extension bit
 *      ...
 */
int ejsEncodeNum(uchar *pos, int64 number)
{
    uchar       *start;
    uint        encoded;

    mprAssert(pos);

    start = pos;
    if (number < 0) {
        number = -number;
        encoded = (uint) (((number & 0x3F) << 1) | 1);
    } else {
        encoded = (uint) (((number & 0x3F) << 1));
    }
    number >>= 6;

    while (number) {
        *pos++ = encoded | 0x80;
        encoded = (int) (number & 0x7f);
        number >>= 7;
    }
    *pos++ = encoded;
    mprAssert((pos - start) < 11);
    return (int) (pos - start);
}


int ejsEncodeUint(uchar *pos, int number)
{
    uchar       *start;
    uint        encoded;

    mprAssert(pos);

    start = pos;
    encoded = (uint) (((number & 0x3F) << 1));
    number >>= 6;

    while (number) {
        *pos++ = encoded | 0x80;
        encoded = (int) (number & 0x7f);
        number >>= 7;
    }
    *pos++ = encoded;
    mprAssert((pos - start) < 11);
    return (int) (pos - start);
}


/*
 *  Encode a 32-bit number. Always emit exactly 4 bytes.
 */
int ejsEncodeWord(uchar *pos, int number)
{
    int         len;

    mprAssert(pos);

    if (abs(number) > EJS_ENCODE_MAX_WORD) {
        mprError(mprGetMpr(NULL), "Code generation error. Word %d exceeds maximum %d", number, EJS_ENCODE_MAX_WORD);
        return 0;
    }
    len = ejsEncodeNum(pos, (int64) number);
    mprAssert(len <= 4);
    return 4;
}


int ejsEncodeDouble(Ejs *ejs, uchar *pos, double number)
{
    number = swapDoubleWord(ejs, number);
    memcpy(pos, &number, sizeof(double));
    return sizeof(double);
}


int ejsEncodeByteAtPos(uchar *pos, int value)
{
    mprAssert(pos);

    *pos = value;
    return 0;
}


int ejsEncodeWordAtPos(uchar *pos, int value)
{
    mprAssert(pos);

    return ejsEncodeWord(pos, value);
}



/*
 *  Read an encoded number. Numbers are little-endian encoded in 7 bits with
 *  the 0x80 bit of each byte being a continuation bit.
 */
int ejsModuleReadNumber(Ejs *ejs, EjsModule *mp, int *number)
{
    mprAssert(ejs);
    mprAssert(mp);
    mprAssert(number);

    if (readNumber(ejs, mp->file, number) < 0) {
        mp->hasError = 1;
        return -1;
    }
    return 0;
}


int ejsModuleReadByte(Ejs *ejs, EjsModule *mp, int *number)
{
    int     c;

    mprAssert(mp);
    mprAssert(number);

    if ((c = mprGetc(mp->file)) < 0) {
        mp->hasError = 1;
        return MPR_ERR_CANT_READ;
    }
    *number = c;
    return 0;
}


#if BLD_FEATURE_EJS_DOC
static void setDoc(Ejs *ejs, EjsModule *mp, EjsVar *block, int slotNum)
{
    if (mp->doc && ejsIsBlock(block)) {
        ejsCreateDoc(ejs, (EjsBlock*) block, slotNum, mp->doc);
        mp->doc = 0;
    }
}


EjsDoc *ejsCreateDoc(Ejs *ejs, EjsBlock *block, int slotNum, cchar *docString)
{
    EjsDoc      *doc;
    char        key[32];

    doc = mprAllocZeroed(ejs, sizeof(EjsDoc));
    if (doc == 0) {
        return 0;
    }
    doc->docString = mprStrdup(doc, docString);
    if (ejs->doc == 0) {
        ejs->doc = mprCreateHash(ejs, EJS_DOC_HASH_SIZE);
    }

    /*
     *  This is slow, but not critical path
     */
    mprSprintf(key, sizeof(key), "%Lx %d", PTOL(block), slotNum);
    mprAddHash(ejs->doc, key, doc);
    return doc;
}
#endif


#if UNUSED && KEEP
static int swapShort(Ejs *ejs, int word)
{
    if (mprGetEndian(ejs) == MPR_LITTLE_ENDIAN) {
        return word;
    }
    word = ((word & 0xFFFF) << 16) | ((word & 0xFFFF0000) >> 16);
    return ((word & 0xFF) << 8) | ((word & 0xFF00) >> 8);
}
#endif

static int swapWord(Ejs *ejs, int word)
{
    if (mprGetEndian(ejs) == MPR_LITTLE_ENDIAN) {
        return word;
    }
    return ((word & 0xFF000000) >> 24) | ((word & 0xFF0000) >> 8) | ((word & 0xFF00) << 8) | ((word & 0xFF) << 24);
}


static double swapDoubleWord(Ejs *ejs, double a)
{
    int64   low, high;

    if (mprGetEndian(ejs) == MPR_LITTLE_ENDIAN) {
        return a;
    }
    low = ((int64) a) & 0xFFFFFFFF;
    high = (((int64) a) >> 32) & 0xFFFFFFFF;
    return  (double) ((low & 0xFF) << 24 | (low & 0xFF00 << 8) | (low & 0xFF0000 >> 8) | (low & 0xFF000000 >> 16) |
            ((high & 0xFF) << 24 | (high & 0xFF00 << 8) | (high & 0xFF0000 >> 8) | (high & 0xFF000000 >> 16)) << 32);
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
    vim: sw=4 ts=4 expandtab

    @end
 */
