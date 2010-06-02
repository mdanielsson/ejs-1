/**
 *  ejsmod.c - Module manager 
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejsmod.h"

/****************************** Forward Declarations **************************/

static int  process(EjsMod *mp, cchar *output, int argc, char **argv);
static void logger(MprCtx ctx, int flags, int level, const char *msg);
static int  setLogging(Mpr *mpr, char *logSpec);

/************************************ Code ************************************/

MAIN(ejsmodMain, int argc, char **argv)
{
    Mpr             *mpr;
    EjsMod          *mp;
    EjsService      *vmService;
    Ejs             *ejs;
    char            *argp, *searchPath, *output;
    int             nextArg, err, flags;

    err = 0;
    output = searchPath = 0;
    
    /*
     *  Create the Embedthis Portable Runtime (MPR) and setup a memory failure handler
     */
    mpr = mprCreate(argc, argv, ejsMemoryFailure);
    mprSetAppName(mpr, argv[0], 0, 0);

    /*
     *  Allocate the primary control structure
     */
    mp = mprAllocObjZeroed(mpr, EjsMod);
    if (mp == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    mp->lstRecords = mprCreateList(mp);
    mp->blocks = mprCreateList(mp);
    mp->docDir = ".";
    
    for (nextArg = 1; nextArg < argc; nextArg++) {
        argp = argv[nextArg];
        if (*argp != '-') {
            break;
        }
        if (strcmp(argp, "--cslots") == 0) {
            mp->cslots = 1;
            mp->genSlots = 1;
            
        } else if (strcmp(argp, "--empty") == 0) {
            mp->empty = 1;
            
        } else if (strcmp(argp, "--error") == 0) {
            /*
             *  Undocumented switch
             */
            mp->exitOnError++;
            mp->warnOnError++;
            
        } else if (strcmp(argp, "--jslots") == 0) {
            /*
             *  This command is currently not documented until the JVM is released.
             */
            mp->jslots = 1;
            mp->genSlots = 1;

        } else if (strcmp(argp, "--html") == 0) {
#if BLD_FEATURE_EJS_DOC
            if (nextArg >= argc) {
                err++;
            } else {
                mp->docDir = argv[++nextArg];
                mp->html = 1;
            }
#else
            mprPrintfError(mpr, "Doc generation is not enabled. Reconfigure with --enable-doc");
            err++;
#endif
            
        } else if (strcmp(argp, "--listing") == 0) {
            mp->listing = 1;
            
        } else if (strcmp(argp, "--log") == 0) {
            /*
             *  Undocumented switch
             */
            if (nextArg >= argc) {
                err++;
            } else {
                setLogging(mpr, argv[++nextArg]);
            }

        } else if (strcmp(argp, "--out") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                output = argv[++nextArg];
                mp->cslots = 1;
                mp->genSlots = 1;
            }

        } else if (strcmp(argp, "--search") == 0 || strcmp(argp, "--searchpath") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                searchPath = argv[++nextArg];
            }

        } else if (strcmp(argp, "--showDebug") == 0) {
            mp->showDebug++;

        } else if (strcmp(argp, "--showBuiltin") == 0) {
            mp->showBuiltin = 1;
            
        } else if (strcmp(argp, "--version") == 0 || strcmp(argp, "-V") == 0) {
            mprPrintfError(mpr, "%s %s\n"
                "Copyright (C) Embedthis Software 2003-2010\n"
                "Copyright (C) Michael O'Brien 2003-2010\n",
               BLD_NAME, BLD_VERSION);  
            exit(0);

        } else if (strcmp(argp, "--warn") == 0) {
            /*
             *  Undocumented switch
             */
            mp->warnOnError++;

        } else if (strcmp(argp, "--xml") == 0) {
#if BLD_FEATURE_EJS_DOC
            mp->xml = 1;
#else
            mprPrintfError(mpr, "Doc generation is not enabled. Reconfigure with --enable-doc");
            err++;
#endif
        
        } else {
            err++;
            break;
        }
    }
    
    if (argc == nextArg) {
        err++;
    }
    
    if (mp->genSlots == 0 && mp->listing == 0 && mp->html == 0 && mp->xml == 0) {
        mp->listing = 1;
    }

    if (err) {
        /*
         *  Examples:
         *      ejsmod file.mod                              # Defaults to --listing
         *      ejsmod --listing embedthis.mod 
         *      ejsmod --out slots.h embedthis.mod 
         */
        mprPrintfError(mpr, 
            "Usage: %s [options] modules ...\n"
            "  Ejscript module manager options:\n"
            "  --cslots              # Generate a C slot definitions file\n"
            "  --empty               # Create empty interpreter\n"
            "  --html dir            # Generate HTML documentation to the specified directory\n"
            "  --listing             # Create assembler listing files (default)\n"
            "  --out                 # Output file for all C slots (implies --cslots)\n"
            "  --search ejsPath      # Module file search path\n"
            "  --showBuiltin         # Show builtin properties\n"
            "  --showDebug           # Show debug instructions\n"
            "  --version             # Emit the program version information\n\n", mpr->name);
        return -1;
    }

    /*
     *  Need an interpreter to load modules
     */
    vmService = ejsCreateService(mpr); 
    if (vmService == 0) {
        return MPR_ERR_NO_MEMORY;
    }

    flags = EJS_FLAG_COMPILER | EJS_FLAG_NO_EXE;
    if (mp->empty) {
        flags |= EJS_FLAG_EMPTY;
    }
    if (mp->html || mp->xml) {
        flags |= EJS_FLAG_DOC;
    }
    ejs = ejsCreate(vmService, NULL, searchPath, flags);
    if (ejs == 0) {
        return MPR_ERR_NO_MEMORY;
    }
    mp->ejs = ejs;

    if (nextArg < argc) {
        /*
         *  Process the module files supplied on the command line.
         */
        if (process(mp, output, argc - nextArg, &argv[nextArg]) < 0) {
            err++;
        }
    }
    if (mp->errorCount > 0) {
        err = -1;
    }
#if VXWORKS
    mprFree(ejs);
    if (mprStop(mpr)) {
        mprFree(mpr);
    }
#endif
    return err;
}


static int process(EjsMod *mp, cchar *output, int argc, char **argv)
{
    MprList     *modules, *allModules;
    EjsModule   *module;
    MprFile     *outfile;
    void        (*callback)(struct Ejs *ejs, int kind, ...);
    int         i, next, builtinGlobals;

    callback = (mp->listing) ? emListingLoadCallback : 0;
    allModules = mprCreateList(mp);
    builtinGlobals = ejsGetPropertyCount(mp->ejs, mp->ejs->global);
    
    if (output) {
        outfile = mprOpen(mp, output, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 0664);
    } else {
        outfile = 0;
    }

    /*
     *  For each module on the command line
     */
    for (i = 0; i < argc && !mp->fatalError; i++) {
        mp->ejs->userData = mp;
        module = 0;
        if (!mprPathExists(mp, argv[i], R_OK)) {
            mprError(mp, "Can't access module %s", argv[i]);
            return EJS_ERR;
        }
        mp->ejs->loaderCallback = callback;
        if ((ejsLoadModule(mp->ejs, argv[i], -1, -1, EJS_MODULE_DONT_INIT, &modules)) < 0) {
            mp->ejs->loaderCallback = NULL;
            mprError(mp, "Can't load module %s\n%s", argv[i], ejsGetErrorMsg(mp->ejs, 0));
            return EJS_ERR;
        }
        mp->ejs->loaderCallback = NULL;
        
        if (mp->genSlots) {
            /*
             *  Create the slot files for all modules loaded
             */
            for (next = 0; (module = mprGetNextItem(modules, &next)) != 0; ) {
                emCreateSlotFiles(mp, module, outfile, builtinGlobals);
            }
        }
        mprAppendList(allModules, modules);
        mprFree(modules);
    }
        
#if BLD_FEATURE_EJS_DOC
    if (mp->html || mp->xml) {
        emCreateDoc(mp);
    }
#endif
    mprFree(outfile);
    return 0;
}



static int setLogging(Mpr *mpr, char *logSpec)
{
    MprFile     *file;
    char        *levelSpec;
    int         level;

    level = 0;

    if ((levelSpec = strchr(logSpec, ':')) != 0) {
        *levelSpec++ = '\0';
        level = atoi(levelSpec);
    }

    if (strcmp(logSpec, "stdout") == 0) {
        file = mpr->fileSystem->stdOutput;
    } else {
        if ((file = mprOpen(mpr, logSpec, O_WRONLY, 0664)) == 0) {
            mprPrintfError(mpr, "Can't open log file %s\n", logSpec);
            return EJS_ERR;
        }
    }

    mprSetLogLevel(mpr, level);
    mprSetLogHandler(mpr, logger, (void*) file);

    return 0;
}



static void logger(MprCtx ctx, int flags, int level, const char *msg)
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
            mprStaticPrintf(file, "%s: Error: %s\n", prefix, msg);
        } else {
            mprFprintf(file, "%s: Error: %s\n", prefix, msg);
        }

    } else if (flags & MPR_FATAL_SRC) {
        mprFprintf(file, "%s: Fatal: %s\n", prefix, msg);
        
    } else if (flags & MPR_RAW) {
        mprFprintf(file, "%s", msg);
    }
    
    if (flags & (MPR_ERROR_SRC | MPR_FATAL_SRC | MPR_ASSERT_SRC)) {
        mprBreakpoint();
    }
}


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
