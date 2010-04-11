/*
 *  romFiles -- Compiled Files
 */
#include "mpr.h"

#if BLD_FEATURE_ROMFS
static uchar _file_1[] = {
    112,114,105,110,116, 40, 39, 72,101,108,108,111, 32, 87,111,114,
    108,100, 39, 41, 10,
    0 };

MprRomInode romFiles[] = {
    { "hello.es", _file_1, 21, 1 },
    { 0, 0, 0, 0 },
};
#endif /* BLD_FEATURE_ROMFS */
