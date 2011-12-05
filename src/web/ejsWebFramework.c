/*
 *  ejsWebFramework.c -- Ejscript web framework processing.
 *
 *  Ejscript provides an MVC paradigm for efficiently creating dynamic applications using server-side Javascript.
 *
 *  Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "ejs.h"

#if BLD_FEATURE_EJS_WEB

/*********************************** Locals ***********************************/
/*
 *  Singleton instance for the Web server control structure
 */
static EjsWebControl *webControl;

/***************************** Forward Declarations ***************************/
/*
 *  Support auto recompilation if cross compiler is enabled or building native
 */
#if (BLD_FEATURE_EJS_CROSS_COMPILER || !BUILDING_CROSS) && BLD_FEATURE_EJS_AUTO_COMPILE
    #define AUTO_COMPILE 1
#else
    #define AUTO_COMPILE 0
#endif

#if AUTO_COMPILE
static int  compile(EjsWeb *web, cchar *shell, cchar *kind, cchar *name);
static char *locateShell(EjsWeb *web);
#endif

static int caselessmatch(cchar *url, cchar *ext);
static void createCookie(Ejs *ejs, EjsVar *cookies, cchar *name, cchar *value, cchar *domain, cchar *path);
static int  initInterp(Ejs *ejs, EjsWebControl *control);
static int  loadApplication(EjsWeb *web);
static int  loadController(EjsWeb *web);
static int  loadComponent(EjsWeb *web, cchar *kind, cchar *name, cchar *sourceExtension);
static int  build(EjsWeb *web, cchar *kind, cchar *name, cchar *module, cchar *sourceExtension, int force);
static int  parseControllerAction(EjsWeb *web);

/************************************ Code ************************************/
/*
 *  Create and configure web framework types
 */
void ejsConfigureWebTypes(Ejs *ejs)
{
    ejsConfigureWebRequestType(ejs);
    ejsConfigureWebResponseType(ejs);
    ejsConfigureWebHostType(ejs);
    ejsConfigureWebControllerType(ejs);
    ejsConfigureWebSessionType(ejs);
    ejsConfigureWebCacheType(ejs);
}


/*
 *  Loadable module interface. Called when loaded from a shared library.
 */
MprModule *ejs_webModuleInit(Ejs *ejs, cchar *path)
{
    MprModule   *module;
    int         priorGen;

    module = mprCreateModule(ejs, "ejsWeb", BLD_VERSION, 0, 0, 0);
    if (module == 0) {
        return 0;
    }
    priorGen = ejsSetGeneration(ejs, EJS_GEN_ETERNAL);
    ejsConfigureWebTypes(ejs);
    ejsSetGeneration(ejs, priorGen);
    return module;
}


/*
 *  Called once by the web server handler when it it loaded.
 */
int ejsOpenWebFramework(EjsWebControl *control, bool useMaster)
{
    mprAssert(control);

    /*
     *  Create the Ejscript service
     */
    control->service = ejsCreateService(control);
    if (control->service == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    if (useMaster) {
        /*
         *  Create the master interpreter
         */
        control->master = ejsCreate(control->service, NULL, control->searchPath, EJS_FLAG_MASTER);
        if (control->master == 0) {
            mprFree(control->service);
            return MPR_ERR_NO_MEMORY;
        }
        if (initInterp(control->master, control) < 0) {
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    webControl = control;
    return 0;
}


static int initInterp(Ejs *ejs, EjsWebControl *control)
{
    EjsVar      *sessions;

#if !BLD_FEATURE_STATIC
    if (ejsLoadModule(ejs, "ejs.web", -1, -1, 0, NULL) < 0) {
        mprError(control, "Can't load ejs.web.mod: %s", ejsGetErrorMsg(ejs, 1));
        return MPR_ERR_CANT_INITIALIZE;
    }
#if BLD_FEATURE_EJS_DB 
    if (ejsLoadModule(ejs, "ejs.db", -1, -1, 0, NULL) < 0) {
        mprError(control, "Can't load ejs.db.mod: %s", ejsGetErrorMsg(ejs, 1));
        return MPR_ERR_CANT_INITIALIZE;
    }
#endif
#if BLD_FEATURE_SQLITE 
    if (ejsLoadModule(ejs, "ejs.db.sqlite", -1, 0, 0, NULL) < 0) {
        mprError(control, "Can't load ejs.db.sqlite.mod: %s", ejsGetErrorMsg(ejs, 1));
        return MPR_ERR_CANT_INITIALIZE;
    }
#endif
#endif
    control->sessionTimeout = EJS_SESSION_TIMEOUT;
#if ES_ejs_web_sessions
    sessions = ejsGetProperty(ejs, ejs->global, ES_ejs_web_sessions);
#else
{
    EjsName qname;
    sessions = ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.web", "sessions"));
}
#endif
    ejs->sessions = sessions;
    control->sessions = (EjsObject*) sessions;
    ejs->dontExit = 1;
    return 0;
}


/*
 *  Return the web server master interpreter. If not using a master interpreter, it returns the current interp.
 */
Ejs *ejsGetMaster(MprCtx ctx)
{
    return (Ejs*) webControl->master;
}


/*
 *  Initialize a new web request structure. 
 *  Given request       "/carmen/admin/list/1?a=b&c=d", then the args would be:
 *      baseUrl         "/carmen"
 *      url             "/admin/list/1"
 *      query           "a=b&c=d"
 *      baseDir         "/path/carmen"
 */
EjsWeb *ejsCreateWebRequest(MprCtx ctx, EjsWebControl *control, void *handle, cchar *baseUrl, cchar *url,
        cchar *baseDir, cchar* searchPath, int flags)
{
    Ejs             *ejs;
    EjsWeb          *web;
    cchar           *appUrl;

    web = (EjsWeb*) mprAllocObjZeroed(ctx, EjsWeb);
    if (web == 0) {
        return 0;
    }
    if (flags & EJS_WEB_FLAG_APP) {
        appUrl = baseUrl;
    } else {
        appUrl = 0;
        flags |= EJS_WEB_FLAG_SOLO;
    }

    web->appDir = mprStrdup(web, baseDir);
    mprStrTrim((char*) web->appDir, "/");
    web->appUrl = appUrl;
    web->url = url;
    web->flags = flags;
    web->handle = handle;
    web->control = control;

    mprLog(ctx, 3, "ejs: CreateWebRequest: AppDir %s, AppUrl %s, URL %s", web->appDir, web->appUrl, web->url);

    if (control->master) {
        ejs = web->ejs = ejsCreate(ctx, control->master, searchPath, 0);
        ejs->master = control->master;
    } else {
        ejs = web->ejs = ejsCreate(ctx, NULL, searchPath, 0);
        if (ejs && initInterp(ejs, control) < 0) {
            mprFree(web);
            return 0;
        }
    }
    if (ejs == 0) {
        mprFree(web);
        return 0;
    }
    ejsSetHandle(ejs, web);
    return web;
}


/*
 *  Parse the request URI and create the controller and action names. URI is in the form: "controller/action"
 */
static int parseControllerAction(EjsWeb *web)
{
    cchar   *url;
    char    *cp, *controllerName;

    if (web->flags & EJS_WEB_FLAG_SOLO || caselessmatch(web->url, EJS_WEB_EXT)) {
        if (web->flags & EJS_WEB_FLAG_SOLO) {
            ejsName(&web->controllerName, "ejs.web", "_SoloController");
        } else {
            ejsName(&web->controllerName, EJS_PUBLIC_NAMESPACE, "BaseController");
        }
        ejsName(&web->doActionName, "ejs.web", "renderView");

        /*
         *  View name strips extension and converts "/" to "_"
         */
        web->viewName = mprStrdup(web, &web->url[1]);
        if ((cp = strchr(web->viewName, '.')) != 0) {
            *cp = '\0';
        }
        for (cp = web->viewName; *cp; cp++) {
            if (*cp == '/') {
                *cp = '_';
            }
        }
        return 0;
    }

    /*
     *  Request as part of an Ejscript application (not stand-alone)
     */
    for (url = web->url; *url == '/'; url++) {
        ;
    }
    controllerName = mprStrdup(web, url);
    controllerName[0] = toupper((int) controllerName[0]);
    mprStrTrim(controllerName, "/");

    web->viewName = "";
    if ((cp = strchr(controllerName, '/')) != 0) {
        *cp++ = '\0';
        web->viewName = cp;
        if ((cp = strchr(cp, '/')) != 0) {
            *cp++ = '\0';
        }
    }
    if (*controllerName == '\0') {
        controllerName = "Base";
    }
    if (*web->viewName == '\0') {
        web->viewName = "index";
    }

    cp = mprStrcat(web, -1, controllerName, "Controller", NULL);
    ejsName(&web->controllerName, EJS_PUBLIC_NAMESPACE, cp);
    web->controllerFile = controllerName;
    ejsName(&web->doActionName, "ejs.web", "doAction");
    return 0;
}


static int loadApplication(EjsWeb *web)
{
    return loadComponent(web, "app", "App", ".es");
}


/*
 *  Load the controller module
 */
static int loadController(EjsWeb *web)
{
    return loadComponent(web, "controller", web->controllerFile, ".es");
}


static int createController(EjsWeb *web)
{
    Ejs         *ejs;
    EjsVar      *host, *request, *response, *argv[16];
    EjsType     *type;
    int         oldGen, slotNum;

    ejs = web->ejs;

    if (web->flags & EJS_WEB_FLAG_APP) {
        if (loadApplication(web) < 0) {
            return MPR_ERR_NOT_FOUND;
        }
    }

    web->controllerType = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, &web->controllerName);
    if (web->controllerType == 0 || !ejsIsType(web->controllerType)) {
        if (web->controllerFile && loadController(web) < 0) {
            web->error = mprAsprintf(web, -1, "Can't load controller %s", web->controllerName.name);
            return MPR_ERR_NOT_FOUND;
        }
        web->controllerType = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, &web->controllerName);
        if (web->controllerType == 0 || !ejsIsType(web->controllerType)) {
            web->error = mprAsprintf(web, -1, "Can't find controller class %s", web->controllerName.name);
            return MPR_ERR_NOT_FOUND;
        }
    }
    oldGen = ejsSetGeneration(ejs, EJS_GEN_ETERNAL);

    if ((web->cookie = (char*) ejsGetHeader(ejs, "HTTP_COOKIE")) != 0) {
        ejsParseWebSessionCookie(web);
    }

    if (web->flags & EJS_WEB_FLAG_SESSION && web->session == 0) {
        web->session = ejsCreateSession(ejs, 0, 0);
    }

    /*
     *  Create the Host, Request and Response objects. These are virtual objects that lazily create their properties.
     */
    host =      (EjsVar*) ejsCreateWebHostObject(ejs, web->handle);
    response =  (EjsVar*) ejsCreateWebResponseObject(ejs, web->handle);
    request =   (EjsVar*) ejsCreateWebRequestObject(ejs, web->handle);

    argv[0] = (web->flags & EJS_WEB_FLAG_SOLO) ? (EjsVar*) ejs->falseValue : (EjsVar*) ejs->trueValue;
    argv[1] = (EjsVar*) ejsCreateString(ejs, web->appDir);
    argv[2] = (EjsVar*) ejsCreateString(ejs, web->appUrl);
    argv[3] = (web->session) ? (EjsVar*) web->session: (EjsVar*) ejs->nullValue;
    argv[4] = host;
    argv[5] = request;
    argv[6] = response;

    web->controller = (EjsVar*) ejsCreateObject(ejs, web->controllerType, 0);
    if (web->controller == 0) {
        web->error = "Memory allocation failure";
        return MPR_ERR_NO_MEMORY;
    }

    ejsRunFunctionBySlot(ejs, web->controller, ES_ejs_web_Controller_ejs_web_initialize, 7, argv);

    type = (EjsType*) web->controllerType;
    if (type->hasConstructor) {
        slotNum = type->block.numInherited;
        ejsRunFunctionBySlot(ejs, web->controller, slotNum, 0, NULL);
    }

    web->params = ejsGetProperty(ejs, web->controller, ES_ejs_web_Controller_params);
    ejsDefineParams(ejs);
    ejsSetGeneration(ejs, oldGen);
    return 0;
}


static int getDoAction(EjsWeb *web)
{
    Ejs     *ejs;

    ejs = web->ejs;
    web->doAction = ejsGetPropertyByName(ejs, (EjsVar*) web->controllerType, &web->doActionName);
    if (web->doAction == 0 || !ejsIsFunction(web->doAction)) {
        web->error = mprAsprintf(web, -1, "Internal error: Can't find function %s::%s", web->doActionName.space,
                web->doActionName.name);
        return EJS_ERR;
    }
    return 0;
}


/*
 *  Run a web request
 */
int ejsRunWebRequest(EjsWeb *web)
{
    Ejs             *ejs;
    EjsVar          *result, *argv[1];

    ejs = web->ejs;

    /*
     *  Parse the url and extract the controller and action name
     */
    if (parseControllerAction(web) < 0) {
        web->error = mprAsprintf(web, -1, "URL is not in the right form: \"%s\"", web->url);
        return MPR_ERR_BAD_ARGS;
    }
    if (createController(web) < 0) {
        return MPR_ERR_CANT_CREATE;
    }
    if (getDoAction(web) < 0) {
        return MPR_ERR_CANT_CREATE;
    }

    argv[0] = (EjsVar*) ejsCreateString(ejs, web->viewName);
    result = ejsRunFunction(ejs, (EjsFunction*) web->doAction, web->controller, 1, argv);
    if (result == 0 && ejs->exception) {
        web->error = ejsGetErrorMsg(ejs, 1);
        return MPR_ERR_CANT_COMPLETE;
    }
    return 0;
}


int ejsLoadView(Ejs *ejs)
{
    EjsWeb      *web;
    char        *name, *cp;
    int         rc;

    mprAssert(ejs);
    web = ejs->handle;

    if (!(web->flags & EJS_WEB_FLAG_SOLO) && !caselessmatch(web->url, EJS_WEB_EXT)) {
        /*
         *  Normal views/...
         */
        name = mprJoinPath(ejs, web->controllerFile, web->viewName);
        rc = loadComponent(web, "view", name, EJS_WEB_EXT);
        mprFree(name);
        return rc;

    }
    name = mprStrdup(web, &web->url[1]);
    if ((cp = strrchr(name, '.')) && mprStrcmpAnyCase(cp, EJS_WEB_EXT) == 0) {
        *cp = '\0';
    }
    rc = loadComponent(web, "", name, EJS_WEB_EXT);
    mprFree(name);
    return rc;
}


/*
 *  Load a module corresponding to a source component. If the source is newer, then recompile the component. Build 
 *  the module from web->appdir and name depending on the kind. Kind will be "app", "controller", "view" or "".
 */
static int loadComponent(EjsWeb *web, cchar *kind, cchar *name, cchar *sourceExtension)
{
    Ejs         *ejs;
    char        *module, *pluralKind, *soloPage;
    int         rc, retry, loaded;

    ejs = web->ejs;
    module = 0;
    rc = 0;

    for (loaded = retry = 0; retry < 2 && !loaded; retry++) {
        if (strcmp(kind, "app") == 0) {
            module = mprJoinPath(web, web->appDir, "App" EJS_MODULE_EXT);
            rc = build(web, kind, NULL, module, sourceExtension, retry);

        } else if (*kind) {
            /* Note we pluralize the kind (e.g. view to views) */
            pluralKind = mprStrcat(web, -1, kind, "s", NULL);
            module = mprJoinPathExt(web, mprJoinPath(web, mprJoinPath(web, web->appDir, pluralKind), name), EJS_MODULE_EXT);
            rc = build(web, kind, name, module, sourceExtension, retry);
            mprFree(pluralKind);

        } else {
            /*
             *  Solo web pages
             */
            module = mprJoinPathExt(web, mprJoinPath(web, web->appDir, name), EJS_MODULE_EXT);
            soloPage = mprJoinPathExt(web, mprGetNativePath(web, name), sourceExtension);
            rc = build(web, kind, soloPage, module, sourceExtension, retry);
            mprFree(soloPage);
        }
        if (rc < 0) {
            break;
        }

        if ((rc = ejsLoadModule(web->ejs, module, -1, 0, 0, NULL)) < 0) {
            mprLog(web, 5, "Loading module %s failed. Status %d. %s", module, rc, (retry == 0) ? "Retrying" : "");
            if (retry > 0) {
                web->error = mprAsprintf(web, -1, "Can't load module : \"%s\"\n%s", module, ejsGetErrorMsg(ejs, 1));
                break;
            }
            ejsClearException(web->ejs);
        } else {
            loaded = 1;
        }
    }
    mprFree(module);
    return rc;
}


#if AUTO_COMPILE
/*
 *  Find the ejs program
 */
static char *locateShell(EjsWeb *web)
{
    return mprSearchPath(web, EJS_EJS_EXE, MPR_SEARCH_EXE, 
        mprGetAppDir(web),      //  Search in same dir as application (or override module path) (Windows)
#if BLD_DEBUG && !WIN
        BLD_ABS_BIN_DIR,
#endif
        BLD_BIN_PREFIX,         //  Search the standard binary install directory
#if !WIN
        BLD_ABS_BIN_DIR, 
#endif
        NULL); //  Search the local dev bin
}


/*
 *  Compile a component into a loadable module. Return true if the compile succeeded.
 */
static int compile(EjsWeb *web, cchar *shell, cchar *kind, cchar *name)
{
    Ejs         *ejs;
    MprCmd      *cmd;
    char        *commandLine, *err, *dir, *ejsweb;
    int         status;

    cmd = mprCreateCmd(web);
    mprSetCmdDir(cmd, web->appDir);

    dir = mprGetPathDir(web, shell);
#if WIN || DEBUG_IDE
    ejsweb = mprJoinPath(web, dir, EJS_EJSWEB EJS_MODULE_EXT);
#else
    ejsweb = mprJoinPath(web, dir, "../modules/" EJS_EJSWEB EJS_MODULE_EXT);
#endif

    if (strcmp(kind, "view") == 0) {
        name = mprJoinPath(cmd, "views", name);
    }
    if (strcmp(kind, "app") == 0) {
        commandLine = mprAsprintf(web, -1, "\"%s\" \"%s\" --quiet compile app", shell, ejsweb);
    } else {
        commandLine = mprAsprintf(web, -1, "\"%s\" \"%s\" --quiet compile %s \"%s\"", shell, ejsweb, kind, name);
    }
    ejs = (web->ejs->master) ? web->ejs->master : web->ejs;

    /*
     *  Must be thread-safe so that multiple threads don't try to compile the same file
     */
    lock(ejs);
    mprLog(web, 3, "ejs cmd: cd %s; %s", web->appDir, commandLine);
    status = mprRunCmd(cmd, commandLine, NULL, &err, 0);
    if (status) {
        web->error = mprStrdup(web, err);
        mprLog(web, 3, "Compilation failure for %s\n%s", commandLine, err);
    }
    unlock(ejs);
    mprFree(cmd);
    return status;
}
#endif /* AUTO_COMPILE */


/*
 *  Build a resource.
 *  Path has a ".mod" extension.
 */
static int build(EjsWeb *web, cchar *kind, cchar *name, cchar *module, cchar *sourceExtension, int force)
{
    MprPath     moduleInfo;
    int         rc;

    rc = 0;
    mprGetPathInfo(web, module, &moduleInfo);

#if AUTO_COMPILE
{
    MprPath     sourceInfo;
    char        *source, *shell;

    if (strcmp(kind, "app") == 0) {
        source = mprJoinPath(web, web->appDir, "src/App.es");
    } else {
        source = mprJoinPathExt(web, mprTrimPathExtension(web, module), sourceExtension);
    }
    mprGetPathInfo(web, source, &sourceInfo);

    if (!moduleInfo.valid && !sourceInfo.valid) {
        mprLog(web, 3, "Can't find module %s", module);
        web->error = mprAsprintf(web, -1, "Can't find module: \"%s\"", module);
        rc = MPR_ERR_NOT_FOUND;

    } else if (moduleInfo.valid && !sourceInfo.valid) {
        /* No source to rebuild */
        mprLog(web, 5, "Using module %s - missing source file", module);

    } else if (moduleInfo.valid && sourceInfo.valid && sourceInfo.mtime <= moduleInfo.mtime && !force) {
        /* Up to date already */
        mprLog(web, 5, "Using module %s - is up to date", module);

    } else {
        /* Either module out of date or not present with source present or forced rebuild */
        if ((shell = locateShell(web)) == 0) {
            if (!moduleInfo.valid) {
                mprError(web, "Can't find shell: %s to compile %s", EJS_EJSWEB_EXE, source);
                return MPR_ERR_CANT_ACCESS;
            }
            mprLog(web, 5, "Using module %s - missing shell", module);
            /* Use existing module even though out of date -- no shell */

        } else if (compile(web, shell, kind, name) != 0) {
            rc = MPR_ERR_BAD_STATE;
        }
    }
    mprFree(source);
}
#else
    if (!moduleInfo.valid) {
        mprLog(web, 3, "Can't find module %s", module);
        rc = MPR_ERR_NOT_FOUND;
    }
#endif
    return rc;
}


/*
 *  This routine parses the cookie header to search for a session cookie.
 *  There may be multiple cookies where the most qualified path come first
 */
EjsVar *ejsCreateCookies(Ejs *ejs)
{
    EjsWeb      *web;
    cchar       *domain, *path, *cookieName, *cookieValue;
    char        *cookieString, *value, *tok, *key, *dp, *sp;
    int         seenSemi;

    web = ejs->handle;
    if (web->cookies) {
        return web->cookies;
    }
    web->cookies = (EjsVar*) ejsCreateSimpleObject(ejs);
    cookieString = mprStrdup(web, web->cookie);
    key = cookieString;
    cookieName = cookieValue = domain = path = 0;

    while (*key) {
        while (*key && isspace((int) *key)) {
            key++;
        }
        tok = key;
        while (*tok && !isspace((int) *tok) && *tok != ';' && *tok != '=') {
            tok++;
        }
        if (*tok) {
            *tok++ = '\0';
        }
        while (isspace((int) *tok)) {
            tok++;
        }

        seenSemi = 0;
        if (*tok == '\"') {
            value = ++tok;
            while (*tok != '\"' && *tok != '\0') {
                tok++;
            }
            if (*tok) {
                *tok++ = '\0';
            }

        } else {
            value = tok;
            while (*tok != ';' && *tok != '\0') {
                tok++;
            }
            if (*tok) {
                seenSemi++;
                *tok++ = '\0';
            }
        }

        /*
         *  Handle back-quoting in value
         */
        if (strchr(value, '\\')) {
            for (dp = sp = value; *sp; sp++) {
                if (*sp == '\\') {
                    sp++;
                }
                *dp++ = *sp++;
            }
            *dp = '\0';
        }

        /*
         *  Example:
         *  $Version="1"; NAME="value"; $Path="PATH"; $Domain="DOMAIN"; NAME="value"; $Path="PATH"; $Domain="DOMAIN"; 
         */
        if (*key == '$') {
            key++;
            switch (tolower((int) *key)) {
            case 'd':
                if (mprStrcmpAnyCase(key, "domain") == 0) {
                    domain = value;
                }
                break;

            case 'p':
                if (mprStrcmpAnyCase(key, "path") == 0) {
                    path = value;
                }
                break;

            case 'v':
#if UNUSED
                if (mprStrcmpAnyCase(key, "version") == 0) {
                    version = value;
                }
#endif
                break;
            default:
                break;
            }
            
        } else {
            if (cookieName) {
                createCookie(ejs, web->cookies, cookieName, cookieValue, domain, path);
                cookieName = cookieValue = path = domain = 0;
            }
            cookieName = key;
            cookieValue = value;
        }

        key = tok;
        if (!seenSemi) {
            while (*key && *key != ';') {
                key++;
            }
            if (*key) {
                key++;
            }
        }
    }
    if (cookieName) {
        createCookie(ejs, web->cookies, cookieName, cookieValue, domain, path);
    }
    mprFree(cookieString);
    return web->cookies;
}


static EjsVar *createString(Ejs *ejs, cchar *value)
{
    return value ? (EjsVar*) ejsCreateString(ejs, value) : ejs->nullValue;
}


static void createCookie(Ejs *ejs, EjsVar *cookies, cchar *name, cchar *value, cchar *domain, cchar *path)
{
    EjsType     *cookieType;
    EjsName     qname;
    EjsVar      *cookie;
    
#if ES_ejs_web_Cookie
    cookieType = ejsGetType(ejs, ES_ejs_web_Cookie);
#else
    cookieType = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, ejsName(&qname, "ejs.web", "Cookie"));
#endif
    mprAssert(cookieType);
    cookie = (EjsVar*) ejsCreateObject(ejs, cookieType, 0);
    ejsSetProperty(ejs, cookie, ES_ejs_web_Cookie_name, createString(ejs, name));
    ejsSetProperty(ejs, cookie, ES_ejs_web_Cookie_value, createString(ejs, value));
    ejsSetProperty(ejs, cookie, ES_ejs_web_Cookie_path, createString(ejs, path));
    ejsSetProperty(ejs, cookie, ES_ejs_web_Cookie_domain, createString(ejs, domain));
    ejsName(&qname, "", mprStrdup(cookie, name));
    ejsSetPropertyByName(ejs, cookies, &qname, (EjsVar*) cookie);
}


/*
 *  Define a form variable as an ejs property in the params[] collection. Support a.b.c syntax
 */
void ejsDefineWebParam(Ejs *ejs, cchar *key, cchar *svalue)
{
    EjsName     qname;
    EjsWeb      *web;
    EjsVar      *where, *vp, *value;
    char        *subkey, *end;
    int         slotNum;

    web = ejsGetHandle(ejs);

    where = web->params;
    mprAssert(where);

    if (*svalue == '[') {
        value = ejsDeserialize(ejs, ejsCreateString(ejs, svalue));
    } else {
        value = (EjsVar*) ejsCreateString(ejs, svalue);
    }

    /*
     *  name.name.name
     */
    if (strchr(key, '.') == 0) {
        ejsName(&qname, "", key);
        ejsSetPropertyByName(ejs, where, &qname, value);

    } else {
        subkey = mprStrdup(ejs, key);
        for (end = strchr(subkey, '.'); end; subkey = end, end = strchr(subkey, '.')) {
            *end++ = '\0';
            ejsName(&qname, "", subkey);
            vp = ejsGetPropertyByName(ejs, where, &qname);
            if (vp == 0) {
                slotNum = ejsSetPropertyByName(ejs, where, &qname, (EjsVar*) ejsCreateObject(ejs, ejs->objectType, 0));
                vp = ejsGetProperty(ejs, where, slotNum);
            }
            where = vp;
        }
        mprAssert(where);
        ejsName(&qname, "", subkey);
        ejsSetPropertyByName(ejs, where, &qname, value);
    }
}


static int caselessmatch(cchar *url, cchar *ext)
{
    cchar   *cp, *ep;

    for (cp = url; *cp; cp++) {
        for (ep = ext; *ep; ep++) {
            if (tolower((int) *cp) != tolower((int) *ep)) {
                break;
            }
        }
        if (*ep == '\0') {
            return 1;
        }
    }
    return 0;
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
