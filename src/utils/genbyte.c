/**
 *  genbyte.c - Program to generate various forms of the byte code definition files
 *
 *  Generates: ejsByteCode.h, ejsGoto.h
 *
 *  Interactive shell that interprets interactive sessions and command files.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#define EJS_DEFINE_OPTABLE 1

#include    "ejsByteCodeTable.h"

/********************************** Forwards **********************************/

static void footer(MprFile *file);
static void genByteCodeHeader(Mpr *mpr, cchar *dir);
static void genByteGotoHeader(Mpr *mpr, cchar *dir);
static void header(MprFile *file, cchar *name, cchar *desc);

/************************************ Code ************************************/

int main(int argc, char **argv)
{
    Mpr     *mpr;
    cchar   *dir;

    mpr = mprCreate(argc, argv, NULL);
    if (mprStart(mpr, 0) < 0) {
        mprError(mpr, "Can't start mpr services");
        return MPR_ERR;
    }
    dir = (argc == 2) ? argv[1] : ".";
    genByteCodeHeader(mpr, dir);
    genByteGotoHeader(mpr, dir);
    mprFree(mpr);
    return 0;
}


static void genByteCodeHeader(Mpr *mpr, cchar *dir)
{
    MprFile     *file;
    EjsOptable  *op;
    char        *path;

    path = mprJoinPath(mpr, dir, "ejsByteCode.h");
    file = mprOpen(mpr, path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (file == 0) {
        mprError(mpr, "Can't open %s", path);
        return;
    } 
    header(file, "ejsByteCode", "Ejscript VM Byte Code");

    mprFprintf(file, "typedef enum EjsOpCode {\n");
    for (op = ejsOptable; op->name; op++) {
        mprFprintf(file, "    EJS_OP_%s,\n", op->name);
    }
    mprFprintf(file, "} EjsOpCode;\n");
    footer(file);
    mprFree(file);
}


static void genByteGotoHeader(Mpr *mpr, cchar *dir)
{
    MprFile     *file;
    EjsOptable  *op;
    char        *path;

    path = mprJoinPath(mpr, dir, "ejsByteGoto.h");
    file = mprOpen(mpr, path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (file == 0) {
        mprError(mpr, "Can't open %s", path);
        return;
    } 
    // header(file, "ejsByteGoto", "Ejscript Byte Code Jump Labels");

    mprFprintf(file, "static void *opcodeJump[] = {\n");
    for (op = ejsOptable; op->name; op++) {
        mprFprintf(file, "    &&EJS_OP_%s,\n", op->name);
    }
    mprFprintf(file, "};\n");
    // footer(file);
    mprFree(file);
}


static void header(MprFile *file, cchar *name, cchar *desc)
{
    mprFprintf(file, "\
/*\n\
 *  %s.h - %s\n\
 *\n\
 *  Copyright (c) All Rights Reserved. See details at the end of the file.\n\
 */\n\
\n\
#ifndef _h_EJS_%s\n\
#define _h_EJS_%s 1\n\
\n\
\n", name, desc, name, name);
}


static void footer(MprFile *file)
{
    mprFprintf(file, "\
\n\
#endif\n\
\n\
/*\n\
 *  Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.\n\
 *  Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.\n\
 *\n\
 *  This software is distributed under commercial and open source licenses.\n\
 *  You may use the GPL open source license described below or you may acquire\n\
 *  a commercial license from Embedthis Software. You agree to be fully bound\n\
 *  by the terms of either license. Consult the LICENSE.TXT distributed with\n\
 *  this software for full details.\n\
 *\n\
 *  This software is open source; you can redistribute it and/or modify it\n\
 *  under the terms of the GNU General Public License as published by the\n\
 *  Free Software Foundation; either version 2 of the License, or (at your\n\
 *  option) any later version. See the GNU General Public License for more\n\
 *  details at: http://www.embedthis.com/downloads/gplLicense.html\n\
 *\n\
 *  This program is distributed WITHOUT ANY WARRANTY; without even the\n\
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
 *\n\
 *  This GPL license does NOT permit incorporating this software into\n\
 *  proprietary programs. If you are unable to comply with the GPL, you must\n\
 *  acquire a commercial license to use this software. Commercial licenses\n\
 *  for this software and support services are available from Embedthis\n\
 *  Software at http://www.embedthis.com\n\
 */\n");
}

