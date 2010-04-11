/*
 *  ejsModule.h - Module file format.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#ifndef _h_EJS_MODULE
#define _h_EJS_MODULE 1

#include    "ejsCore.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************* Prototypes *********************************/
/*
 *  A module file may contain multiple logical modules.
 *
 *  Module File Format and Layout:
 *
 *  (N) Numbers are 1-3 little-endian encoded bytes using the 0x80 as the continuation character
 *  (S) Strings are pointers into the constant pool encoded as number offsets. Strings are UTF-8.
 *  (T) Types are encoded and ored with the type encoding masks below. Types are either: untyped, 
 *      unresolved or primitive (builtin). The relevant mask is ored into the lower 2 bits. Slot numbers and
 *      name tokens are shifted up 2 bits. Zero means untyped.
 *
 *  ModuleHeader {
 *      short       magic
 *      int         fileVersion
 *      int         version
 *      int         flags
 *  }
 *
 *  Module {
 *      byte        section
 *      string      name
 *      number      version
 *      number      checksum
 *      number      constantPoolLength
 *      block       constantPool
 *  }
 *
 *  Dependencies {
 *      byte        section
 *      string      moduleName
 *      number      minVersion
 *      number      maxVersion
 *      number      checksum
 *      byte        flags
 *  }
 *
 *  Type {
 *      byte        section
 *      string      typeName
 *      string      namespace
 *      number      attributes
 *      number      slot
 *      type        baseType
 *      number      numStaticProperties
 *      number      numInstanceProperties
 *      number      numInterfaces
 *      type        Interfaces ...
 *      ...
 *  }
 *
 *  Property {
 *      byte        section
 *      string      name
 *      string      namespace
 *      number      attributes
 *      number      slot
 *      type        property type
 *  }
 *
 *  Function {
 *      byte        section
 *      string      name
 *      string      namespace
 *      number      nextSlotForSetter
 *      number      attributes
 *      byte        languageMode
 *      type        returnType
 *      number      slot
 *      number      argCount
 *      number      localCount
 *      number      exceptionCount
 *      number      codeLength
 *      block       code        
 *  }
 *
 *  Exception {
 *      byte        section
 *      byte        flags
 *      number      tryStartOffset
 *      number      tryEndOffset
 *      number      handlerStartOffset
 *      number      handlerEndOffset
 *      number      numOpenBlocks
 *      type        catchType
 *  }
 *
 *  Block {
 *      byte        section
 *      string      name
 *      number      slot
 *      number      propCount
 *  }
 *
 *  Documentation {
 *      byte        section
 *      string      text
 *  }
 */

/*
 *  Type encoding masks
 */
#define EJS_ENCODE_GLOBAL_NOREF         0x0
#define EJS_ENCODE_GLOBAL_NAME          0x1
#define EJS_ENCODE_GLOBAL_SLOT          0x2
#define EJS_ENCODE_GLOBAL_MASK          0x3

/*
 *  Fixup kinds
 */
#define EJS_FIXUP_BASE_TYPE             1
#define EJS_FIXUP_INTERFACE_TYPE        2
#define EJS_FIXUP_RETURN_TYPE           3
#define EJS_FIXUP_TYPE_PROPERTY         4
#define EJS_FIXUP_INSTANCE_PROPERTY     5
#define EJS_FIXUP_LOCAL                 6
#define EJS_FIXUP_EXCEPTION             7

/*
 *  Number encoding uses one bit per byte plus a sign bit in the first byte
 */ 
#define EJS_ENCODE_MAX_WORD             0x07FFFFFF

typedef struct EjsTypeFixup
{
    int                 kind;                       /* Kind of fixup */
    EjsVar              *target;                    /* Target to fixup */
    int                 slotNum;                    /* Slot of target */
    EjsName             typeName;                   /* Type name */
    int                 typeSlotNum;                /* Type slot number */
} EjsTypeFixup;


#define EJS_MODULE_MAGIC        0xC7DA

/*
 *  Module file format version
 */
#define EJS_MODULE_VERSION      2
#define EJS_VERSION_FACTOR      1000

/*
 *  Module content version
 */
#define EJS_COMPAT_VERSION(v1, v2) ((v1 / EJS_VERSION_FACTOR) == (v2 / EJS_VERSION_FACTOR))
#define EJS_MAKE_COMPAT_VERSION(version) (version / EJS_VERSION_FACTOR * EJS_VERSION_FACTOR)
#define EJS_MAKE_VERSION(maj, min, patch) (((((maj) * EJS_VERSION_FACTOR) + (min)) * EJS_VERSION_FACTOR) + (patch))
#define EJS_MAJOR(version)  (((version / EJS_VERSION_FACTOR) / EJS_VERSION_FACTOR) % EJS_VERSION_FACTOR)
#define EJS_MINOR(version)  ((version / EJS_VERSION_FACTOR) % EJS_VERSION_FACTOR)
#define EJS_PATCH(version)  (version % EJS_VERSION_FACTOR)
#define EJS_MAX_VERSION     EJS_MAKE_VERSION(EJS_VERSION_FACTOR-1, EJS_VERSION_FACTOR-1, EJS_VERSION_FACTOR-1)

/*
 *  Section types
 */
#define EJS_SECT_MODULE         1           /* Module section */
#define EJS_SECT_MODULE_END     2           /* End of a module */
#define EJS_SECT_DEPENDENCY     3           /* Module dependency */
#define EJS_SECT_CLASS          4           /* Class definition */
#define EJS_SECT_CLASS_END      5           /* End of class definition */
#define EJS_SECT_FUNCTION       6           /* Function */
#define EJS_SECT_FUNCTION_END   7           /* End of function definition */
#define EJS_SECT_BLOCK          8           /* Nested block */
#define EJS_SECT_BLOCK_END      9           /* End of Nested block */
#define EJS_SECT_PROPERTY       10          /* Property (variable) definition */
#define EJS_SECT_EXCEPTION      11          /* Exception definition */
#define EJS_SECT_DOC            12          /* Documentation for an element */
#define EJS_SECT_MAX            13

/*
 *  Psudo section types for loader callback
 */
#define EJS_SECT_START          (EJS_SECT_MAX + 1)
#define EJS_SECT_END            (EJS_SECT_MAX + 2)


/*
 *  Align headers on a 4 byte boundary
 */
#define EJS_HDR_ALIGN           4


/*
 *  Module header flags
 */
#define EJS_MODULE_INTERP_EMPTY 0x2         /* Module compiled with --empty. ie. not using native elements */


/*
 *  Module loader flags
 */
#define EJS_MODULE_DONT_INIT    0x1         /* Don't run initializers */
#define EJS_MODULE_BUILTIN      0x2         /* Loading builtins */

/*
 *  File format is little-endian. All headers are aligned on word boundaries.
 */
typedef struct EjsModuleHdr {
    int         magic;                      /* Magic number for Ejscript modules */
    int         fileVersion;                /* Module file format version */
    int         flags;                      /* Module flags */
} EjsModuleHdr;


/*
 *  Structure for the string constant pool
 */
typedef struct EjsConst {
    char        *pool;                      /* Constant pool storage */
    int         size;                       /* Size of constant pool storage */
    int         len;                        /* Length of active constant pool */
    int         base;                       /* Base used during relocations */
    int         locked;                     /* No more additions allowed */
    MprHashTable *table;                    /* Hash table for fast lookup */
} EjsConst;


/*
 *  Module
 */
typedef struct EjsModule {
    char            *name;                  /* Name of this module */
    char            *vname;                 /* Versioned name */
    char            *path;                  /* Module file path name */
    int             version;                /* Made with EJS_MAKE_VERSION */
    int             minVersion;             /* Minimum version when used as a dependency */
    int             maxVersion;             /* Maximum version when used as a dependency */
    int             checksum;               /* Checksum of slots and names */

    MprList         *dependencies;          /* Module file dependencies. List of EjsModules */

    /*
     *  Used during code generation
     */
    struct EcCodeGen *code;                 /* Code generation buffer */
    MprFile         *file;                  /* File handle */
    MprList         *globalProperties;      /* List of global properties */
    EjsFunction     *initializer;           /* Initializer method */

    /*
     *  Used only while loading modules
     */
    EjsBlock        *scopeChain;            /* Scope of nested types/functions/blocks, being loaded */
    EjsConst        *constants;             /* Constant pool */
    int             nameToken;              /* */
    int             firstGlobalSlot;        /* First global slot (if used) */
    struct EjsFunction  *currentMethod;     /* Current method being loaded */

    uint            compiling       : 1;    /* Module currently being compiled from source */
    uint            loaded          : 1;    /* Module has been loaded from an external file */
    uint            nativeLoaded    : 1;    /* Backing shared library loaded */
    uint            configured      : 1;    /* Module is already configured */
    uint            hasNative       : 1;    /* Has native property definitions */
    uint            hasInitializer  : 1;    /* Has initializer function */
    uint            initialized     : 1;    /* Initializer has run */
    uint            hasError        : 1;    /* Module has a loader error */
    int             flags;                  /* Loading flags */

#if BLD_FEATURE_EJS_DOC
    char            *doc;                   /* Current doc string */
#endif

} EjsModule;


/*
 *  Documentation string information
 */
#if BLD_FEATURE_EJS_DOC

/*
 *  Element documentation string. The loader will create if required.
 */
typedef struct EjsDoc {
    char        *docString;                         /* Original doc string */
    char        *brief;
    char        *description;
    char        *example;
    char        *requires;
    char        *returns;
    char        *stability;                         /* prototype, evolving, stable, mature, deprecated */
    char        *spec;                              /* Where specified */
    bool        hide;                               /* Hide doc if true */

    int         cracked;                            /* Doc has been cracked and tokenized */

    struct EjsDoc *duplicate;                       /* From @duplicate directive */

    MprList     *defaults;                          /* Parameter default values */
    MprList     *params;                            /* Function parameters */
    MprList     *options;                           /* Option parameter values */
    MprList     *events;                            /* Option parameter values */
    MprList     *see;
    MprList     *throws;

    EjsTrait    *trait;                             /* Back pointer to trait */
} EjsDoc;
#endif

/******************************** Prototypes **********************************/

extern EjsModule    *ejsCreateModule(struct Ejs *ejs, cchar *name, int version);
extern int          ejsLoadModule(struct Ejs *ejs, cchar *name, int minVer, int maxVer, int flags, MprList **modules);
extern char         *ejsSearchForModule(Ejs *ejs, cchar *name, int minVer, int maxVer);

extern int          ejsModuleReadName(struct Ejs *ejs, MprFile *file, char **name, int len);
extern int          ejsModuleReadNumber(struct Ejs *ejs, EjsModule *module, int *number);
extern int          ejsModuleReadByte(struct Ejs *ejs, EjsModule *module, int *number);
extern char         *ejsModuleReadString(struct Ejs *ejs, EjsModule *module);
extern int          ejsModuleReadType(struct Ejs *ejs, EjsModule *module, EjsType **typeRef, EjsTypeFixup **fixup, 
                        EjsName *typeName, int *slotNum);
extern int          ejsSetModuleConstants(struct Ejs *ejs, EjsModule *mp, cchar *pool, int poolSize);

extern double       ejsDecodeDouble(uchar **pp);
extern int64        ejsDecodeNum(uchar **pp);
extern int          ejsDecodeWord(uchar **pp);
extern int          ejsEncodeNum(uchar *pos, int64 number);
extern int          ejsEncodeWord(uchar *pos, int number);
extern int          ejsEncodeDouble(uchar *pos, double number);
extern int          ejsEncodeByteAtPos(uchar *pos, int value);
extern int          ejsEncodeUint(uchar *pos, int number);
extern int          ejsEncodeWordAtPos(uchar *pos, int value);

#if BLD_FEATURE_EJS_DOC
extern char         *ejsGetDocKey(struct Ejs *ejs, EjsBlock *block, int slotNum, char *buf, int bufsize);
extern EjsDoc       *ejsCreateDoc(struct Ejs *ejs, EjsBlock *block, int slotNum, cchar *docString);
#endif

#ifdef __cplusplus
}
#endif
#endif /* _h_EJS_MODULE */

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
