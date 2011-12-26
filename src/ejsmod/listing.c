/**
 *  listing.c - Assembler listing generator.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejsmod.h"
#include    "ejsByteCodeTable.h"

/***************************** Forward Declarations ****************************/

static cchar *getBlockName(EjsMod *mp, EjsVar *block, int slotNum);
static uchar getByte(EjsMod *mp);
static uint  getWord(EjsMod *mp);
#if BLD_FEATURE_FLOATING_POINT
static double getDouble(EjsMod *mp);
#endif
static int64 getNum(EjsMod *dp);
static char *getString(EjsMod *dp);
static void getGlobal(EjsMod *mp, char *buf, int buflen);
static void lstSlotAssignments(EjsMod *mp, EjsModule *module, EjsVar *parent, int slotNum, EjsVar *obj);
static char *getAttributeString(EjsMod *mp, int attributes);
static void interp(EjsMod *mp, EjsModule *module, EjsFunction *fun);
static void leadin(EjsMod *mp, EjsModule *module, int classDec, int inFunction);
static void lstBlock(EjsMod *mp, EjsModule *module, EjsVar *block, int slotNum, cchar *name, int numSlots);
static void lstClass(EjsMod *mp, EjsModule *module, int slotNum, EjsType *klass, int attributes);
static void lstClose(EjsMod *mp, MprList *modules);
static void lstDependency(EjsMod *mp, EjsModule *module, EjsModule *dependant);
static void lstEndModule(EjsMod *mp, EjsModule *module);
static void lstException(EjsMod *mp, EjsModule *module, EjsFunction *fun);
static void lstFunction(EjsMod *mp, EjsModule *module, EjsVar *block, int slotNum, EjsName qname, EjsFunction *fun, 
    int attributes);
static int  lstOpen(EjsMod *mp, char *moduleFilename, EjsModuleHdr *hdr);
static void lstProperty(EjsMod *mp, EjsModule *module, EjsVar *block, int slotNum, EjsName qname, int attributes, 
    EjsName typeName);
static void lstModule(EjsMod *mp, EjsModule *module);

/*********************************** Code *************************************/
/*
 *  Listing loader callback. This is invoked at key points when loading a module file.
 */
void emListingLoadCallback(Ejs *ejs, int kind, ...)
{
    va_list         args;
    EjsModuleHdr    *hdr;
    EjsMod          *mp;
    Lst             *lst;
    MprList         *modules;
    char            *name;

    va_start(args, kind);

    mp = ejs->userData;

    lst = mprAllocObjZeroed(mp, Lst);

    /*
     *  Decode the record type and create a list for later processing. We need to process
     *  after the loader has done fixup for forward type references.
     */
    switch (kind) {

    case EJS_SECT_BLOCK:
        lst->module = va_arg(args, EjsModule*);
        lst->owner = va_arg(args, EjsVar*);
        lst->slotNum = va_arg(args, int);
        lst->name = va_arg(args, char*);
        lst->numSlots = va_arg(args, int);
        break;

    case EJS_SECT_BLOCK_END:
        break;

    case EJS_SECT_CLASS:
        lst->module = va_arg(args, EjsModule*);
        lst->slotNum = va_arg(args, int);
        lst->qname = va_arg(args, EjsName);
        lst->type = va_arg(args, EjsType*);
        lst->attributes = va_arg(args, int);
        break;

    case EJS_SECT_CLASS_END:
        break;

    case EJS_SECT_DEPENDENCY:
        lst->module = va_arg(args, EjsModule*);
        lst->dependency = va_arg(args, EjsModule*);
        break;

    case EJS_SECT_END:
        modules = va_arg(args, MprList*);
        lstClose(mp, modules);
        mprFree(lst);
        return;

    case EJS_SECT_EXCEPTION:
        lst->module = va_arg(args, EjsModule*);
        lst->fun = va_arg(args, EjsFunction*);
        break;

    case EJS_SECT_FUNCTION:
        lst->module = va_arg(args, EjsModule*);
        lst->owner = va_arg(args, EjsVar*);
        lst->slotNum = va_arg(args, int);
        lst->qname = va_arg(args, EjsName);
        lst->fun = va_arg(args, EjsFunction*);
        lst->attributes = va_arg(args, int);
        break;

    case EJS_SECT_FUNCTION_END:
        break;

    case EJS_SECT_START:
        name = va_arg(args, char*);
        hdr = va_arg(args, EjsModuleHdr*);
        lstOpen(mp, name, hdr);
        mprFree(lst);
        return;

    case EJS_SECT_PROPERTY:
        lst->module = va_arg(args, EjsModule*);
        lst->owner = va_arg(args, EjsVar*);
        lst->slotNum = va_arg(args, int);
        lst->qname = va_arg(args, EjsName);
        lst->attributes = va_arg(args, int);
        lst->typeName = va_arg(args, EjsName);
        break;

    case EJS_SECT_MODULE:
        break;

    case EJS_SECT_MODULE_END:
        break;

    default:
        mprAssert(0);
    }

    lst->kind = kind;
    mprAddItem(mp->lstRecords, lst);
}


/*
 *  Loader completion routine. Process listing records and emit the listing file.
 */
static void lstClose(EjsMod *mp, MprList *modules)
{
    EjsModule   *module;
    Lst         *lst;
    bool        headerOutput;
    int         next, nextModule, count;

    for (nextModule = 0; (module = (EjsModule*) mprGetNextItem(modules, &nextModule)) != 0; ) {

        headerOutput = 0;
        count = 0;

        for (next = 0; (lst = (Lst*) mprGetNextItem(mp->lstRecords, &next)) != 0; ) {

            if (lst->module != module) {
                continue;
            }
            if (!headerOutput) {
                lstModule(mp, lst->module);
                headerOutput = 1;
            }

            switch (lst->kind) {
            case EJS_SECT_BLOCK:
                lstBlock(mp, lst->module, lst->owner, lst->slotNum, lst->name, lst->numSlots);
                count++;
                break;

            case EJS_SECT_CLASS:
                lstClass(mp, lst->module, lst->slotNum, lst->type, lst->attributes);
                count++;
                break;

            case EJS_SECT_DEPENDENCY:
                lstDependency(mp, lst->module, lst->dependency);
                break;

            case EJS_SECT_EXCEPTION:
                lstException(mp, lst->module, lst->fun);
                break;

            case EJS_SECT_FUNCTION:
                lstFunction(mp, lst->module, lst->owner, lst->slotNum, lst->qname, lst->fun, lst->attributes);
                count++;
                break;

            case EJS_SECT_PROPERTY:
                lstProperty(mp, lst->module, lst->owner, lst->slotNum, lst->qname, lst->attributes, lst->typeName);
                count++;
                break;

            default:
            case EJS_SECT_START:
            case EJS_SECT_END:
                mprAssert(0);
                break;
            }
        }

        if (count > 0) {
            lstEndModule(mp, module);
        }
    }
    mprFree(mp->file);
    mp->file = 0;
}


static int lstOpen(EjsMod *mp, char *moduleFilename, EjsModuleHdr *hdr)
{
    char    *path, *name, *ext;

    mprAssert(mp);

    name = mprGetPathBase(mp, moduleFilename);
    if ((ext = strstr(name, EJS_MODULE_EXT)) != 0) {
        *ext = '\0';
    }

    path = mprStrcat(mp, -1, name, EJS_LISTING_EXT, NULL);
    if ((mp->file = mprOpen(mp, path,  O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 0664)) == 0) {
        mprError(mp, "Can't create %s", path);
        mprFree(path);
        return EJS_ERR;
    }
    mprEnableFileBuffering(mp->file, 0, 0);
    mprFprintf(mp->file, "#\n#  %s -- Module Listing for %s\n#\n", path, moduleFilename);
    mprFree(path);
    return 0;
}


static void lstBlock(EjsMod *mp, EjsModule *module, EjsVar *owner, int slotNum, cchar *name, int numSlots)
{
    cchar       *blockName;

    mprFprintf(mp->file, "\n");
    blockName = getBlockName(mp, owner, slotNum);
    mprFprintf(mp->file, "BLOCK:      [%s-%02d]  %s (Slots %d)\n", blockName, slotNum, name, numSlots);
}


/*
 *  List a class (type)
 */
static void lstClass(EjsMod *mp, EjsModule *module, int slotNum, EjsType *klass, int attributes)
{
    Ejs         *ejs;

    ejs = mp->ejs;

    mprFprintf(mp->file, "\n");

    if (klass->baseType) {
        mprFprintf(mp->file, "CLASS:      %sclass %s extends %s\n", getAttributeString(mp, attributes), 
            klass->qname.name, klass->baseType->qname.name);
    } else {
        mprFprintf(mp->file, "CLASS:      %sclass %s\n", getAttributeString(mp, attributes), klass->qname.name);
    }

    leadin(mp, module, 1, 0);
    mprFprintf(mp->file, "        #  Class Details: %d class traits, %d instance traits, requested slot %d\n",
        ejsGetPropertyCount(ejs, (EjsVar*) klass),
        klass->instanceBlock ? ejsGetPropertyCount(ejs, (EjsVar*) klass->instanceBlock) : 0, 
        slotNum);
}


static void lstDependency(EjsMod *mp, EjsModule *module, EjsModule *dependant)
{
    leadin(mp, module, 0, 0);
    mprFprintf(mp->file, "DEPENDENCY: require %s (sum %d)\n\n", dependant->vname, dependant->checksum);
}


static void lstEndModule(EjsMod *mp, EjsModule *module)
{
    char        *pp;
    int         i, size;

    mprAssert(mp);

    mprFprintf(mp->file,
        "\n----------------------------------------------------------------------------------------------\n");

    lstSlotAssignments(mp, module, NULL, 0, mp->ejs->global);

    /*
     *  Dump the constant pool
     */
    size = module->constants->len;
    mprFprintf(mp->file,
        "\n----------------------------------------------------------------------------------------------\n"
        "#\n"
        "#  Constant Pool (size %d bytes)\n"
        "#\n", size);

    pp = module->constants->pool;
    for (i = 0; pp < &module->constants->pool[size]; i++) {
        mprFprintf(mp->file, "%04d   \"%s\"\n", i, pp);
        pp = strchr(pp, '\0') + 1;
    }
}


static void lstFunction(EjsMod *mp, EjsModule *module, EjsVar *block, int slotNum, EjsName qname, EjsFunction *fun, 
        int attributes)
{
    Ejs         *ejs;
    EjsTrait    *trait;
    EjsName     lname;
    EjsType     *resultType;
    cchar       *blockName;
    int         i, numLocals;

    ejs = mp->ejs;
    mprFprintf(mp->file,  "\nFUNCTION:   ");

    /*
     *  Do the function declaration
     */
    if (attributes) {
        if (slotNum < 0) {
            /* Special just for global initializers */
            mprFprintf(mp->file,  "[initializer]  %s %sfunction %s(", qname.space, getAttributeString(mp, attributes), 
                qname.name);
        } else {
            blockName = getBlockName(mp, block, slotNum);
            mprFprintf(mp->file,  "[%s-%02d]  %s %sfunction %s(", blockName, slotNum, qname.space,
                getAttributeString(mp, attributes), qname.name);
        }
    } else {
        blockName = getBlockName(mp, block, slotNum);
        mprFprintf(mp->file,  "[%s-%02d]  %s function %s(", blockName, slotNum, qname.space, qname.name);
    }

    for (i = 0; i < (int) fun->numArgs; ) {
        lname = ejsGetPropertyName(ejs, (EjsVar*) fun, i);
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) fun, i);
        if (trait->type) {
            mprFprintf(mp->file,  "%s: %s", lname.name, trait->type->qname.name);
        } else {
            mprFprintf(mp->file,  "%s", lname.name);
        }
        if (++i < (int) fun->numArgs) {
            mprFprintf(mp->file,  ", ");
        }
    }

    resultType = fun->resultType;
    mprFprintf(mp->file,  ") : %s\n", resultType ? resultType->qname.name : "void");
    mprFprintf(mp->file,  "\n");

    /*
     *  Repeat the args
     */
    for (i = 0; i < (int) fun->numArgs; i++) {
        lname = ejsGetPropertyName(ejs, (EjsVar*) fun, i);
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) fun, i);
        mprFprintf(mp->file,  "     ARG:   [arg-%02d]   %s %s", i, lname.space, lname.name);
        if (trait->type) {
            mprFprintf(mp->file,  " : %s", trait->type->qname.name);
        }
        mprFprintf(mp->file,  "\n");
    }

    numLocals = fun->block.obj.numProp - fun->numArgs;
    for (i = 0; i < numLocals; i++) {
        lname = ejsGetPropertyName(ejs, (EjsVar*) fun, i + fun->numArgs);
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) fun, i + fun->numArgs);
        mprFprintf(mp->file,  "   LOCAL:   [local-%02d] var %s", i + fun->numArgs, lname.name);
        if (trait->type) {
            mprFprintf(mp->file,  " : %s", trait->type->qname.name);
        }
        mprFprintf(mp->file,  "\n");
    }
    mprFprintf(mp->file,  "\n");
    interp(mp, module, fun);

    leadin(mp, module, 0, 0);
    mprFprintf(mp->file,  "\n");
}


void lstException(EjsMod *mp, EjsModule *module, EjsFunction *fun)
{
    EjsEx           *ex;
    EjsCode         *code;
    cchar           *exKind;
    int             i;

    code = &fun->body.code;

    if (code->numHandlers <= 0) {
        return;
    }

    mprFprintf(mp->file,
        "\n"
        "#\n"
        "#  Exception Section\n"
        "#    Kind     TryStart TryEnd  HandlerStart  HandlerEnd   CatchType\n"
        "#\n");

    for (i = 0; i < code->numHandlers; i++) {
        ex = code->handlers[i];

        if (ex->flags & EJS_EX_FINALLY) {
            exKind  = "finally";
        } else if (ex->flags & EJS_EX_ITERATION) {
            exKind  = "iteration";
        } else if (ex->flags & EJS_EX_CATCH) {
            exKind = "catch";
        } else {
            exKind = "unknown";
        }
        mprFprintf(mp->file,
            "%-3d %-10s %5d   %5d      %5d        %5d       %s\n",
            i, exKind, ex->tryStart, ex->tryEnd, ex->handlerStart, ex->handlerEnd,
            ex->catchType ? (ex->catchType->qname.name) : "");
    }
    mprFprintf(mp->file, "\n");
}


static void lstProperty(EjsMod *mp, EjsModule *module, EjsVar *block, int slotNum, EjsName qname, int attributes, 
        EjsName typeName)
{
    Ejs         *ejs;
    EjsType     *propType;
    cchar       *blockName;

    ejs = mp->ejs;
    mprFprintf(mp->file, "VARIABLE:   ");

    blockName = getBlockName(mp, block, slotNum);
    mprFprintf(mp->file, "[%s-%02d]  %s %svar %s", blockName, slotNum, qname.space,
        getAttributeString(mp, attributes), qname.name);

    if (typeName.name && typeName.name[0]) {
        mprFprintf(mp->file, " : %s", typeName.name);
    }
    mprFprintf(mp->file, "\n");

    if (block == 0) {
        /*
         *  Nested block.
         */
        if (typeName.name) {
            propType = (EjsType*) ejsGetPropertyByName(ejs, ejs->global, &typeName);
        } else {
            propType = 0;
        }
        mprAssert(mp->currentBlock && ejsIsBlock(mp->currentBlock));
        slotNum = ejsDefineProperty(ejs, (EjsVar*) mp->currentBlock, -1, &qname, propType, attributes, 0);
    }
}


static void lstModule(EjsMod *mp, EjsModule *module)
{
    mprFprintf(mp->file,
        "\n==============================================================================================\n\n"
        "MODULE:   %s", module->vname);

    if (module->hasInitializer) {
        mprFprintf(mp->file, " <%s>\n", module->hasInitializer ? "hasInitializer, " : "");
    }
    mprFprintf(mp->file, "\n");
}


static int decodeOperands(EjsMod *mp, EjsOptable *opt, char *argbuf, int argbufLen, int address, int *stackEffect)
{
    int         *argp;
    char        *sval, *bufp;
    uchar       *start;
    int         i, argc, ival, len, buflen, j, numEntries;
#if BLD_FEATURE_FLOATING_POINT
    double      dval;
#endif

    *stackEffect = opt->stackEffect;

    /*
     *  Keep a local progressive pointer into the argbuf and a length of the remaining room in the buffer.
     */
    *argbuf = '\0';
    bufp = argbuf;
    buflen = argbufLen;

    for (argc = 0, argp = opt->args; *argp; argc++, argp++) ;

    start = mp->pc;
    ival = 0;

    for (i = 0, argp = opt->args; i < argc; i++) {
        switch (opt->args[i]) {
        case EBC_NONE:
            break;

        case EBC_BYTE:
            ival = getByte(mp);
            mprSprintf(bufp, buflen,  "<%d> ", ival);
            break;

#if BLD_FEATURE_FLOATING_POINT
        case EBC_DOUBLE:
            dval = getDouble(mp);
            mprSprintf(bufp, buflen,  "<%f> ", dval);
            break;
#endif

        case EBC_ARGC:
        case EBC_ARGC2:
        case EBC_ARGC3:
            ival = (int) getNum(mp);
            mprSprintf(bufp, buflen,  "<argc: %d> ", ival);
            break;

        case EBC_SLOT:
            ival = (int) getNum(mp);
            mprSprintf(bufp, buflen,  "<slot: %d> ", ival);
            break;

        case EBC_NUM:
            ival = (int) getNum(mp);
            mprSprintf(bufp, buflen,  "<%d> ", ival);
            break;

        case EBC_JMP8:
            ival = getByte(mp);
            mprSprintf(bufp, buflen,  "<addr: %d> ", ((char) ival) + address + 1);
            break;

        case EBC_JMP:
            ival = getWord(mp);
            mprSprintf(bufp, buflen,  "<addr: %d> ", ival + address + 4);
            break;

        case EBC_INIT_DEFAULT8:
            numEntries = getByte(mp);
            mprSprintf(bufp, buflen,  "<%d> ", numEntries);
            len = (int) strlen(bufp);
            bufp += len;
            buflen -= len;
            for (j = 0; j < numEntries; j++) {
                ival = getByte(mp);
                mprSprintf(bufp, buflen,  "<%d> ", ival + 2);
                len = (int) strlen(bufp);
                bufp += len;
                buflen -= len;
            }
            break;

        case EBC_INIT_DEFAULT:
            numEntries = getByte(mp);
            mprSprintf(bufp, buflen,  "<%d> ", numEntries);
            len = (int) strlen(bufp);
            bufp += len;
            buflen -= len;
            for (j = 0; j < numEntries; j++) {
                ival = getWord(mp);
                mprSprintf(bufp, buflen,  "<%d> ", ival + 2);
                len = (int) strlen(bufp);
                bufp += len;
                buflen -= len;
            }
            break;

        case EBC_STRING:
            sval = getString(mp);
            mprSprintf(bufp, buflen,  "<%s> ", sval);
            break;

        case EBC_GLOBAL:
            getGlobal(mp, bufp, buflen);
            break;

        default:
            mprError(mp, "Bad arg type in opcode table");
            break;
        }
        len = (int) strlen(bufp);
        bufp += len;
        buflen -= len;

        if (opt->args[i] == EBC_ARGC) {
            *stackEffect -= ival;
        } else if (opt->args[i] == EBC_ARGC2) {
            *stackEffect -= (ival * 2);
        } else if (opt->args[i] == EBC_ARGC3) {
            *stackEffect -= (ival * 3);
        }
        if (i == 0 && opt->stackEffect == EBC_POPN) {
            *stackEffect = -ival;
        }
    }
    return (int) (mp->pc - start);
}


/*
 *  Interpret the code for a function
 */
static void interp(EjsMod *mp, EjsModule *module, EjsFunction *fun)
{
    EjsOptable  *optable, *opt;
    EjsCode     *code;
    uchar       *start;
    char        argbuf[MPR_MAX_STRING], lineInfo[MPR_MAX_STRING], name[MPR_MAX_STRING];
    char        *currentLine, *currentFile, *src, *dest;
    int         maxOp, opcode, lineNumber, stack, codeLen, address, stackEffect, nbytes, i, lastDebug;

    mprAssert(mp);
    mprAssert(module);
    mprAssert(fun);

    /*
     *  Store so that getNum and getString can easily read instructions
     */
    code = &fun->body.code;
    mp->fun = fun;
    mp->module = module;
    mp->pc = code->byteCode;
    codeLen = code->codeLen;
    start = mp->pc;
    stack = 0;
    lastDebug = 0;
    currentLine = 0;
    lineNumber = 0;
    currentFile = 0;

    optable = ejsGetOptable(mp);

    for (maxOp = 0, opt = optable; opt->name; opt++) {
        maxOp++;
    }
    
    while ((mp->pc - start) < codeLen) {
        address = (int) (mp->pc - start);
        opcode = *mp->pc++;
        argbuf[0] = '\0';
        stackEffect = 0;

        if (opcode < 0 || opcode >= maxOp) {
            mprError(mp, "Bad opcode %x at address %d.\n", opcode, address);
            return;
        }
        opt = &optable[opcode];

        if (opcode != EJS_OP_DEBUG || mp->showDebug) {
            /*
             *  Output address [stack] opcode
             *  Format:  "address: [stackDepth] opcode <args> ..."
             */
            if (lastDebug) {
                mprFprintf(mp->file, "\n");
                lastDebug = 0;
            }
            mprFprintf(mp->file,  "    %04d: [%d] %02x ", address, stack, opcode);
            mp->showAsm = 1;

        } else {
            mp->showAsm = 0;
        }

        if (opcode == EJS_OP_DEBUG) {
            if ((currentFile = getString(mp)) == 0) {
                goto badToken;
            }
            lineNumber = (int) getNum(mp);
            if ((currentLine = getString(mp)) == 0) {
                goto badToken;
            }
            nbytes = (int) ((mp->pc - start) - address - 1);

        } else {
            nbytes = decodeOperands(mp, opt, argbuf, (int) sizeof(argbuf), (int) (mp->pc - start), &stackEffect);
        }

        if (mp->showAsm) {
            for (i = 24 - (nbytes * 3); i >= 0; i--) {
                mprFprintf(mp->file, ".");
            }
            for (dest = name, src = opt->name; *src; src++, dest++) {
                if (*src == '_') {
                    *dest = *++src;
                } else {
                    *dest = tolower((int) *src);
                }
            }
            *dest++ = '\0';
            mprFprintf(mp->file,  " %s %s\n", name, argbuf);

        } else if (opcode != EJS_OP_DEBUG) {
            for (i = 24 - (nbytes * 3); i >= 0; i--) {
                mprFprintf(mp->file, " ");
            }
            mprFprintf(mp->file,  " %s\n", argbuf);
        }

        stack += stackEffect;

        if (opcode == EJS_OP_RETURN_VALUE || opcode == EJS_OP_RETURN) {
            stack = 0;
        }

        if (stack < 0) {
            if (mp->warnOnError) {
                mprPrintfError(mp, "Instruction stack is negative %d\n", stack);
            }
            if (mp->exitOnError) {
                exit(255);
            }
        }
        if (opcode == EJS_OP_DEBUG) {
            if (!lastDebug) {
                mprFprintf(mp->file, "\n");
            }
            mprSprintf(lineInfo, sizeof(lineInfo), "%s:%d", currentFile, lineNumber);
            mprFprintf(mp->file, "    # %-25s %s\n", lineInfo, currentLine);
            lastDebug = 1;
        }
    }
    return;

badToken:
    mprError(mp, "Bad input stream token 0x%x at %d.\n", 0, address);
    mp->error = 1;
}


static void lstVarSlot(EjsMod *mp, EjsModule *module, EjsName *qname, EjsTrait *trait, int slotNum)
{
    mprAssert(slotNum >= 0);
    mprAssert(trait);
    mprAssert(qname);

    if (qname->name == 0 || qname->name[0] == '\0') {
        mprFprintf(mp->file, "%04d    reserved slot for static super property\n", slotNum);

    } else if (trait->type) {
        if (trait->type == mp->ejs->functionType) {
            mprFprintf(mp->file, "%04d    %s function %s\n", slotNum, qname->space, qname->name);

        } else if (trait->type == mp->ejs->functionType) {
            mprFprintf(mp->file, "%04d    %s class %s\n", slotNum, qname->space, qname->name);

        } else {
            mprFprintf(mp->file, "%04d    %s var %s: %s\n", slotNum, qname->space, qname->name, trait->type->qname.name);
        }

    } else {
        mprFprintf(mp->file, "%04d    %s var %s\n", slotNum, qname->space, qname->name);
    }
}


/*
 *  List the various property slot assignments
 */
static void lstSlotAssignments(EjsMod *mp, EjsModule *module, EjsVar *parent, int slotNum, EjsVar *obj)
{
    Ejs             *ejs;
    EjsTrait        *trait;
    EjsType         *type;
    EjsVar          *vp;
    EjsFunction     *fun;
    EjsBlock        *block, *instanceBlock;
    EjsName         qname;
    int             i, numInherited, count;

    mprAssert(obj);
    mprAssert(module);

    ejs = mp->ejs;

    if (obj->visited) {
        return;
    }
    obj->visited = 1;

    if (obj == ejs->global) {
        type = (EjsType*) obj;
        numInherited = (type->baseType) ? ejsGetPropertyCount(ejs, (EjsVar*) type->baseType) : 0;
        mprFprintf(mp->file,  "\n#\n"
            "#  Global slot assignments (Num prop %d, num inherited %d)\n"
            "#\n", ejsGetPropertyCount(ejs, (EjsVar*) type), numInherited);

        /*
         *  List slots for global
         */
        count = ejsGetNumTraits(ejs->globalBlock);
        for (i = module->firstGlobalSlot; i < count; i++) {
            trait = ejsGetPropertyTrait(ejs, ejs->global, i);
            qname = ejsGetPropertyName(ejs, ejs->global, i);
            if (qname.name == 0) {
                continue;
            }
            if (trait->attributes & EJS_ATTR_BUILTIN && !mp->showBuiltin) {
                continue;
            }
            mprAssert(trait);
            lstVarSlot(mp, module, &qname, trait, i);
        }


        /*
         *  List slots for the initializer
         */
        fun = (EjsFunction*) module->initializer;
        if (fun) {
            mprFprintf(mp->file,  "\n#\n"
                "#  Initializer slot assignments (Num prop %d)\n"
                "#\n", ejsGetPropertyCount(ejs, (EjsVar*) fun));

            count = ejsGetNumTraits((EjsBlock*) fun);
            for (i = 0; i < count; i++) {
                trait = ejsGetPropertyTrait(ejs, (EjsVar*) fun, i);
                qname = ejsGetPropertyName(ejs, (EjsVar*) fun, i);
                if (qname.name == 0) {
                    continue;
                }
                if (trait->attributes & EJS_ATTR_BUILTIN && !mp->showBuiltin) {
                    continue;
                }
                mprAssert(trait);
                lstVarSlot(mp, module, &qname, trait, i);
            }
        }

    } else if (ejsIsFunction(obj)) {

        fun = (EjsFunction*) obj;
        qname = ejsGetPropertyName(ejs, fun->owner, fun->slotNum);

        count = ejsGetNumTraits((EjsBlock*) obj);
        if (count > 0) {
            mprFprintf(mp->file,  "\n#\n"
                "#  Local slot assignments for the \"%s\" function (Num slots %d)\n"
                "#\n", qname.name, count);

            for (i = 0; i < count; i++) {
                trait = ejsGetPropertyTrait(ejs, obj, i);
                mprAssert(trait);
                qname = ejsGetPropertyName(ejs, obj, i);
                lstVarSlot(mp, module, &qname, trait, i);
            }
        }

    } else if (ejsIsType(obj)) {
        /*
         *  Types
         */
        type = (EjsType*) obj;
        numInherited = type->baseType ? ejsGetPropertyCount(ejs, (EjsVar*) type->baseType) : 0;
        mprFprintf(mp->file,  "\n#\n"
            "#  Class slot assignments for the \"%s\" class (Num slots %d, num inherited %d)\n"
            "#\n", type->qname.name,
            ejsGetPropertyCount(ejs, (EjsVar*) type), numInherited);

        count = ejsGetNumTraits((EjsBlock*) type);
        for (i = 0; i < count; i++) {
            trait = ejsGetPropertyTrait(ejs, (EjsVar*) type, i);
            mprAssert(trait);
            qname = ejsGetPropertyName(ejs, obj, i);
            lstVarSlot(mp, module, &qname, trait, i);
        }

        instanceBlock = type->instanceBlock;
        if (type->baseType && type->baseType->instanceBlock) {
            numInherited = ejsGetPropertyCount(ejs, (EjsVar*) type->baseType->instanceBlock);
        } else {
            numInherited = 0;
        }
        mprFprintf(mp->file,  "\n#\n"
            "#  Instance slot assignments for the \"%s\" class (Num prop %d, num inherited %d)\n"
            "#\n", type->qname.name,
            instanceBlock ? ejsGetPropertyCount(ejs, (EjsVar*) instanceBlock): 0 ,
            numInherited);

        if (instanceBlock) {
            count = ejsGetNumTraits(instanceBlock);
            for (i = 0; i < count; i++) {
                trait = ejsGetPropertyTrait(ejs, (EjsVar*) instanceBlock, i);
                mprAssert(trait);
                qname = ejsGetPropertyName(ejs, (EjsVar*) instanceBlock, i);
                if (qname.name) {
                    lstVarSlot(mp, module, &qname, trait, i);
                }
            }
        }

    } else if (ejsIsBlock(obj)) {

        qname = ejsGetPropertyName(ejs, parent, slotNum);
        block = (EjsBlock*) obj;
        count = ejsGetNumTraits(block);
        if (count > 0) {
            mprFprintf(mp->file,  
                "\n#\n"
                "#  Block slot assignments for the \"%s\" (Num slots %d)\n"
                "#\n", qname.name, ejsGetPropertyCount(ejs, obj));
            
            count = ejsGetNumTraits((EjsBlock*) obj);
            for (i = 0; i < count; i++) {
                trait = ejsGetPropertyTrait(ejs, obj, i);
                mprAssert(trait);
                qname = ejsGetPropertyName(ejs, obj, i);
                lstVarSlot(mp, module, &qname, trait, i);
            }
        }
    }

    /*
     *  Now recurse on types, functions and blocks
     */
    count = ejsGetNumTraits((EjsBlock*) obj);

    i = (obj == ejs->global) ? module->firstGlobalSlot : 0;
    for (; i < count; i++) {
        trait = ejsGetPropertyTrait(ejs, obj, i);
        qname = ejsGetPropertyName(ejs, obj, i);
        vp = ejsGetProperty(ejs, obj, i);
        if (vp == 0 || (trait->attributes & EJS_ATTR_BUILTIN && !mp->showBuiltin)) {
            continue;
        }
        if (ejsIsType(vp) || ejsIsFunction(vp) || ejsIsBlock(vp)) {
            lstSlotAssignments(mp, module, obj, i, vp);
        }
    }

    obj->visited = 0;
}


static cchar *getBlockName(EjsMod *mp, EjsVar *block, int slotNum)
{
    EjsFunction     *fun;
    EjsName         qname;

    if (block) {
        if (ejsIsType(block)) {
            return ((EjsType*) block)->qname.name;

        } else if (ejsIsFunction(block)) {
            fun = (EjsFunction*) block;
            if (fun->owner) {
                qname = ejsGetPropertyName(mp->ejs, fun->owner, fun->slotNum);
            } else {
                /*
                 *  Only the initializers don't have an owner
                 */
                ejsName(&qname, 0, EJS_INITIALIZER_NAME);
            }
            return qname.name;
        }
    }
    mprAssert(ejsIsBlock(block));
    qname = ejsGetPropertyName(mp->ejs, block, slotNum);
    return qname.name;
}


static char *getAttributeString(EjsMod *mp, int attributes)
{
    static char attributeBuf[MPR_MAX_STRING];

    attributeBuf[0] = '\0';

    /*
     *  Order to look best
     */
    if (attributes & EJS_ATTR_NATIVE) {
        strcat(attributeBuf, "native ");
    }
    if (attributes & EJS_ATTR_PROTOTYPE) {
        strcat(attributeBuf, "prototype ");
    }
    if (attributes & EJS_ATTR_CONST) {
        strcat(attributeBuf, "const ");
    }
    if (attributes & EJS_ATTR_STATIC) {
        strcat(attributeBuf, "static ");
    }
    if (attributes & EJS_ATTR_FINAL) {
        strcat(attributeBuf, "final ");
    }
    if (attributes & EJS_ATTR_OVERRIDE) {
        strcat(attributeBuf, "override ");
    }
    if (attributes & EJS_ATTR_DYNAMIC_INSTANCE) {
        strcat(attributeBuf, "dynamic ");
    }
    if (attributes & EJS_ATTR_ENUMERABLE) {
        strcat(attributeBuf, "enumerable ");
    }
    if (attributes & EJS_ATTR_GETTER) {
        strcat(attributeBuf, "get ");
    }
    if (attributes & EJS_ATTR_SETTER) {
        strcat(attributeBuf, "set ");
    }
    return attributeBuf;
}


static uchar getByte(EjsMod *mp)
{
    if (mp->showAsm) {
        mprFprintf(mp->file, "%02x ",  mp->pc[0] & 0xFF);
    }
    return *mp->pc++ & 0xFF;
}


static uint getWord(EjsMod *mp)
{
    uchar   *start;
    uint    value;

    start = mp->pc;
    value = ejsDecodeWord(&mp->pc);

    if (mp->showAsm) {
        for (; start < mp->pc; start++) {
            mprFprintf(mp->file, "%02x ", *start & 0xff);
        }
    }
    return value;
}


#if BLD_FEATURE_FLOATING_POINT
static double getDouble(EjsMod *mp)
{
    uchar   *start;
    double  value;

    start = mp->pc;
    value = ejsDecodeDouble(mp->ejs, &mp->pc);

    if (mp->showAsm) {
        for (; start < mp->pc; start++) {
            mprFprintf(mp->file, "%02x ", *start & 0xff);
        }
    }
    return value;
}
#endif


/*
 *  Get an encoded number
 */
static int64 getNum(EjsMod *mp)
{
    uchar       *start;
    int64       value;

    start = mp->pc;
    value = ejsDecodeNum(&mp->pc);

    if (mp->showAsm) {
        for (; start < mp->pc; start++) {
            mprFprintf(mp->file, "%02x ", *start & 0xff);
        }
    }
    return value;
}


/*
 *  Read an interned string constants are stored as token offsets into the constant pool. The pool contains null
 *  terminated UTF-8 strings.
 */
static char *getString(EjsMod *mp)
{
    int     number;

    number = (int) getNum(mp);
    if (number < 0) {
        return 0;
    }
    return &mp->module->constants->pool[number];
}


/*
 *  Return the length of bytes added to buf
 */
static void getGlobal(EjsMod *mp, char *buf, int buflen)
{
    Ejs             *ejs;
    EjsName         qname;
    EjsVar          *vp;
    int             t, slotNum;

    ejs = mp->ejs;
    vp = 0;

    if ((t = (int) getNum(mp)) < 0) {
        mprSprintf(buf, buflen,  "<can't read code>");
        return;
    }

    switch (t & EJS_ENCODE_GLOBAL_MASK) {
    default:
        mprAssert(0);
        return;

    case EJS_ENCODE_GLOBAL_NOREF:
        return;

    case EJS_ENCODE_GLOBAL_SLOT:
        /*
         *  Type is a builtin primitive type or we are binding globals.
         */
        slotNum = t >> 2;
        if (0 <= slotNum && slotNum < ejsGetPropertyCount(ejs, ejs->global)) {
            vp = ejsGetProperty(ejs, ejs->global, slotNum);
        }
        if (vp && ejsIsType(vp)) {
            mprSprintf(buf, buflen, "<type: 0x%x,  %s::%s> ", t, ((EjsType*) vp)->qname.space, ((EjsType*) vp)->qname.name);
        }
        break;

    case EJS_ENCODE_GLOBAL_NAME:
        /*
         *  Type was unknown at compile time
         */
        qname.name = &mp->module->constants->pool[t >> 2];
        if (qname.name == 0) {
            mprAssert(0);
            mprSprintf(buf, buflen,  "<var: 0x%x,  missing name> ", t);
            return;
        }
        if ((qname.space = getString(mp)) == 0) {
            mprSprintf(buf, buflen,  "<var: 0x%x,  missing namespace> ", t);
            return;
        }
        if (qname.name) {
            vp = ejsGetPropertyByName(ejs, ejs->global, &qname);
        }
        mprSprintf(buf, buflen, "<var: 0x%x,  %s::%s> ", t, qname.space, qname.name);
        break;
    }

    if (vp == 0) {
        mprSprintf(buf, buflen, "<var: %d,  cannot resolve var/typ at slot e> ", t);
    }
}


static void leadin(EjsMod *mp, EjsModule *module, int classDec, int inFunction)
{
    mprFprintf(mp->file, "    ");
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
