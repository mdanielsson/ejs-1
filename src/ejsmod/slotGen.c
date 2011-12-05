/**
 *  slotGen.c - Slot file generator
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "ejsmod.h"

/**************************** Forward Declarations ****************************/

static int  createSlotFile(EjsMod *bp, EjsModule *mp, MprFile *file, bool java, int builtinGlobals);
static int  genType(EjsMod *bp, MprFile *file, EjsModule *mp, EjsType *type, int firstClassSlot,
                int firstInstanceSlot, bool java);

/*********************************** Code *************************************/

int emCreateSlotFiles(EjsMod *bp, EjsModule *mp, MprFile *outfile, int builtinGlobals)
{
    int     rc;

    rc = 0;
    if (bp->cslots) {
        rc += createSlotFile(bp, mp, outfile, 0, builtinGlobals);
    }
    if (bp->jslots) {
        rc += createSlotFile(bp, mp, outfile, 1, builtinGlobals);
    }
    return rc;
}


static int createSlotFile(EjsMod *bp, EjsModule *mp, MprFile *file, bool java, int builtinGlobals)
{
    MprFile     *localFile;
    Ejs         *ejs;
    EjsName     qname;
    EjsType     *type;
    char        *path, slotsName[MPR_MAX_FNAME], moduleName[MPR_MAX_FNAME];
    char        *cp, *sp, *dp, *javaPackage;
    int         firstClassSlot, firstInstanceSlot, slotNum;

    mprAssert(bp);

    path = 0;
    localFile = 0;
    ejs = bp->ejs;

    mprStrcpy(moduleName, sizeof(moduleName), mp->name);
    for (cp = moduleName; *cp; cp++)  {
        if (*cp == '.') {
            *cp = '_';
        }
    }

    mprSprintf(slotsName, sizeof(slotsName), "%sSlots", mp->name);
    slotsName[0] = toupper((int) slotsName[0]);
    for (dp = sp = slotsName; *sp; sp++) {
        if (*sp == '.') {
            ++sp;
            *dp++ = toupper((int) *sp);
        } else {
            *dp++ = *sp;
        }
    }
    *dp = '\0';

    if (file == 0) {
        if (java) {
            path = mprStrcat(bp, -1, slotsName, ".java", NULL);
        } else {
            path = mprStrcat(bp, -1, mp->name, ".slots.h", NULL);
        }
        localFile = file = mprOpen(bp, path, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 0664);
    }
    if (file == 0) {
        mprError(bp, "Can't open %s", path);
        mprFree(path);
        return MPR_ERR_CANT_OPEN;
    }
    mprEnableFileBuffering(file, 0, 0);

    mprFprintf(file,
        "/*\n"
        " * %s -- Property slot definitions for the \"%s\" module\n"
        " *\n"
        " * This file is generated by ec\n"
        " *\n"
        " * Slot definitions. Version %s.\n"
        " */\n"
        "\n", path, mp->name, BLD_VERSION);


    if (java) {
        /*
         *  Java won't allow a package called "default" so we remap it to ejs.
         *  This only affects java source packaging, not Ejscript package naming.
         */
        javaPackage = (char*) ((strcmp(mp->name, "default") == 0) ? "ejs" : mp->name);
        mprFprintf(file,
            "package %s;\n"
            "\n"
            "public class %s\n"
            "{\n",
            javaPackage, slotsName);

    } else {
        mprFprintf(file,
            "#ifndef _h_SLOTS_%s\n"
            "#define _h_SLOTS_%s 1\n\n",
            slotsName, slotsName);
    }

    mprFprintf(file, "\n/**\n * Slots for the \"%s\" module \n */\n", mp->name);

    if (bp->showBuiltin) {
        firstClassSlot = 0;
        firstInstanceSlot = 0;

    } else {
        firstClassSlot = (ejs->flags & EJS_FLAG_EMPTY) ? 0 : builtinGlobals;
        firstInstanceSlot = 0;
    }

    ejsName(&qname, EJS_INTRINSIC_NAMESPACE, EJS_GLOBAL);
    slotNum = ejsGetPropertyCount(ejs, ejs->global);
    type = ejsCreateType(ejs, &qname, NULL, NULL, sizeof(EjsType), slotNum, ejs->globalBlock->obj.numProp, 0, 0, NULL);
    type->block = *ejs->globalBlock;
    type->block.obj.var.type = ejs->typeType;
    type->block.obj.var.isType = 1;
    ejs->global = (EjsVar*) type;
    ejs->globalBlock = &type->block;

    if (genType(bp, file, mp, type, firstClassSlot, firstInstanceSlot, java) < 0) {
        mprError(bp, "Can't generate slot file for module %s", mp->name);
        mprFree(path);
        mprFree(localFile);
        return EJS_ERR;
    }
    mprFprintf(file, "\n#define _ES_CHECKSUM_%s %d\n", moduleName, mp->checksum);

    if (java) {
        mprFprintf(file, "\n}\n");
    } else {
        mprFprintf(file, "\n#endif\n");
    }

    mprFree(localFile);
    mprFree(path);
    return 0;
}



static char *mapName(MprCtx ctx, cchar *name, int java, int mapTypeName)
{
    cchar   *value;
    char    *buf, *cp;

    if (name == 0) {
        name = "";
    }

    value = name;
    if (mapTypeName) {
        if (strcmp(name, EJS_GLOBAL) == 0) {
            value = "";
        }
    }
    if (strcmp(name, "*") == 0) {
        value = "DEFAULT";
    }

    if (java) {
        if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "boolean") == 0) {
            value = "booleanValue";
        } else if (strcmp(name, "double") == 0) {
            value = "doubleValue";
        } else if (strcmp(name, "long") == 0) {
            value = "longValue";
        } else if (strcmp(name, "false") == 0) {
            value = "falseValue";
        } else if (strcmp(name, "true") == 0) {
            value = "trueValue";
        } else if (strcmp(name, "assert") == 0) {
            value = "assertValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        } else if (strcmp(name, "int") == 0) {
            value = "intValue";
        }
    }

    if (strcmp(name, "+") == 0) {
        value = "PLUS";
    } else if (strcmp(name, "-") == 0) {
        value = "MINUS";
    } else if (strcmp(name, "/") == 0) {
        value = "DIV";
    } else if (strcmp(name, "*") == 0) {
        value = "MUL";
    } else if (strcmp(name, "%") == 0) {
        value = "MOD";
    } else if (strcmp(name, "[") == 0) {
        value = "LBRACKET";
    } else if (strcmp(name, "&") == 0) {
        value = "AND";
    } else if (strcmp(name, "|") == 0) {
        value = "OR";
    } else if (strcmp(name, "<<") == 0) {
        value = "LSH";
    } else if (strcmp(name, ">>") == 0) {
        value = "RSH";
    } else if (strcmp(name, "<") == 0) {
        value = "LT";
    } else if (strcmp(name, ">") == 0) {
        value = "GT";
    } else if (strcmp(name, "<=") == 0) {
        value = "LE";
    } else if (strcmp(name, ">=") == 0) {
        value = "GE";
    } else if (strcmp(name, "=") == 0) {
        value = "ASSIGN";
    } else if (strcmp(name, "==") == 0) {
        value = "EQ";
    } else if (strcmp(name, "!=") == 0) {
        value = "NE";
    } else if (strcmp(name, "===") == 0) {
        value = "STRICT_EQ";
    } else if (strcmp(name, "!==") == 0) {
        value = "STRICT_NE";
    } else if (strcmp(name, "!") == 0) {
        value = "NOT";
    } else if (strcmp(name, "~") == 0) {
        value = "NEG";
    } else if (strcmp(name, "< ") == 0) {
        value = "LT_BUG";
    } else if (strcmp(name, "= ") == 0) {
        value = "ASSIGN_BUG";
    }

    buf = mprStrdup(ctx, value);
    for (cp = buf; *cp; cp++)  {
        if (*cp == '-') {
            *cp = '_';
        }
    }
    return buf;
}



static char *mapSpace(MprCtx ctx, cchar *space, int java, int mapTypeName)
{
    char    *value;

    if (space == 0) {
        space = "";
    }
    if (strcmp(space, EJS_INTRINSIC_NAMESPACE) == 0) {
        space = "";
    } else if (strcmp(space, EJS_PUBLIC_NAMESPACE) == 0) {
        space = "";
    } else if (strcmp(space, EJS_PRIVATE_NAMESPACE) == 0) {
        space = "";
    } else if (strcmp(space, EJS_ITERATOR_NAMESPACE) == 0) {
        space = "";
    } else if (strcmp(space, EJS_CONSTRUCTOR_NAMESPACE) == 0) {
        space = "";
    } else if (strstr(space, ",private]") != 0) {
        space = "";
    } else if (strstr(space, ",protected]") != 0) {
        space = "";
    } else if (strstr(space, "internal-") != 0) {
        space = "";
    }
    value = (char*) space;

    return value;
}



static char *mapFullName(MprCtx ctx, EjsName *qname, int java, int mapTypeName)
{
    cchar       *name, *space;
    char        *cp, *buf;

    if (qname == 0) {
        return mprStrdup(ctx, "");
    }
    name = mapName(ctx, qname->name, java, mapTypeName);
    space = mapSpace(ctx, qname->space, java, mapTypeName);

    if (*space) {
        buf = mprStrcat(ctx, -1, space, "_", name, NULL);
    } else {
        buf = mprStrdup(ctx, name);
    }
    for (cp = buf; *cp; cp++)  {
        if (*cp == '-') {
            *cp = '_';
        }
    }
    return buf;
}




static void defineSlot(EjsMod *bp, MprFile *file, EjsModule *mp, EjsType *type, EjsName *fun, EjsName *name, int slotNum, 
    bool java)
{
    Ejs     *ejs;
    char    nameBuf[MPR_MAX_STRING];
    char    *funSep, *sp, *typeStr, *funStr, *nameStr;

    ejs = bp->ejs;

    typeStr = mapFullName(file, &type->qname, java, 1);
    funStr = mapFullName(file, fun, java, 0);
    nameStr = mapFullName(file, name, java, 0);

    if ((EjsVar*) type == ejs->global) {
        if (!(ejs->flags & EJS_FLAG_EMPTY)) {
            /*
             *  Don't define global slots for global types for non-bound global types
             */
            return;
        }
        if (strcmp(mp->name, "ejs") != 0 && typeStr[0] == '\0') {
            /*
             *  Don't emit slots for non-core modules that have an empty namespace/type field
             */
            return;
        }
    }

    if (nameStr[0] != '\0') {
        funSep = (char*) ((*funStr && *typeStr) ? "_" : "");

        if (java) {
            if (*funStr == '\0') {
                mprSprintf(nameBuf, sizeof(nameBuf), "  public static final int %s = ", nameStr);
            } else {
                mprSprintf(nameBuf, sizeof(nameBuf), "  public static final int %s%s%s_%s = ", typeStr, funSep, 
                    funStr, nameStr);
            }

        } else {
            if (*typeStr == '\0' && *funStr == '\0') {
                mprSprintf(nameBuf, sizeof(nameBuf), "#define ES_%s", nameStr);
            } else {
                mprSprintf(nameBuf, sizeof(nameBuf), "#define ES_%s%s%s_%s", typeStr, funSep, funStr, nameStr);
            }
        }

        for (sp = nameBuf; *sp; sp++) {
            if (*sp == '.') {
                *sp = '_';
            }
        }
        if (java) {
            mprFprintf(file, "%-70s %d;\n", nameBuf, slotNum);
        } else {
            mprFprintf(file, "%-70s %d\n", nameBuf, slotNum);
        }
    }

    mprFree(typeStr);
    mprFree(funStr);
    mprFree(nameStr);
}


static void defineSlotOrigin(EjsMod *bp, MprFile *file, EjsModule *mp, EjsType *type, int slotNum, bool java)
{
    char        name[MPR_MAX_STRING], *typeStr, *sp;

    if ((EjsVar*) type == bp->ejs->global && !bp->showBuiltin) {
        return;
    }

    typeStr = mapFullName(file, &type->qname, java, 1);

    if (typeStr && typeStr[0] != '\0') {
        if (java) {
            mprSprintf(name, sizeof(name), "    public static final int %s__origin = ", typeStr);

        } else {
            mprSprintf(name, sizeof(name), "#define ES_%s__origin", typeStr);
        }

        for (sp = name; *sp; sp++) {
            if (*sp == '.') {
                *sp = '_';
            }
        }
        if (java) {
            mprFprintf(file, "%-70s %d;\n", name, slotNum);
        } else {
            mprFprintf(file, "%-70s %d\n", name, slotNum);
        }
    }
    mprFree(typeStr);
}



static void defineSlotCount(EjsMod *bp, MprFile *file, EjsModule *mp, EjsType *type, char *suffix, int numSlots, bool java)
{
    char        name[MPR_MAX_STRING], *typeStr, *sp;

    if ((EjsVar*) type == bp->ejs->global) {
        if (!bp->showBuiltin) {
            return;
        }
    }

    typeStr = mapFullName(file, &type->qname, java, 1);
    if (*typeStr == '\0') {
        mprFree(typeStr);
        typeStr = mprStrdup(file, EJS_GLOBAL);
    }

    if (java) {
        mprSprintf(name, sizeof(name), "    public static final int %s_NUM_%s_PROP = ", typeStr, suffix);
    } else {
        mprSprintf(name, sizeof(name), "#define ES_%s_NUM_%s_PROP", typeStr, suffix);
    }

    for (sp = name; *sp; sp++) {
        if (*sp == '.') {
            *sp = '_';
        }
    }
    if (java) {
        mprFprintf(file, "%-70s %d;\n", name, numSlots);
    } else {
        mprFprintf(file, "%-70s %d\n", name, numSlots);
    }
    mprFree(typeStr);
}



/*
 *  Generate the slot offsets for a type
 */
static int genType(EjsMod *bp, MprFile *file, EjsModule *mp, EjsType *type, int firstClassSlot, int firstInstanceSlot, 
        bool java)
{
    Ejs             *ejs;
    EjsVar          *vp;
    EjsTrait        *trait;
    EjsType         *nt;
    EjsBlock        *instanceBlock;
    EjsFunction     *fun;
    cchar           *typeName;
    EjsName         qname, lqname;
    int             slotNum, i, methodHeader, numClassInherited, numInstanceInherited, count;

    mprAssert(bp);
    mprAssert(type);
    mprAssert(ejsIsType(type));

    ejs = bp->ejs;

    typeName = type->qname.name;

    mprFprintf(file, "\n\n/**\n *   Class property slots for the \"%s\" class \n */\n", typeName);

    /*
     *  Process class traits (skip base class slots)
     *  Class traits are any class properties or methods.
     */
    defineSlotOrigin(bp, file, mp, type, firstClassSlot, java);

    count = ejsGetNumTraits((EjsBlock*) type);
    for (slotNum = firstClassSlot; slotNum < count; slotNum++) {
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) type, slotNum);
        qname = ejsGetPropertyName(ejs, (EjsVar*) type, slotNum);
        if (qname.name == 0) {
            continue;
        }
        defineSlot(bp, file, mp, type, 0, &qname, slotNum, java);
    }

    /*
     *  For the global type, only emit the count for the "ejs" module
     */
    if ((EjsVar*) type != ejs->global || (ejs->flags & EJS_FLAG_EMPTY && strcmp(mp->name, "ejs") == 0)) {
        defineSlotCount(bp, file, mp, type, "CLASS", slotNum, java);
    }


    /*
     *  Process instance traits
     */
    mprFprintf(file, "\n/**\n * Instance slots for \"%s\" type \n */\n", typeName);
    instanceBlock = type->instanceBlock;
    if (instanceBlock) {
        count = ejsGetNumTraits(instanceBlock);
        for (slotNum = firstInstanceSlot; slotNum < count; slotNum++) {
            trait = ejsGetPropertyTrait(ejs, (EjsVar*) instanceBlock, slotNum);
            qname = ejsGetPropertyName(ejs, (EjsVar*) instanceBlock, slotNum);
            if (qname.name == 0) {
                continue;
            }
            defineSlot(bp, file, mp, type, 0, &qname, slotNum, java);
        }

    } else {
        slotNum = 0;
    }
    
    /*
     *  For the global type, only emit the count for the "ejs" module
     */
    if ((EjsVar*) type != ejs->global || (ejs->flags & EJS_FLAG_EMPTY && strcmp(mp->name, "ejs") == 0)) {
        defineSlotCount(bp, file, mp, type, "INSTANCE", slotNum, java);
    }


    /*
     *  Now examine methods in the type and define any locals and args
     */
    methodHeader = 0;
    count = ejsGetNumTraits((EjsBlock*) type);
    for (slotNum = firstClassSlot; slotNum < count; slotNum++) {
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) type, slotNum);
        qname = ejsGetPropertyName(ejs, (EjsVar*) type, slotNum);
        if (qname.name == 0) {
            continue;
        }
        if (trait->type != ejs->functionType) {
            continue;
        }

        vp = ejsGetProperty(ejs, (EjsVar*) type, slotNum);
        if (vp == 0 || !ejsIsFunction(vp)) {
            continue;
        }

        fun = ((EjsFunction*) vp);
        if (fun->block.obj.numProp == 0) {
            continue;
        }

        if (!methodHeader) {
            mprFprintf(file, "\n/**\n * \n *    Local slots for methods in type %s \n */\n", typeName);
            methodHeader++;
        }

        /*
         *  Output the arg names and local variable names.
         */
        for (i = 0; i < (int) fun->numArgs; i++) {
#if UNUSED
            lp = ejsGetPropertyTrait(ejs, vp, i);
#endif
            lqname = ejsGetPropertyName(ejs, vp, i);
            defineSlot(bp, file, mp, type, &qname, &lqname, i, java);
        }
        for (; i < fun->block.obj.numProp; i++) {
#if UNUSED
            lp = ejsGetPropertyTrait(ejs, vp, i);
#endif
            lqname = ejsGetPropertyName(ejs, vp, i);
            defineSlot(bp, file, mp, type, &qname, &lqname, i, java);
        }
    }


    /*
     *  Now process nested types.
     */
    count = ejsGetNumTraits((EjsBlock*) type);
    for (slotNum = firstClassSlot; slotNum < count; slotNum++) {
        trait = ejsGetPropertyTrait(ejs, (EjsVar*) type, slotNum);
        qname = ejsGetPropertyName(ejs, (EjsVar*) type, slotNum);

        if (qname.name == 0) {
            continue;
        }
        vp = ejsGetProperty(ejs, (EjsVar*) type, slotNum);
        if (vp == 0) {
            continue;
        }

        if (! ejsIsType(vp) || vp->visited) {
            continue;
        }

        nt = (EjsType*) vp;

        if (nt->module != mp) {
            continue;
        }
        vp->visited = 1;

        numClassInherited = (nt->baseType) ? ejsGetPropertyCount(ejs, (EjsVar*) nt->baseType) : 0;
        numInstanceInherited = (nt->instanceBlock && nt->baseType->instanceBlock) ?
            ejsGetPropertyCount(ejs, (EjsVar*) nt->baseType->instanceBlock) : 0;

        if (genType(bp, file, mp, nt, numClassInherited, numInstanceInherited, java) < 0) {
            vp->visited = 0;
            return EJS_ERR;
        }
        vp->visited = 0;
    }

    return 0;
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
