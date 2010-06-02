
/******************************************************************************/
/* 
 *  This file is an amalgamation of all the individual source code files for
 *  Multithreaded Portable Runtime 3.1.3.
 *
 *  Catenating all the source into a single file makes embedding simpler and
 *  the resulting application faster, as many compilers can do whole file
 *  optimization.
 *
 *  If you want to modify mpr, you can still get the whole source
 *  as individual files if you need.
 */


/************************************************************************/
/*
 *  Start of file "../src/include/mprOs.h"
 */
/************************************************************************/

/*
 *  mprOs.h -- Include O/S headers and smooth out per-O/S differences
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */


/*
 *  This header is part of the Michael's Portable Runtime and aims to include
 *  all necessary O/S headers and to unify the constants and declarations 
 *  required by Embedthis products. It can be included by C or C++ programs.
 */


#ifndef _h_MPR_OS_HDRS
#define _h_MPR_OS_HDRS 1

#include    "buildConfig.h"

/*
 *  Porters, add your CPU families here and update configure code. 
 */
#define MPR_CPU_UNKNOWN     0
#define MPR_CPU_IX86        1
#define MPR_CPU_PPC         2
#define MPR_CPU_SPARC       3
#define MPR_CPU_XSCALE      4
#define MPR_CPU_ARM         5
#define MPR_CPU_MIPS        6
#define MPR_CPU_68K         7
#define MPR_CPU_SIMNT       8           /* VxWorks NT simulator */
#define MPR_CPU_SIMSPARC    9           /* VxWorks sparc simulator */
#define MPR_CPU_IX64        10          /* AMD64 or EMT64 */
#define MPR_CPU_UNIVERSAL   11          /* MAC OS X universal binaries */
#define MPR_CPU_SH4         12


#if BLD_UNIX_LIKE && !VXWORKS && !MACOSX && !FREEBSD
    #include    <sys/types.h>
    #include    <time.h>
    #include    <arpa/inet.h>
    #include    <ctype.h>
    #include    <dirent.h>
    #include    <dlfcn.h>
    #include    <fcntl.h>
    #include    <grp.h> 
    #include    <errno.h>
    #include    <libgen.h>
    #include    <limits.h>
    #include    <netdb.h>
    #include    <net/if.h>
    #include    <netinet/in.h>
    #include    <netinet/tcp.h>
    #include    <netinet/ip.h>
    #include    <pthread.h> 
    #include    <pwd.h> 
    #include    <sys/poll.h>
    #include    <setjmp.h>
    #include    <signal.h>
    #include    <stdarg.h>
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <string.h>
    #include    <syslog.h>
    #include    <sys/ioctl.h>
    #include    <sys/mman.h>
    #include    <sys/stat.h>
    #include    <sys/param.h>
    #if !CYGWIN && !SOLARIS
        #include    <sys/prctl.h>
    #endif
    #include    <sys/resource.h>
    #include    <sys/sem.h>
    #include    <sys/shm.h>
    #include    <sys/socket.h>
    #include    <sys/select.h>
    #include    <sys/time.h>
    #include    <sys/times.h>
    #include    <sys/utsname.h>
    #include    <sys/uio.h>
    #include    <sys/wait.h>
    #include    <unistd.h>
#if LINUX && !__UCLIBC__
    #include    <sys/sendfile.h>
#endif
#if CYGWIN || LINUX
    #include    <stdint.h>
#else
    #include    <netinet/in_systm.h>
#endif
#if BLD_FEATURE_FLOATING_POINT
    #define __USE_ISOC99 1
    #include    <math.h>
#if !CYGWIN
    #include    <resolv.h>
    #include    <values.h>
#endif
#endif
#endif /* BLD_UNIX_LIKE */


#if VXWORKS
    #include    <vxWorks.h>
    #include    <envLib.h>
    #include    <sys/types.h>
    #include    <time.h>
    #include    <arpa/inet.h>
    #include    <ctype.h>
    #include    <dirent.h>
    #include    <fcntl.h>
    #include    <errno.h>
    #include    <iosLib.h>
    #include    <limits.h>
    #include    <loadLib.h>
    #include    <netdb.h>
    #include    <net/if.h>
    #include    <netinet/tcp.h>
    #include    <netinet/in.h>
    #include    <netinet/ip.h>
    #include    <selectLib.h>
    #include    <setjmp.h>
    #include    <signal.h>
    #include    <stdarg.h>
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <string.h>
    #include    <symSyncLib.h>
    #include    <sysSymTbl.h>
    #include    <sys/fcntlcom.h>
    #include    <sys/ioctl.h>
    #include    <sys/stat.h>
    #include    <sys/socket.h>
    #include    <sys/times.h>
    #include    <unistd.h>
    #include    <unldLib.h>
    #if BLD_FEATURE_FLOATING_POINT
    #include    <float.h>
    #define __USE_ISOC99 1
    #include    <math.h>
    #endif
    #include    <sockLib.h>
    #include    <inetLib.h>
    #include    <ioLib.h>
    #include    <pipeDrv.h>
    #include    <hostLib.h>
    #include    <netdb.h>
    #include    <tickLib.h>
    #include    <taskHookLib.h>
#if _WRS_VXWORKS_MAJOR >= 6
    #include    <wait.h>
#endif
#endif /* VXWORKS */


#if MACOSX
    #include    <time.h>
    #include    <arpa/inet.h>
    #include    <ctype.h>
    #include    <dirent.h>
    #include    <dlfcn.h>
    #include    <fcntl.h>
    #include    <grp.h> 
    #include    <errno.h>
    #include    <libgen.h>
    #include    <limits.h>
    #include    <mach-o/dyld.h>
    #include    <netdb.h>
    #include    <net/if.h>
    #include    <netinet/in.h>
    #include    <netinet/tcp.h>
    #include    <sys/poll.h>
    #include    <pthread.h> 
    #include    <pwd.h> 
    #include    <resolv.h>
    #include    <setjmp.h>
    #include    <signal.h>
    #include    <stdarg.h>
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <stdint.h>
    #include    <string.h>
    #include    <syslog.h>
    #include    <sys/ioctl.h>
    #include    <sys/mman.h>
    #include    <sys/types.h>
    #include    <sys/stat.h>
    #include    <sys/param.h>
    #include    <sys/resource.h>
    #include    <sys/sem.h>
    #include    <sys/shm.h>
    #include    <sys/socket.h>
    #include    <sys/select.h>
    #include    <sys/sysctl.h>
    #include    <sys/time.h>
    #include    <sys/times.h>
    #include    <sys/types.h>
    #include    <sys/uio.h>
    #include    <sys/utsname.h>
    #include    <sys/wait.h>
    #include    <unistd.h>
    #include    <libkern/OSAtomic.h>
    #if BLD_FEATURE_FLOATING_POINT
    #include    <float.h>
    #define __USE_ISOC99 1
    #include    <math.h>
    #endif
#endif /* MACOSX */


#if FREEBSD
    #include    <time.h>
    #include    <arpa/inet.h>
    #include    <ctype.h>
    #include    <dirent.h>
    #include    <dlfcn.h>
    #include    <fcntl.h>
    #include    <grp.h> 
    #include    <errno.h>
    #include    <libgen.h>
    #include    <limits.h>
    #include    <netdb.h>
    #include    <sys/socket.h>
    #include    <net/if.h>
    #include    <netinet/in_systm.h>
    #include    <netinet/in.h>
    #include    <netinet/tcp.h>
    #include    <netinet/ip.h>
    #include    <pthread.h> 
    #include    <pwd.h> 
    #include    <resolv.h>
    #include    <signal.h>
    #include    <stdarg.h>
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <stdint.h>
    #include    <string.h>
    #include    <syslog.h>
    #include    <sys/ioctl.h>
    #include    <sys/types.h>
    #include    <sys/stat.h>
    #include    <sys/param.h>
    #include    <sys/resource.h>
    #include    <sys/sem.h>
    #include    <sys/shm.h>
    #include    <sys/select.h>
    #include    <sys/time.h>
    #include    <sys/times.h>
    #include    <sys/types.h>
    #include    <sys/utsname.h>
    #include    <sys/wait.h>
    #include    <sys/mman.h>
    #include    <sys/sysctl.h>
    #include    <unistd.h>
    #include    <poll.h>
#if BLD_FEATURE_FLOATING_POINT
    #include    <float.h>
    #define __USE_ISOC99 1
    #include    <math.h>
#endif
#endif /* FREEBSD */

#if WIN
    /*
     *  We replace insecure functions with Embedthis replacements
     */
    #define     _CRT_SECURE_NO_DEPRECATE 1

    /*
     *  Need this to get the the latest winsock APIs
     */
    #ifndef     _WIN32_WINNT
    #define     _WIN32_WINNT 0x501
    #endif

    #include    <winsock2.h>
    #include    <ws2tcpip.h>
    #include    <ctype.h>
    #include    <conio.h>
    #include    <direct.h>
    #include    <errno.h>
    #include    <fcntl.h>
    #include    <io.h>
    #include    <limits.h>
    #include    <malloc.h>
    #include    <process.h>
    #include    <sys/stat.h>
    #include    <sys/types.h>
    #include    <setjmp.h>
    #include    <stddef.h>
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <string.h>
    #include    <stdarg.h>
    #include    <time.h>
    #include    <windows.h>
    #if BLD_FEATURE_FLOATING_POINT
    #include    <math.h>
    #include    <float.h>
    #endif
    #include    <shlobj.h>
    #include    <shellapi.h>
    #include    <wincrypt.h>
    #if BLD_DEBUG
    #include    <crtdbg.h>
    #endif
    #undef     _WIN32_WINNT
#endif /* WIN */


#if WINCE
    #include    <ctype.h>
    #include    <malloc.h>
    #include    <stddef.h>
    #include    <stdio.h>
    #include    <stdlib.h>
    #include    <string.h>
    #include    <stdarg.h>
    #include    <time.h>
    #include    <winsock2.h>
    #include    <windows.h>
    #include    <winbase.h>
    #include    <winuser.h>
    #if BLD_FEATURE_FLOATING_POINT
    #include    <float.h>
    #endif
    #include    <shlobj.h>
    #include    <shellapi.h>
    #include    <wincrypt.h>
#endif /* WINCE */


#if BREW
    #if BLD_FEATURE_FLOATING_POINT
    #warning "Floating point is not supported on Brew"
    #endif
    #if BLD_FEATURE_MULTITHREAD
    #warning "Multithreading is not supported on Brew"
    #endif
    #include    <AEEModGen.h>
    #include    <AEEAppGen.h>
    #include    <BREWVersion.h>

    #if BREW_MAJ_VER == 2
        #ifdef __GNUC__
        #define __inline extern __inline__
        #endif
        #include    <AEENet.h>
        #undef __inline
    #endif

    #include    <AEE.h>
    #include    <AEEBitmap.h>
    #include    <AEEDisp.h>
    #include    <AEEFile.h>
    #include    <AEEHeap.h>
    #include    <AEEImageCtl.h>
    #include    <AEEMedia.h>
    #include    <AEEMediaUtil.h>
    #include    <AEEMimeTypes.h>
    #include    <AEEStdLib.h>
    #include    <AEEShell.h>
    #include    <AEESoundPlayer.h>
    #include    <AEEText.h>
    #include    <AEETransform.h>
    #include    <AEEWeb.h>
    #if BREW_MAJ_VER >= 3
    #include    <AEESMS.h>
    #endif
    #include    <AEETAPI.h>
#endif /* BREW */

/*
 *  Word size and conversions between integer and pointer.
 */
#if __WORDSIZE == 64 || __amd64 || __x86_64 || __x86_64__
    #define MPR_64_BIT 1
    #define ITOP(i)         ((void*) ((int64) i))
    #define PTOI(i)         ((int) ((int64) i))
    #define LTOP(i)         ((void*) ((int64) i))
    #define PTOL(i)         ((int64) i)
#else
    #define MPR_64_BIT 0
    #define ITOP(i)         ((void*) ((int) i))
    #define PTOI(i)         ((int) i)
    #define LTOP(i)         ((void*) ((int) i))
    #define PTOL(i)         ((int64) (int) i)
#endif

/*
 *  Standard const types used by the MPR
 */
typedef unsigned char uchar;
typedef signed char schar;
typedef const char cchar;
typedef const unsigned char cuchar;
typedef const unsigned short cushort;
typedef const void cvoid;
typedef short uni;
typedef const uni cuni;

#ifdef __cplusplus
extern "C" {
#else
    #if !MACOSX
        typedef int bool;
    #endif
#endif

#if BLD_UNIX_LIKE
    typedef pthread_t   MprOsThread;
#elif BLD_CPU_ARCH == MPR_CPU_IX64
    typedef int64       MprOsThread;
#else
    typedef int         MprOsThread;
#endif

#ifndef BITSPERBYTE
#define BITSPERBYTE     (8 * sizeof(char))
#endif

#if !SOLARIS
#define BITS(type)      (BITSPERBYTE * (int) sizeof(type))
#endif

#ifndef MAXINT
#if INT_MAX
    #define MAXINT      INT_MAX
#else
    #define MAXINT      0x7fffffff
#endif
#endif
#ifndef MAXINT64
    #define MAXINT64    INT64(0x7fffffffffffffff)
#endif

/*
 *  Byte orderings
 */
#define MPR_LITTLE_ENDIAN   1
#define MPR_BIG_ENDIAN      2
/*
 *  Current endian ordering
 */
#define MPR_ENDIAN          BLD_ENDIAN

#ifndef max
    #define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
    #define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef PRINTF_ATTRIBUTE
    #if (__GNUC__ >= 3) && !DOXYGEN && BLD_DEBUG
        /** 
         *  Use gcc attribute to check printf fns.  a1 is the 1-based index of the parameter containing the format, 
         *  and a2 the index of the first argument. Note that some gcc 2.x versions don't handle this properly 
         */     
        #define PRINTF_ATTRIBUTE(a1, a2) __attribute__ ((format (__printf__, a1, a2)))
    #else
        #define PRINTF_ATTRIBUTE(a1, a2)
    #endif
#endif

#define MPR_INLINE inline

/*
 *  Optimize expression evaluation code depending if the value is likely or not
 */
#undef likely
#undef unlikely
#if (__GNUC__ >= 3)
    #define likely(x)   __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
#endif

#if !__UCLIBC__ && !CYGWIN && __USE_XOPEN2K
/*
    Disable because Linux spin locks run slower 
    #define BLD_HAS_SPINLOCK    1
 */
#define BLD_HAS_SPINLOCK    0
#endif

#if VXWORKS && (_WRS_VXWORKS_MAJOR <= 5 || _DIAB_TOOL)
#define BLD_HAS_UNNAMED_UNIONS 0
#else
#define BLD_HAS_UNNAMED_UNIONS 1
#endif

#if BLD_CC_DOUBLE_BRACES
    #define  VA_NULL    {{0}}
#else
    #define  VA_NULL    {0}
#endif


#if CYGWIN || LINUX
    __extension__ typedef long long int int64;
    __extension__ typedef unsigned long long int uint64;

#if CYGWIN
    typedef unsigned long ulong;
#endif

    typedef off_t MprOffset;
    typedef intptr_t pint;

    #define INT64(x) (x##LL)
    #define UINT64(x) (x##ULL)

    #define closesocket(x)  close(x)
    #define MPR_BINARY      ""
    #define MPR_TEXT        ""
    #define SOCKET_ERROR    -1
    #define SET_SOCKOPT_CAST void*

#if BLD_FEATURE_FLOATING_POINT
    #define MAX_FLOAT       MAXFLOAT
    #define isNan(f) (f == FP_NAN)
#endif

#if CYGWIN
    #ifndef PTHREAD_MUTEX_RECURSIVE_NP
        #define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
    #endif
    #define __WALL          0
#else
    #define O_BINARY        0
    #define O_TEXT          0

    /*
     *  For some reason it is removed from fedora 6 pthreads.h and only comes in for UNIX96
     */
    extern int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict
        __attr, int *__restrict __kind) __THROW;
    /* 
     *  Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
     *  PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or PTHREAD_MUTEX_DEFAULT).  
     */
    extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind) __THROW;
    extern char **environ;
#endif

    #define LD_LIBRARY_PATH "LD_LIBRARY_PATH"

#endif  /* CYGWIN || LINUX  */


#if VXWORKS
    typedef off_t MprOffset;
    typedef unsigned int uint;
    typedef unsigned long ulong;
    typedef long long int int64;
    typedef unsigned long long int uint64;
    typedef int pint;

    #define HAVE_SOCKLEN_T
    #define INT64(x) (x##LL)
    #define UINT64(x) (x##ULL)

    #define MPR_BINARY      ""
    #define MPR_TEXT        ""
    #define O_BINARY        0
    #define O_TEXT          0
    #define SOCKET_ERROR    -1
    #define MSG_NOSIGNAL    0
    #define __WALL          0

    #define SET_SOCKOPT_CAST char*
    #define closesocket(x) close(x)

#if BLD_FEATURE_FLOATING_POINT
    #define MAX_FLOAT       FLT_MAX
#endif

    #undef R_OK
    #define R_OK    4
    #undef W_OK
    #define W_OK    2
    #undef X_OK
    #define X_OK    1
    #undef F_OK
    #define F_OK    0

    extern int sysClkRateGet();

    #ifndef SHUT_RDWR
        #define SHUT_RDWR 2
    #endif

    #define getpid mprGetpid
    extern uint mprGetpid();

#if _WRS_VXWORKS_MAJOR < 6
    #define NI_MAXHOST      128
    extern STATUS access(cchar *path, int mode);
    typedef int     socklen_t;
    struct sockaddr_storage {
        char        pad[1024];
    };
#else
    /*
     *  This may or may not be necessary - let us know dev@embedthis.com if your system needs this (and why).
     */
    #if _DIAB_TOOL
        #if BLD_HOST_CPU_ARCH == MPR_CPU_PPC
            // #define __va_copy(dest, src) *(dest) = *(src)
            #define __va_copy(dest, src) memcpy((dest), (src), sizeof(va_list))
        #endif
    #endif
    #define HAVE_SOCKLEN_T
#endif
    #if _DIAB_TOOL
    #define inline __inline__
    #endif

extern int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif  /* VXWORKS */


#if MACOSX
    typedef off_t MprOffset;
    typedef unsigned long ulong;

    __extension__ typedef long long int int64;
    __extension__ typedef unsigned long long int uint64;
    typedef intptr_t pint;

    #define INT64(x) (x##LL)
    #define UINT64(x) (x##ULL)

    #define closesocket(x)  close(x)
    #define MPR_BINARY      ""
    #define MPR_TEXT        ""
    #define O_BINARY        0
    #define O_TEXT          0
    #define SOCKET_ERROR    -1
    #define MSG_NOSIGNAL    0
    #define __WALL          0           /* 0x40000000 */
    #define SET_SOCKOPT_CAST void*
    #define PTHREAD_MUTEX_RECURSIVE_NP  PTHREAD_MUTEX_RECURSIVE

#if BLD_FEATURE_FLOATING_POINT
    #define MAX_FLOAT       MAXFLOAT
#endif
    
    /*
     *  Fix for MAC OS X - getenv
     */
    #if !HAVE_DECL_ENVIRON
    #ifdef __APPLE__
        #include <crt_externs.h>
        #define environ (*_NSGetEnviron())
    #else
        extern char **environ;
    #endif
    #endif
    #define LD_LIBRARY_PATH "DYLD_LIBRARY_PATH"
#endif /* MACOSX */


#if FREEBSD
    typedef off_t MprOffset;
    typedef unsigned long ulong;
    typedef intptr_t pint;

    __extension__ typedef long long int int64;
    __extension__ typedef unsigned long long int uint64;
    #define INT64(x) (x##LL)

    #define closesocket(x)  close(x)
    #define MPR_BINARY      ""
    #define MPR_TEXT        ""
    #define O_BINARY        0
    #define O_TEXT          0
    #define SOCKET_ERROR    -1
    #define MPR_DLL_EXT     ".dylib"
    #define __WALL          0
    #define PTHREAD_MUTEX_RECURSIVE_NP  PTHREAD_MUTEX_RECURSIVE

#if BLD_FEATURE_FLOATING_POINT
    #define MAX_FLOAT       MAXFLOAT
#endif

    #define CLD_EXITED 1
    #define CLD_KILLED 2
    #define LD_LIBRARY_PATH "LD_LIBRARY_PATH"

#endif /* FREEBSD */

/*
 *  All windows like systems. Includes WINCE.
 */
#if BLD_WIN_LIKE
    typedef unsigned int uint;
    typedef unsigned long ulong;
    typedef unsigned short ushort;
    typedef __int64 int64;
    typedef unsigned __int64 uint64;
    typedef int64   MprOffset;
    typedef int     uid_t;
    typedef void    *handle;
    typedef char    *caddr_t;
    typedef long    pid_t;
    typedef int     gid_t;
    typedef ushort  mode_t;
    typedef void    *siginfo_t;
    typedef int     socklen_t;
    #if WINCE
        typedef int pint;
    #else
        typedef intptr_t pint;
    #endif

    struct timezone {
      int  tz_minuteswest;      /* minutes W of Greenwich */
      int  tz_dsttime;          /* type of dst correction */
    };

    #define HAVE_SOCKLEN_T
    #define INT64(x) (x##i64)
    #define UINT64(x) (x##Ui64)
    #define MSG_NOSIGNAL    0
    #define MPR_BINARY      "b"
    #define MPR_TEXT        "t"

    #if !WINCE
    #define access      _access
    #define chdir       _chdir
    #define close       _close
    #define fileno      _fileno
    #define fstat       _fstat
    #define getcwd      _getcwd
    #define getpid      _getpid
    #define gettimezone _gettimezone
    #define lseek       _lseek
    #define mkdir(a,b)  _mkdir(a)
    #define open        _open
    #define putenv      _putenv
    #define read        _read
    #define rmdir(a)    _rmdir(a)
    #define stat        _stat
    #define strdup      _strdup
    #define umask       _umask
    #define unlink      _unlink
    #define write       _write
    #endif
    
    #define MPR_TEXT    "t"

    #ifndef R_OK
    #define R_OK    4
    #endif
    #ifndef W_OK
    #define W_OK    2
    #endif
    #ifndef X_OK
    #define X_OK    4
    #endif
    #ifndef F_OK
    #define F_OK    0
    #endif

    #define EPERM           1
    #define ENOENT          2
    #define ESRCH           3
    #define EINTR           4
    #define EIO             5
    #define ENXIO           6
    #define E2BIG           7
    #define ENOEXEC         8
    #define EBADF           9
    #define ECHILD          10
    #define EAGAIN          11

    /*
     *  VS 2010 defines these
     */
    #ifndef EWOULDBLOCK
    #define EWOULDBLOCK     EAGAIN
    #define EINPROGRESS     36
    #define EALREADY        37
    #define ENETDOWN        43
    #define ECONNRESET      44
    #define ECONNREFUSED    45
    #define EADDRNOTAVAIL   49
    #define EISCONN         56
    #define EADDRINUSE      46
    #define ENETUNREACH     51
    #define ECONNABORTED    53
    #endif

    #define ENOMEM          12
    #define EACCES          13
    #define EFAULT          14
    #define EOSERR          15
    #define EBUSY           16
    #define EEXIST          17
    #define EXDEV           18
    #define ENODEV          19
    #define ENOTDIR         20
    #define EISDIR          21
    #define EINVAL          22
    #define ENFILE          23
    #define EMFILE          24
    #define ENOTTY          25
    #define EFBIG           27
    #define ENOSPC          28
    #define ESPIPE          29
    #define EROFS           30
    #define EMLINK          31
    #define EPIPE           32
    #define EDOM            33
    #define ERANGE          34

    #undef SHUT_RDWR
    #define SHUT_RDWR           2
    
#if BLD_FEATURE_FLOATING_POINT
    #define MAX_FLOAT       DBL_MAX
#endif
#ifndef FILE_FLAG_FIRST_PIPE_INSTANCE
    #define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000
#endif

    #define SET_SOCKOPT_CAST cchar*
    #define inline __inline
    #define chmod _chmod
    #define isNan(f) (_isnan(f))

    /*
     *  PHP can't handle these
     */
    #if !BUILDING_PHP
        #define popen _popen
        #define pclose _pclose
    #endif

    /*
     *  When time began
     */
    #define TIME_GENESIS UINT64(11644473600000000)

    extern void     srand48(long);
    extern long     lrand48(void);
    extern long     ulimit(int, ...);
    extern long     nap(long);
    extern int      getuid(void);
    extern int      geteuid(void);

    extern int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif /* WIN_LIKE */


#if WINCE

    typedef void FILE;
    typedef int off_t;

    struct stat {
        int     st_dev;
        int     st_ino;
        ushort  st_mode;
        short   st_nlink;
        short   st_uid;
        short   st_gid;
        int     st_rdev;
        long    st_size;
        time_t  st_atime;
        time_t  st_mtime;
        time_t  st_ctime;
    };

    #ifndef EOF
        #define EOF         -1
    #endif

    #define O_RDONLY        0
    #define O_WRONLY        1
    #define O_RDWR          2
    #define O_NDELAY        0x4
    #define O_NONBLOCK      0x4
    #define O_APPEND        0x8
    #define O_CREAT         0x100
    #define O_TRUNC         0x200
    #define O_TEXT          0x400
    #define O_EXCL          0x800
    #define O_BINARY        0x1000

    /*
     *  stat flags
     */
    #define S_IFMT          0170000 
    #define S_IFDIR         0040000
    #define S_IFCHR         0020000         /* character special */
    #define S_IFIFO         0010000
    #define S_IFREG         0100000
    #define S_IREAD         0000400
    #define S_IWRITE        0000200
    #define S_IEXEC         0000100

    #ifndef S_ISDIR
        #define S_ISDIR(X) (((X) & S_IFMT) == S_IFDIR)
    #endif
    #ifndef S_ISREG
        #define S_ISREG(X) (((X) & S_IFMT) == S_IFREG)
    #endif

    #define STARTF_USESHOWWINDOW 0
    #define STARTF_USESTDHANDLES 0

    /*
     *  Tunable parameters
     */
    #define     BUFSIZ      MPR_BUFSIZE
    #define     PATHSIZE    MPR_MAX_PATH
    #define gethostbyname2(a,b) gethostbyname(a)

    extern int access(cchar *filename, int flags);
    extern int chdir(cchar * dirname);
    extern int chmod(cchar *path, int mode);
    extern int close(int handle);
    extern void exit(int status);
    extern long _get_osfhandle(int handle);
    extern char *getcwd(char* buffer, int maxlen);
    extern char *getenv(cchar *charstuff);
    extern uint getpid();
    extern long lseek(int handle, long offset, int origin);
    extern int mkdir(cchar *dir, int mode);
    extern time_t mktime(struct tm *pt);
    extern int _open_osfhandle(int *handle, int flags);
    extern uint open(cchar *file, int mode,...);
    extern int read(int handle, void *buffer, uint count);
    extern int rename(cchar *from, cchar *to);
    extern int rmdir(cchar * dir);
    extern uint sleep(uint secs);
    extern int stat(cchar *path, struct stat *stat);
    extern char *strdup(char *s);
    extern int write(int handle, cvoid *buffer, uint count);
    extern int umask(int mode);
    extern int unlink(cchar *path);

    extern int errno;

    #undef CreateFile
    #define CreateFile CreateFileA
    WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile);

    #undef CreateProcess
    #define CreateProcess CreateProcessA

    #undef FindFirstFile
    #define FindFirstFile FindFirstFileA
    WINBASEAPI HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

    #undef FindNextFile
    #define FindNextFile FindNextFileA
    WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

    #undef GetModuleFileName
    #define GetModuleFileName GetModuleFileNameA
    WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

    #undef GetModuleHandle
    #define GetModuleHandle GetModuleHandleA
    WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);

    #undef GetProcAddress
    #define GetProcAddress GetProcAddressA
    // extern FARPROC GetProcAddressA(HMODULE module, LPCSTR name);

    #undef GetFileAttributes
    #define GetFileAttributes GetFileAttributesA
    extern DWORD GetFileAttributesA(cchar *path);

    extern void GetSystemTimeAsFileTime(FILETIME *ft);

    #undef LoadLibrary
    #define LoadLibrary LoadLibraryA
    HINSTANCE WINAPI LoadLibraryA(LPCSTR lpLibFileName);

    #define WSAGetLastError GetLastError

    #define _get_timezone getTimezone
    extern int getTimezone(int *secs);

    extern struct tm *localtime_r(const time_t *when, struct tm *tp);
    extern struct tm *gmtime_r(const time_t *t, struct tm *tp);

#endif /* WINCE */


#if SOLARIS
    typedef off_t MprOffset;
    typedef long long int int64;
    typedef unsigned long long int uint64;

    #define INT64(x) (x##LL)
    #define UINT64(x) (x##ULL)

    #define closesocket(x)  close(x)
    #define MPR_BINARY      ""
    #define MPR_TEXT        ""
    #define O_BINARY        0
    #define O_TEXT          0
    #define SOCKET_ERROR    -1
    #define MSG_NOSIGNAL    0
    #define INADDR_NONE     ((in_addr_t) 0xffffffff)
    #define __WALL  0
    #define PTHREAD_MUTEX_RECURSIVE_NP  PTHREAD_MUTEX_RECURSIVE
    #define LD_LIBRARY_PATH "LD_LIBRARY_PATH"
#if BLD_FEATURE_FLOATING_POINT
    #define MAX_FLOAT       MAXFLOAT
#endif

#endif /* SOLARIS */



#if BREW
    typedef off_t MprOffset;
    typedef unsigned int uint;
    typedef unsigned long ulong;
    typedef unsigned short ushort;
    typedef uint    off_t;
    typedef long    pid_t;

    #ifndef _TIME_T_DEFINED
        typedef int64 time_t;
    #endif

    #define O_RDONLY        0
    #define O_WRONLY        1
    #define O_RDWR          2
    #define O_CREAT         0x200
    #define O_TRUNC         0x400
    #define O_BINARY        0
    #define O_TEXT          0x20000
    #define O_EXCL          0x40000
    #define O_APPEND        0x80000

    #define R_OK    4
    #define W_OK    2
    #define X_OK    1
    #define F_OK    0

    #define SEEK_SET    0
    #define SEEK_CUR    1
    #define SEEK_END    2

    #undef  printf
    #define printf DBGPRINTF

    extern int  getpid();
    extern int  isalnum(int c);
    extern int  isalpha(int c);
    extern int  isdigit(int c);
    extern int  islower(int c);
    extern int  isupper(int c);
    extern int  isspace(int c);
    extern int  isxdigit(int c);

    extern uint strlen(cchar *str);
    extern char *strstr(cchar *string, cchar *strSet);
    extern void *memset(cvoid *dest, int c, uint count);
    extern void exit(int status);
    extern char *strpbrk(cchar *str, cchar *set);
    extern uint strspn(cchar *str, cchar *set);
    extern int  tolower(int c);
    extern int  toupper(int c);
    extern void *memcpy(void *dest, cvoid *src, uint count);
    extern void *memmove(void *dest, cvoid *src, uint count);

    extern int  atoi(cchar *str);
    extern void free(void *ptr);
    extern void *malloc(uint size);
    extern void *realloc(void *ptr, uint size);
    extern char *strcat(char *dest, cchar *src);
    extern char *strchr(cchar *str, int c);
    extern int  strcmp(cchar *s1, cchar *s2);
    extern int  strncmp(cchar *s1, cchar *s2, uint count);
    extern char *strcpy(char *dest, cchar *src);
    extern char *strncpy(char *dest, cchar *src, uint count);
    extern char *strrchr(cchar *str, int c);

    #if BREWSIM && BLD_DEBUG
        extern _CRTIMP int __cdecl _CrtCheckMemory(void);
        extern _CRTIMP int __cdecl _CrtSetReportHook();
    #endif

#endif /* BREW */

#ifdef __cplusplus
}
#endif

#endif /* _h_MPR_OS_HDRS */

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
/************************************************************************/
/*
 *  End of file "../src/include/mprOs.h"
 */
/************************************************************************/



/************************************************************************/
/*
 *  Start of file "../src/include/mprTune.h"
 */
/************************************************************************/

/*
 *  mprTune.h - Header for the Michael's Portable Runtime (MPR) Base.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/*
 *  See mpr.dox for additional documentation.
 */


#ifndef _h_MPR_TUNE
#define _h_MPR_TUNE 1


#include    "buildConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Build tuning
 */
#define MPR_TUNE_SIZE       1       /* Tune for size */
#define MPR_TUNE_BALANCED   2       /* Tune balancing speed and size */
#define MPR_TUNE_SPEED      3       /* Tune for speed */

#ifndef BLD_TUNE
#define BLD_TUNE MPR_TUNE_BALANCED
#endif


#if BLD_TUNE == MPR_TUNE_SIZE || DOXYGEN
    /*
     *  Squeeze mode optimizes to reduce memory usage
     */
    #define MPR_MAX_FNAME           256         /**< Reasonable filename size */
    #define MPR_MAX_PATH            512         /**< Reasonable path name size */
    #define MPR_MAX_URL             512         /**< Max URL size. Also request URL size. */
    #define MPR_DEFAULT_STACK       (64 * 1024) /**< Default thread stack size (64K) */
    #define MPR_MAX_STRING          1024        /**< Maximum (stack) string size */
    #define MPR_DEFAULT_ALLOC       64          /**< Default small alloc size */
    #define MPR_DEFAULT_HASH_SIZE   23          /**< Default size of hash table */ 
    #define MPR_MAX_ARGC            128         /**< Reasonable max of args */
    #define MPR_MAX_LOG_STRING      512         /**< Maximum log message */
    #define MPR_BUFSIZE             4096        /**< Reasonable size for buffers */
    #define MPR_BUF_INCR            4096        /**< Default buffer growth inc */
    #define MPR_MAX_BUF             4194304     /**< Max buffer size */
    #define MPR_XML_BUFSIZE         4096        /**< XML read buffer size */
    #define MPR_HTTP_BUFSIZE        4096        /**< HTTP buffer size. Must fit complete HTTP headers */
    #define MPR_SSL_BUFSIZE         4096        /**< SSL has 16K max*/
    #define MPR_LIST_INCR           8           /**< Default list growth inc */
    #define MPR_FILES_HASH_SIZE     29          /** Hash size for rom file system */
    #define MPR_TIME_HASH_SIZE      67          /** Hash size for time token lookup */
    #define MPR_HTTP_MAX_PASS       64          /**< Size of password */
    #define MPR_HTTP_MAX_USER       64          /**< Size of user name */
    #define MPR_HTTP_MAX_SECRET     32          /**< Random bytes to use */
    
#elif BLD_TUNE == MPR_TUNE_BALANCED
    
    /*
     *  Tune balancing speed and size
     */
    #define MPR_MAX_FNAME           256
    #define MPR_MAX_PATH            1024
    #define MPR_MAX_URL             2048
    #define MPR_DEFAULT_STACK       (128 * 1024)
    #define MPR_MAX_STRING          2048
    #define MPR_DEFAULT_ALLOC       256
    #define MPR_DEFAULT_HASH_SIZE   43
    #define MPR_MAX_ARGC            256
    #define MPR_MAX_LOG_STRING      8192
    #define MPR_BUFSIZE             4096
    #define MPR_BUF_INCR            4096
    #define MPR_MAX_BUF             -1
    #define MPR_XML_BUFSIZE         4096
    #define MPR_HTTP_BUFSIZE        4096
    #define MPR_SSL_BUFSIZE         4096
    #define MPR_LIST_INCR           16
    #define MPR_FILES_HASH_SIZE     61
    #define MPR_TIME_HASH_SIZE      89
    
    #define MPR_HTTP_MAX_PASS       128
    #define MPR_HTTP_MAX_USER       64
    #define MPR_HTTP_MAX_SECRET     32
    
#else
    /*
     *  Tune for speed
     */
    #define MPR_MAX_FNAME           1024
    #define MPR_MAX_PATH            2048
    #define MPR_MAX_URL             4096
    #define MPR_DEFAULT_STACK       (256 * 1024)
    #define MPR_MAX_STRING          4096
    #define MPR_DEFAULT_ALLOC       512
    #define MPR_DEFAULT_HASH_SIZE   97
    #define MPR_MAX_ARGC            512
    #define MPR_MAX_LOG_STRING      8192
    #define MPR_BUFSIZE             8192
    #define MPR_MAX_BUF             -1
    #define MPR_XML_BUFSIZE         4096
    #define MPR_HTTP_BUFSIZE        8192
    #define MPR_SSL_BUFSIZE         8192
    #define MPR_LIST_INCR           16
    #define MPR_BUF_INCR            1024
    #define MPR_FILES_HASH_SIZE     61
    #define MPR_TIME_HASH_SIZE      97
    
    #define MPR_HTTP_MAX_PASS       128
    #define MPR_HTTP_MAX_USER       64
    #define MPR_HTTP_MAX_SECRET     32
#endif

/*
 *  Select wakeup port. Port can be any free port number. If this is not free, the MPR will use the next free port.
 */
#define MPR_DEFAULT_BREAK_PORT 9473

#define MPR_MAX_IP_NAME         128             /**< Maximum size of a host name string */
#define MPR_MAX_IP_ADDR         128             /**< Maximum size of an IP address */
#define MPR_MAX_IP_PORT         8               /**< MMaximum size of a port number */

/**< Maximum size of an IP address with port number */
#define MPR_MAX_IP_ADDR_PORT    (MPR_MAX_IP_ADDR + MPR_MAX_IP_PORT + 1) 

/*
 *  Signal sent on Unix to break out of a select call.
 */
#define MPR_WAIT_SIGNAL         (SIGUSR2)

/*
 *  Socket event message
 */
#define MPR_SOCKET_MESSAGE      (WM_USER + 32)

/*
 *  Priorities
 */
#define MPR_BACKGROUND_PRIORITY 15          /**< May only get CPU if idle */
#define MPR_LOW_PRIORITY        25
#define MPR_NORMAL_PRIORITY     50          /**< Normal (default) priority */
#define MPR_HIGH_PRIORITY       75
#define MPR_CRITICAL_PRIORITY   99          /**< May not yield */

#define MPR_EVENT_PRIORITY      50          /**< Normal priority */ 
#define MPR_WORKER_PRIORITY     50          /**< Normal priority */
#define MPR_REQUEST_PRIORITY    50          /**< Normal priority */

#define MPR_TICKS_PER_SEC       1000        /**< Time ticks per second */

/* 
 *  Timeouts
 */
#define MPR_TIMEOUT_CMD         60000       /**< Command Request timeout (60 sec) */
#define MPR_TIMEOUT_HTTP        60000       /**< HTTP Request timeout (60 sec) */
#define MPR_TIMEOUT_SOCKETS     10000       /**< General sockets timeout */
#define MPR_TIMEOUT_LOG_STAMP   3600000     /**< Time between log time stamps (1 hr) */
#define MPR_TIMEOUT_PRUNER      600000      /**< Time between pruner runs (10 min) */
#define MPR_TIMEOUT_START_TASK  2000        /**< Time to start tasks running */
#define MPR_TIMEOUT_STOP_TASK   5000        /**< Time to stop or reap tasks */
#define MPR_TIMEOUT_STOP_THREAD 5000        /**< Time to stop running threads */
#define MPR_TIMEOUT_STOP        5000        /**< Wait when stopping resources */
#define MPR_TIMEOUT_LINGER      2000        /**< Close socket linger timeout */
#define MPR_TIMEOUT_HANDLER     10000       /**< Wait period when removing a wait handler */


/*
 *  Default thread counts
 */
#if BLD_FEATURE_MULTITHREAD || DOXYGEN
#define MPR_DEFAULT_MIN_THREADS 0           /**< Default min threads */
#define MPR_DEFAULT_MAX_THREADS 20          /**< Default max threads */
#else
#define MPR_DEFAULT_MIN_THREADS 0
#define MPR_DEFAULT_MAX_THREADS 0
#endif

/*
 *  Debug control
 */
#define MPR_MAX_BLOCKED_LOCKS   100         /* Max threads blocked on lock */
#define MPR_MAX_RECURSION       15          /* Max recursion with one thread */
#define MPR_MAX_LOCKS           512         /* Total lock count max */
#define MPR_MAX_LOCK_TIME       (60 * 1000) /* Time in msec to hold a lock */

#define MPR_TIMER_TOLERANCE     2           /* Used in timer calculations */
#define MPR_HTTP_TIMER_PERIOD   5000        /* Check for expired HTTP connections */
#define MPR_CMD_TIMER_PERIOD    5000        /* Check for expired commands */

/*
 *  Events
 */
#define MPR_EVENT_TIME_SLICE    20          /* 20 msec */

/*
 *  Maximum number of files
 */
#define MPR_MAX_FILE            256

/*
 *  HTTP
 */
#define MPR_HTTP_RETRIES        (2)

#ifdef __cplusplus
}
#endif

#endif /* _h_MPR_TUNE */


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
/************************************************************************/
/*
 *  End of file "../src/include/mprTune.h"
 */
/************************************************************************/



/************************************************************************/
/*
 *  Start of file "../src/include/mpr.h"
 */
/************************************************************************/

/*
 *  mpr.h -- Header for the Michael's Portable Runtime (MPR).
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/**
 *  @file mpr.h
 *  Michael's Portable Runtime (MPR) is a portable runtime core for embedded applications.
 *  The MPR provides management for logging, error handling, events, files, http, memory, ssl, sockets, strings, 
 *  xml parsing, and date/time functions. It also provides a foundation of safe routines for secure programming, 
 *  that help to prevent buffer overflows and other security threats. It is correctly handles null arguments without
 *  crashing. The MPR is a library and a C API that can be used in both C and C++ programs.
 *  \n\n
 *  The MPR uses by convention a set extended typedefs for common types. These include: bool, cchar, cvoid, uchar, 
 *  short, ushort, int, uint, long, ulong, int64, uint64, float, and double. The cchar type is a const char, 
 *  cvoid is const void, and several types have "u" prefixes to denote unsigned qualifiers.
 *  \n\n
 *  The MPR includes a memory manager to minimize memory leaks and maximize allocation efficiency. It utilizes 
 *  a heap and slab allocators with tree links. All memory allocated is connected to a parent memory block thus forming a
 *  tree. When any block is freed, all child blocks are also freed. Most MPR APIs take a memory parent context 
 *  as the first parameter.
 *  \n\n
 *  Many of these APIs are not thread-safe. If utilizing multithreaded programming on a supporting 
 *      operating system, be careful if you modify or delete the underlying data while accessing the resource 
 *      from another thread.
 */

#ifndef _h_MPR
#define _h_MPR 1




#ifdef __cplusplus
extern "C" {
#endif


struct  Mpr;
struct  MprBlk;
struct  MprBuf;
struct  MprCond;
struct  MprDispatcher;
struct  MprEvent;
struct  MprFile;
struct  MprFileSystem;
struct  MprHeap;
struct  MprHttp;
struct  MprModule;
struct  MprOsService;
struct  MprPath;
struct  MprSocket;
struct  MprSocketService;
struct  MprSsl;
struct  MprWaitService;
struct  MprWaitHandler;
struct  MprXml;

#if BLD_FEATURE_MULTITHREAD
struct  MprMutex;
struct  MprThread;
struct  MprThreadService;
struct  MprWorker;
struct  MprWorkerService;
#endif

/**
 *  Standard MPR return and error codes
 */
#define MPR_ERR_OK                      0       /**< Success */
#define MPR_ERR_BASE                    -1      /*   Base error code */
#define MPR_ERR                         -1      /**< Default error code */
#define MPR_ERR_GENERAL                 -1      /**< General error */
#define MPR_ERR_ABORTED                 -2      /**< Action aborted */
#define MPR_ERR_ALREADY_EXISTS          -3      /**< Item already exists */
#define MPR_ERR_BAD_ARGS                -4      /**< Bad arguments or paramaeters */
#define MPR_ERR_BAD_FORMAT              -5      /**< Bad input format */
#define MPR_ERR_BAD_HANDLE              -6
#define MPR_ERR_BAD_STATE               -7      /**< Module is in a bad state */
#define MPR_ERR_BAD_SYNTAX              -8      /**< Input has bad syntax */
#define MPR_ERR_BAD_TYPE                -9
#define MPR_ERR_BAD_VALUE               -10
#define MPR_ERR_BUSY                    -11
#define MPR_ERR_CANT_ACCESS             -12     /**< Can't access the file or resource */
#define MPR_ERR_CANT_COMPLETE           -13
#define MPR_ERR_CANT_CREATE             -14     /**< Can't create the file or resource */
#define MPR_ERR_CANT_INITIALIZE         -15
#define MPR_ERR_CANT_OPEN               -16     /**< Can't open the file or resource */
#define MPR_ERR_CANT_READ               -17     /**< Can't read from the file or resource */
#define MPR_ERR_CANT_WRITE              -18     /**< Can't write to the file or resource */
#define MPR_ERR_DELETED                 -19
#define MPR_ERR_NETWORK                 -20
#define MPR_ERR_NOT_FOUND               -21
#define MPR_ERR_NOT_INITIALIZED         -22     /**< Module or resource is not initialized */
#define MPR_ERR_NOT_READY               -23
#define MPR_ERR_READ_ONLY               -24     /**< The operation timed out */
#define MPR_ERR_TIMEOUT                 -25
#define MPR_ERR_TOO_MANY                -26
#define MPR_ERR_WONT_FIT                -27
#define MPR_ERR_WOULD_BLOCK             -28
#define MPR_ERR_CANT_ALLOCATE           -29
#define MPR_ERR_NO_MEMORY               -30     /**< Memory allocation error */
#define MPR_ERR_CANT_DELETE             -31
#define MPR_ERR_CANT_CONNECT            -32
#define MPR_ERR_MAX                     -33

/**
 *  Standard logging trace levels are 0 to 9 with 0 being the most verbose. These are ored with the error source
 *  and type flags. The MPR_LOG_MASK is used to extract the trace level from a flags word. We expect most apps
 *  to run with level 2 trace enabled.
 */
#define MPR_ERROR       1       /* Hard error trace level */
#define MPR_WARN        2       /* Soft warning trace level */
#define MPR_CONFIG      2       /* Configuration settings trace level. */
#define MPR_INFO        3       /* Informational trace only */
#define MPR_DEBUG       4       /* Debug information trace level */
#define MPR_VERBOSE     9       /* Highest level of trace */
#define MPR_LEVEL_MASK  0xf     /* Level mask */

/*
 *  Error source flags
 */
#define MPR_ERROR_SRC   0x10    /* Originated from mprError */
#define MPR_LOG_SRC     0x20    /* Originated from mprLog */
#define MPR_ASSERT_SRC  0x40    /* Originated from mprAssert */
#define MPR_FATAL_SRC   0x80    /* Fatal error. Log and exit */

/*
 *  Log message type flags. Specify what kind of log / error message it is. Listener handlers examine this flag
 *  to determine if they should process the message.Assert messages are trapped when in DEV mode. Otherwise ignored.
 */
#define MPR_LOG_MSG     0x100   /* Log trace message - not an error */
#define MPR_ERROR_MSG   0x200   /* General error */
#define MPR_ASSERT_MSG  0x400   /* Assert flags -- trap in debugger */
#define MPR_USER_MSG    0x800   /* User message */

/*
 *  Log output modifiers
 */
#define MPR_RAW         0x1000  /* Raw trace output */

/*
 *  Error line number information.
 */
#define MPR_LINE(s)     #s
#define MPR_LINE2(s)    MPR_LINE(s)
#define MPR_LINE3       MPR_LINE2(__LINE__)
#define MPR_LOC        __FILE__ ":" MPR_LINE3

#define MPR_STRINGIFY(s) #s

#if VXWORKS
    #define MAIN(name, _argc, _argv)  \
        int name(_argc, _argv) { \
            extern int main(); \
            return main(argc, #name); \
        } \
        int main(_argc, _argv)
#elif WINCE
    #define MAIN(name, argc, argv)  \
        APIENTRY WinMain(HINSTANCE inst, HINSTANCE junk, LPWSTR args, int junk2) { \
            extern int main(); \
            main((int) args, #name); \
        } \
        int main(argc, argv)
#else
    #define MAIN(name, argc, argv) int main(argc, argv)
#endif

/**
 *  Trigger a breakpoint.
 *  @description Triggers a breakpoint and traps to the debugger. 
 *  @ingroup Mpr
 */
extern void mprBreakpoint();

#if BLD_FEATURE_ASSERT
    #define mprAssert(C)    if (C) ; else mprStaticAssert(MPR_LOC, #C)
#else
    #define mprAssert(C)    if (1) ; else
#endif

/*
 *  Parameter values for serviceEvents(loopOnce)
 */
#define MPR_LOOP_ONCE           1
#define MPR_LOOP_FOREVER        0

#define MPR_TEST_TIMEOUT        10000       /* Ten seconds */
#define MPR_TEST_LONG_TIMEOUT   300000      /* 5 minutes */
#define MPR_TEST_SHORT_TIMEOUT  200         /* 1/5 sec */
#define MPR_TEST_NAP            50          /* Short timeout to prevent busy waiting */

/**
 *  Memory Allocation Service.
 *  @description The MPR provides a memory manager that sits above malloc. This layer provides arena and slab 
 *  based allocations with a tree structured allocation mechanism. The goal of the layer is to provide 
 *  a fast, secure, scalable memory allocator suited for embedded applications in multithreaded environments. 
 *  \n\n
 *  By using a tree structured network of memory contexts, error recovery in applications and memory freeing becomes
 *  much easier and more reliable. When a memory block is allocated a parent memory block must be specified. When
 *  the parent block is freed, all its children are automatically freed. 
 *  \n\n
 *  The MPR handles memory allocation errors globally. The application can configure a memory limits and redline
 *  so that memory depletion can be proactively detected and handled. This relieves most cost from detecting and
 *  handling allocation errors. 
 *  @stability Evolving
 *  @defgroup MprMem MprMem
 *  @see MprCtx, mprFree, mprRealloc, mprAlloc, mprAllocWithDestructor, mprAllocWithDestructorZeroed, mprAllocZeroed, 
        mprGetParent, mprCreate, mprSetAllocLimits, mprAllocObjWithDestructor, mprAllocObjWithDestructorZeroed,
 *      mprHasAllocError mprResetAllocError, mprMemdup, mprStrndup, mprMemcpy, 
 */
typedef struct MprMem { int dummy; } MprMem;

/**
 *  Memory context type.
 *  @description Blocks of memory are allocated using a memory context as the parent. Any allocated memory block
 *      may serve as the memory context for subsequent memory allocations. Freeing a block via \ref mprFree
 *      will release the allocated block and all child blocks.
 *  @ingroup MprMem
 */
typedef void *MprCtx;

/**
 *  Safe String Module
 *  @description The MPR provides a suite of safe string manipulation routines to help prevent buffer overflows
 *      and other potential security traps.
 *  @see MprString, mprAsprintf, mprAllocStrcpy, mprAtoi, mprItoa, mprMemcpy,
 *      mprPrintf, mprReallocStrcat, mprSprintf, mprStaticPrintf, mprStrLower, mprStrTok, mprStrTrim, mprStrUpper,
 *      mprStrcmpAnyCase, mprStrcmpAnyCaseCount, mprStrcpy, mprStrlen, mprVsprintf, mprPrintfError,
 *      mprStrcat, mprAllocStrcpy, mprReallocStrcat, mprVasprintf
 */
typedef struct MprString { int dummy; } MprString;

/*
   Mode values for mprDtoa
 */
#define MPR_DTOA_ALL_DIGITS         0       /* Return all digits */
#define MPR_DTOA_N_DIGITS           2       /* Return total N digits */
#define MPR_DTOA_N_FRACTION_DIGITS  3       /* Return total fraction digits */

/*
    Flags for mprDtoa
 */
#define MPR_DTOA_EXPONENT_FORM      0x10    /* Result in exponent form (N.NNNNe+NN) */
#define MPR_DTOA_FIXED_FORM         0x20    /* Emit in fixed form (NNNN.MMMM)*/

/**
    Convert a double to ascii
    @param ctx Any memory context allocated by the MPR.
    @param value Value to convert
    @param ndigits Number of digits to render
    @param mode  Modes are:
         0   Shortest string,
         1   Like 0, but with Steele & White stopping rule,
         2   Return ndigits of result,
         3   Number of digits applies after the decimal point.
    @param flags Format flags
 */
extern char *mprDtoa(MprCtx ctx, double value, int ndigits, int mode, int flags);

extern int mprIsInfinite(double value);
extern int mprIsZero(double value);
extern int mprIsNan(double value);

/**
 *  Print a formatted message to the standard error channel
 *  @description This is a secure replacement for fprintf(stderr. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string
 *  @return Returns the number of bytes written
 *  @ingroup MprString
 */
extern int mprPrintfError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Print a message to the applications standard output without allocating memory.
 *  @description This is a secure replacement for printf that will not allocate memory.
 *  @param ctx Any memory context allocated by the MPR. This is used to locate the standard output channel and not
 *      to allocate memory.
 *  @param fmt Printf style format string
 *  @return Returns the number of bytes written
 *  @remarks The maximum output is MPR_MAX_STRING - 1.
 *  @ingroup MprString
 */
extern int mprStaticPrintf(MprCtx ctx, cchar *fmt, ...);

/**
 *  Print a message to the standard error channel without allocating memory.
 *  @description This is a secure replacement for fprintf(stderr that will not allocate memory.
 *  @param ctx Any memory context allocated by the MPR. This is used to locate the standard output channel and not
 *      to allocate memory.
 *  @param fmt Printf style format string
 *  @return Returns the number of bytes written
 *  @remarks The maximum output is MPR_MAX_STRING - 1.
 *  @ingroup MprString
 */
extern int mprStaticPrintfError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Formatted print. This is a secure verion of printf that can handle null args.
 *  @description This is a secure replacement for printf. It can handle null arguments without crashes.
 *      minimal footprint. The MPR can be build without using any printf routines.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string
 *  @return Returns the number of bytes written
 *  @ingroup MprString
 */
extern int mprPrintf(MprCtx ctx, cchar *fmt, ...);

/**
 *  Print a formatted message to a file descriptor
 *  @description This is a replacement for fprintf as part of the safe string MPR library. It minimizes 
 *      memory use and uses a file descriptor instead of a File pointer.
 *  @param file MprFile object returned via mprOpen.
 *  @param fmt Printf style format string
 *  @return Returns the number of bytes written
 *  @ingroup MprString
 */
extern int mprFprintf(struct MprFile *file, cchar *fmt, ...);

/**
 *  Format a string into a statically allocated buffer.
 *  @description This call format a string using printf style formatting arguments. A trailing null will 
 *      always be appended. The call returns the size of the allocated string excluding the null.
 *  @param buf Pointer to the buffer.
 *  @param maxSize Size of the buffer.
 *  @param fmt Printf style format string
 *  @return Returns the buffer.
 *  @ingroup MprString
 */
extern char *mprSprintf(char *buf, int maxSize, cchar *fmt, ...);

/**
 *  Format a string into a statically allocated buffer.
 *  @description This call format a string using printf style formatting arguments. A trailing null will 
 *      always be appended. The call returns the size of the allocated string excluding the null.
 *  @param buf Pointer to the buffer.
 *  @param maxSize Size of the buffer.
 *  @param fmt Printf style format string
 *  @param args Varargs argument obtained from va_start.
 *  @return Returns the buffer;
 *  @ingroup MprString
 */
extern char *mprVsprintf(char *buf, int maxSize, cchar *fmt, va_list args);

/**
 *  Convert an integer to a string.
 *  @description This call converts the supplied integer into a string formatted into the supplied buffer.
 *  @param buf Pointer to the buffer that will hold the string.
 *  @param size Size of the buffer.
 *  @param value Integer value to convert
 *  @param radix The base radix to use when encoding the number
 *  @return Returns the number of characters in the string.
 *  @ingroup MprString
 */
extern char *mprItoa(char *buf, int size, int64 value, int radix);

/**
 *  Convert a string to an integer.
 *  @description This call converts the supplied string to an integer using the specified radix (base).
 *  @param str Pointer to the string to parse.
 *  @param radix Base to use when parsing the string
 *  @return Returns the integer equivalent value of the string. 
 *  @ingroup MprString
 */
extern int64 mprAtoi(cchar *str, int radix);

/**
 *  Get the next word token.
 *  @description Split a string into word tokens using the supplied separator.
 *  @param buf Buffer to use to hold the word token
 *  @param bufsize Size of the buffer
 *  @param str Input string to tokenize. Note this cannot be a const string. It will be written.
 *  @param delim String of separator characters to use when tokenizing
 *  @param tok Pointer to a word to hold a pointer to the next token in the original string.
 *  @return Returns the number of bytes in the allocated block.
 *  @ingroup MprString
 */
extern char *mprGetWordTok(char *buf, int bufsize, cchar *str, cchar *delim, cchar **tok);

/**
 *  Safe copy for a block of data.
 *  @description Safely copy a block of data into an existing memory block. The call ensures the destination 
 *      block is not overflowed and returns the size of the block actually copied. This is similar to memcpy, but 
 *      is a safer alternative.
 *  @param dest Pointer to the destination block.
 *  @param destMax Maximum size of the destination block.
 *  @param src Block to copy
 *  @param nbytes Size of the source block
 *  @return Returns the number of characters in the allocated block.
 *  @ingroup MprString
 */
extern int mprMemcpy(void *dest, int destMax, cvoid *src, int nbytes);

/**
 *  Compare two byte strings.
 *  @description Safely compare two byte strings. This is a safe replacement for memcmp.
 *  @param b1 Pointer to the first byte string.
 *  @param b1Len Length of the first byte string.
 *  @param b2 Pointer to the second byte string.
 *  @param b2Len Length of the second byte string.
 *  @return Returns zero if the byte strings are identical. Otherwise returns -1 if the first string is less than the 
 *      second. Returns 1 if the first is greater than the first.
 *  @ingroup MprString
 */
extern int mprMemcmp(cvoid *b1, int b1Len, cvoid *b2, int b2Len);

/**
 *  Catenate strings.
 *  @description Safe replacement for strcat. Catenates a string onto an existing string. This call accepts 
 *      a variable list of strings to append. The list of strings is terminated by a null argument. The call
 *      returns the length of the resulting string. This call is similar to strcat, but it will enforce a 
 *      maximum size for the resulting string and will ensure it is terminated with a null.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param max Maximum size of the new block.
 *  @param src Variable list of strings to append. The final string argument must be null.
 *  @return Returns an allocated string.
 *  @ingroup MprString
 */
extern char *mprStrcat(MprCtx ctx, int max, cchar *src, ...);

extern char *mprStrcatV(MprCtx ctx, int max, cchar *src, va_list args);

/**
 *  Copy a string.
 *  @description Safe replacement for strcpy. Copy a string and ensure the target string is not overflowed. 
 *      The call returns the length of the resultant string or an error code if it will not fit into the target
 *      string. This is similar to strcpy, but it will enforce a maximum size for the copied string and will 
 *      ensure it is terminated with a null.
 *  @param dest Pointer to a pointer that will hold the address of the allocated block.
 *  @param destMax Maximum size of the target string.
 *  @param src String to copy
 *  @return Returns the number of characters in the target string.
 *  @ingroup MprString
 */
extern int mprStrcpy(char *dest, int destMax, cchar *src);

/**
 *  Copy characters from a string.
 *  @description Safe replacement for strncpy. Copy bytes from a string and ensure the target string is not overflowed. 
 *      The call returns the length of the resultant string or an error code if it will not fit into the target
 *      string. This is similar to strcpy, but it will enforce a maximum size for the copied string and will 
 *      ensure it is terminated with a null.
 *  @param dest Pointer to a pointer that will hold the address of the allocated block.
 *  @param destMax Maximum size of the target string.
 *  @param src String to copy
 *  @param count Maximum count of characters to copy
 *  @return Returns the number of characters in the target string.
 *  @ingroup MprString
 */
extern int mprStrcpyCount(char *dest, int destMax, cchar *src, int count);

/**
 *  Find a substring.
 *  @description Locate the first occurrence of pattern in a string, but do not search more than the given length. 
 *  @param str Pointer to the string to search.
 *  @param pattern String pattern to search for.
 *  @param len Count of characters in the pattern to actually search for.
 *  @return Returns the number of characters in the target string.
 *  @ingroup MprString
 */
extern char *mprStrnstr(cchar *str, cchar *pattern, int len);

/**
 *  Compare strings.
 *  @description Compare two strings. This is a safe replacement for strcmp. It can handle null args.
 *  @param str1 First string to compare.
 *  @param str2 Second string to compare.
 *  @return Returns zero if the strings are identical. Return -1 if the first string is less than the second. Return 1
 *      if the first string is greater than the second.
 *  @ingroup MprString
 */
extern int mprStrcmp(cchar *str1, cchar *str2);

/**
 *  Compare strings ignoring case.
 *  @description Compare two strings ignoring case differences. This call operates similarly to strcmp.
 *  @param str1 First string to compare.
 *  @param str2 Second string to compare. 
 *  @return Returns zero if the strings are equivalent, < 0 if s1 sorts lower than s2 in the collating sequence 
 *      or > 0 if it sorts higher.
 *  @ingroup MprString
 */
extern int mprStrcmpAnyCase(cchar *str1, cchar *str2);

/**
 *  Compare strings ignoring case.
 *  @description Compare two strings ignoring case differences for a given string length. This call operates 
 *      similarly to strncmp.
 *  @param str1 First string to compare.
 *  @param str2 Second string to compare.
 *  @param len Length of characters to compare.
 *  @return Returns zero if the strings are equivalent, < 0 if s1 sorts lower than s2 in the collating sequence 
 *      or > 0 if it sorts higher.
 *  @ingroup MprString
 */
extern int mprStrcmpAnyCaseCount(cchar *str1, cchar *str2, int len);

/**
 *  Return the length of a string.
 *  @description Safe replacement for strlen. This call returns the length of a string and tests if the length is 
 *      less than a given maximum.
 *  @param src String to measure.
 *  @param max Maximum length for the string
 *  @return Returns the length of the string or MPR_ERR_WONT_FIT if the length is greater than \a max.
 *  @ingroup MprString
 */
extern int mprStrlen(cchar *src, int max);

/**
 *  Convert a string to lower case.
 *  @description Convert a string to its lower case equivalent.
 *  @param str String to convert.
 *  @return Returns a pointer to the converted string. Will always equal str.
 *  @ingroup MprString
 */
extern char *mprStrLower(char *str);


/**
 *  Convert a string to upper case.
 *  @description Convert a string to its upper case equivalent.
 *  @param str String to convert.
 *  @return Returns a pointer to the converted string. Will always equal str.
 *  @ingroup MprString
 */
extern char *mprStrUpper(char *str);

/**
 *  Trim a string.
 *  @description Trim leading and trailing characters off a string.
 *  @param str String to trim.
 *  @param set String of characters to remove.
 *  @return Returns a pointer to the trimmed string. May not equal \a str. If \a str was dynamically allocated, 
 *      do not call mprFree on the returned trimmed pointer. You must use \a str when calling mprFree.
 *  @ingroup MprString
 */
extern char *mprStrTrim(char *str, cchar *set);

/**
 *  Tokenize a string
 *  @description Split a string into tokens.
 *  @param str String to tokenize.
 *  @param delim String of characters to use as token separators.
 *  @param last Last token pointer.
 *  @return Returns a pointer to the next token.
 *  @ingroup MprString
 */
extern char *mprStrTok(char *str, cchar *delim, char **last);

#if BLD_DEBUG
/*
 *  Just for easy debugging. Adds a "\n" automatically.
 */
extern int print(cchar *fmt, ...);
#endif

/**
 *  Buffer refill callback function
 *  @description Function to call when the buffer is depleted and needs more data.
 *  @param buf Instance of an MprBuf
 *  @param arg Data argument supplied to #mprSetBufRefillProc
 *  @returns The callback should return 0 if successful, otherwise a negative error code.
 *  @ingroup MprBuf
 */
typedef int (*MprBufProc)(struct MprBuf* bp, void *arg);

/**
 *  Dynamic Buffer Module
 *  @description MprBuf is a flexible, dynamic growable buffer structure. It has start and end pointers to the
 *      data buffer which act as read/write pointers. Routines are provided to get and put data into and out of the
 *      buffer and automatically advance the appropriate start/end pointer. By definition, the buffer is empty when
 *      the start pointer == the end pointer. Buffers can be created with a fixed size or can grow dynamically as 
 *      more data is added to the buffer. 
 *  \n\n
 *  For performance, the specification of MprBuf is deliberately exposed. All members of MprBuf are implicitly public.
 *  However, it is still recommended that wherever possible, you use the accessor routines provided.
 *  @stability Evolving.
 *  @see MprBuf, mprCreateBuf, mprSetBufMax, mprStealBuf, mprAdjustBufStart, mprAdjustBufEnd, mprCopyBufDown,
 *      mprFlushBuf, mprGetCharFromBuf, mprGetBlockFromBuf, mprGetBufLength, mprGetBufOrigin, mprGetBufSize,
 *      mprGetBufEnd, mprGetBufSpace, mprGetGrowBuf, mprGrowBuf, mprInsertCharToBuf,
 *      mprLookAtNextCharInBuf, mprLookAtLastCharInBuf, mprPutCharToBuf, mprPutBlockToBuf, mprPutIntToBuf,
 *      mprPutStringToBuf, mprPutFmtToBuf, mprRefillBuf, mprResetBufIfEmpty, mprSetBufSize, mprGetBufRefillProc,
 *      mprSetBufRefillProc, mprFree, MprBufProc
 *  @defgroup MprBuf MprBuf
 */
typedef struct MprBuf {
    char            *data;              /**< Actual buffer for data */
    char            *endbuf;            /**< Pointer one past the end of buffer */
    char            *start;             /**< Pointer to next data char */
    char            *end;               /**< Pointer one past the last data chr */
    int             buflen;             /**< Current size of buffer */
    int             maxsize;            /**< Max size the buffer can ever grow */
    int             growBy;             /**< Next growth increment to use */
    MprBufProc      refillProc;         /**< Auto-refill procedure */
    void            *refillArg;         /**< Refill arg */
} MprBuf;


/**
 *  Create a new buffer
 *  @description Create a new buffer. Use mprFree to free the buffer
 *  @param ctx Any memory context allocated by the MPR
 *  @param initialSize Initial size of the buffer
 *  @param maxSize Maximum size the buffer can grow to
 *  @return a new buffer
 *  @ingroup MprBuf
 */
extern MprBuf *mprCreateBuf(MprCtx ctx, int initialSize, int maxSize);

/**
 *  Set the maximum buffer size
 *  @description Update the maximum buffer size set when the buffer was created
 *  @param buf Buffer created via mprCreateBuf
 *  @param maxSize New maximum size the buffer can grow to
 *  @ingroup MprBuf
 */
extern void mprSetBufMax(MprBuf *buf, int maxSize);

/**
 *  Steal the buffer memory from a buffer
 *  @description Steal ownership of the buffer memory from the buffer structure. All MPR memory is owned by a 
 *      memory context and the contents of the buffer is owned by the MprBuf object. Stealing the buffer content 
 *      memory is useful to preserve the buffer contents after the buffer is freed
 *  @param ctx Memory context to won the memory for the buffer
 *  @param buf Buffer created via mprCreateBuf
 *  @return pointer to the buffer contents. Use mprGetBufLength before calling mprStealBuf to determine the resulting
 *      size of the contents.
 *  @ingroup MprBuf
 */
extern char *mprStealBuf(MprCtx ctx, MprBuf *buf);

/**
 *  Add a null character to the buffer contents.
 *  @description Add a null byte but do not change the buffer content lengths. The null is added outside the
 *      "official" content length. This is useful when calling #mprGetBufStart and using the returned pointer 
 *      as a "C" string pointer.
 *  @param buf Buffer created via mprCreateBuf
 *  @ingroup MprBuf
 */
extern void mprAddNullToBuf(MprBuf *buf);

/**
 *  Adjust the buffer start position
 *  @description Adjust the buffer start position by the specified amount. This is typically used to advance the
 *      start position as content is consumed. Adjusting the start or end position will change the value returned
 *      by #mprGetBufLength. If using the mprGetBlock or mprGetChar routines, adjusting the start position is
 *      done automatically.
 *  @param buf Buffer created via mprCreateBuf
 *  @param count Positive or negative count of bytes to adjust the start position.
 *  @ingroup MprBuf
 */
extern void mprAdjustBufStart(MprBuf *buf, int count);

/**
 *  Adjust the buffer end position
 *  @description Adjust the buffer start end position by the specified amount. This is typically used to advance the
 *      end position as content is appended to the buffer. Adjusting the start or end position will change the value 
 *      returned by #mprGetBufLength. If using the mprPutBlock or mprPutChar routines, adjusting the end position is
 *      done automatically.
 *  @param buf Buffer created via mprCreateBuf
 *  @param count Positive or negative count of bytes to adjust the start position.
 *  @ingroup MprBuf
 */
extern void mprAdjustBufEnd(MprBuf *buf, int count);

/**
 *  Compact the buffer contents
 *  @description Compact the buffer contents by copying the contents down to start the the buffer origin.
 *  @param buf Buffer created via mprCreateBuf
 *  @ingroup MprBuf
 */
extern void mprCompactBuf(MprBuf *buf);

/**
 *  Flush the buffer contents
 *  @description Discard the buffer contents and reset the start end content pointers.
 *  @param buf Buffer created via mprCreateBuf
 *  @ingroup MprBuf
 */
extern void mprFlushBuf(MprBuf *buf);

/**
 *  Get a character from the buffer
 *  @description Get the next byte from the buffer start and advance the start position.
 *  @param buf Buffer created via mprCreateBuf
 *  @return The character or -1 if the buffer is empty.
 *  @ingroup MprBuf
 */
extern int mprGetCharFromBuf(MprBuf *buf);

/**
 *  Get a block of data from the buffer
 *  @description Get a block of data from the buffer start and advance the start position. If the requested
 *      length is greater than the available buffer content, then return whatever data is available.
 *  @param buf Buffer created via mprCreateBuf
 *  @param blk Destination block for the read data. 
 *  @param count Count of bytes to read from the buffer.
 *  @return The count of bytes rread into the block or -1 if the buffer is empty.
 *  @ingroup MprBuf
 */
extern int mprGetBlockFromBuf(MprBuf *buf, char *blk, int count);

/**
 *  Get the buffer content length.
 *  @description Get the length of the buffer contents. This is not the same as the buffer size which may be larger.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns The length of the content stored in the buffer.
 *  @ingroup MprBuf
 */
extern int mprGetBufLength(MprBuf *buf);

/**
 *  Get the origin of the buffer content storage.
 *  @description Get a pointer to the start of the buffer content storage. This may not be equal to the start of
 *      the buffer content if #mprAdjustBufStart has been called. Use #mprGetBufSize to determine the length
 *      of the buffer content storage array. 
 *  @param buf Buffer created via mprCreateBuf
 *  @returns A pointer to the buffer content storage.
 *  @ingroup MprBuf
 */
extern char *mprGetBufOrigin(MprBuf *buf);

/**
 *  Get the current size of the buffer content storage.
 *  @description This returns the size of the memory block allocated for storing the buffer contents.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns The size of the buffer content storage.
 *  @ingroup MprBuf
 */
extern int mprGetBufSize(MprBuf *buf);

/**
 *  Get the space available to store content
 *  @description Get the number of bytes available to store content in the buffer
 *  @param buf Buffer created via mprCreateBuf
 *  @returns The number of bytes available
 *  @ingroup MprBuf
 */
extern int mprGetBufSpace(MprBuf *buf);

/**
 *  Get the start of the buffer contents
 *  @description Get a pointer to the start of the buffer contents. Use #mprGetBufLength to determine the length
 *      of the content. Use #mprGetBufEnd to get a pointer to the location after the end of the content.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns Pointer to the start of the buffer data contents
 *  @ingroup MprBuf
 */
extern char *mprGetBufStart(MprBuf *buf);

/**
 *  Get a reference to the end of the buffer contents
 *  @description Get a pointer to the location immediately after the end of the buffer contents.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns Pointer to the end of the buffer data contents. Points to the location one after the last data byte.
 *  @ingroup MprBuf
 */
extern char *mprGetBufEnd(MprBuf *buf);

/**
 *  Grow the buffer
 *  @description Grow the storage allocated for content for the buffer. The new size must be less than the maximum
 *      limit specified via #mprCreateBuf or #mprSetBufSize.
 *  @param buf Buffer created via mprCreateBuf
 *  @param count Count of bytes by which to grow the buffer content size. 
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprGrowBuf(MprBuf *buf, int count);

/**
 *  Insert a character into the buffer
 *  @description Insert a character into to the buffer prior to the current buffer start point.
 *  @param buf Buffer created via mprCreateBuf
 *  @param c Character to append.
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprInsertCharToBuf(MprBuf *buf, int c);

extern int mprPutPadToBuf(MprBuf *buf, int c, int count);
extern int mprPutSubStringToBuf(MprBuf *buf, cchar *str, int count);

/**
 *  Peek at the next character in the buffer
 *  @description Non-destructively return the next character from the start position in the buffer. 
 *      The character is returned and the start position is not altered.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprLookAtNextCharInBuf(MprBuf *buf);

/**
 *  Peek at the last character in the buffer
 *  @description Non-destructively return the last character from just prior to the end position in the buffer. 
 *      The character is returned and the end position is not altered.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprLookAtLastCharInBuf(MprBuf *buf);

/**
 *  Put a character to the buffer.
 *  @description Append a character to the buffer at the end position and increment the end pointer.
 *  @param buf Buffer created via mprCreateBuf
 *  @param c Character to append
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprPutCharToBuf(MprBuf *buf, int c);

/**
 *  Put a block to the buffer.
 *  @description Append a block of data  to the buffer at the end position and increment the end pointer.
 *  @param buf Buffer created via mprCreateBuf
 *  @param ptr Block to append
 *  @param size Size of block to append
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprPutBlockToBuf(MprBuf *buf, cchar *ptr, int size);

/**
 *  Put an integer to the buffer.
 *  @description Append a integer to the buffer at the end position and increment the end pointer.
 *  @param buf Buffer created via mprCreateBuf
 *  @param i Integer to append to the buffer
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprPutIntToBuf(MprBuf *buf, int i);

/**
 *  Put a string to the buffer.
 *  @description Append a null terminated string to the buffer at the end position and increment the end pointer.
 *  @param buf Buffer created via mprCreateBuf
 *  @param str String to append
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprPutStringToBuf(MprBuf *buf, cchar *str);

/**
 *  Put a formatted string to the buffer.
 *  @description Format a string and Append to the buffer at the end position and increment the end pointer.
 *  @param buf Buffer created via mprCreateBuf
 *  @param fmt Printf style format string
 *  @param ... Variable arguments for the format string
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprPutFmtToBuf(MprBuf *buf, cchar *fmt, ...);

/**
 *  Refill the buffer with data
 *  @description Refill the buffer by calling the refill procedure specified via #mprSetBufRefillProc
 *  @param buf Buffer created via mprCreateBuf
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprRefillBuf(MprBuf *buf);

/**
 *  Reset the buffer
 *  @description If the buffer is empty, reset the buffer start and end pointers to the beginning of the buffer.
 *  @param buf Buffer created via mprCreateBuf
 *  @ingroup MprBuf
 */
extern void mprResetBufIfEmpty(MprBuf *buf);

/**
 *  Set the buffer size
 *  @description Set the current buffer content size and maximum size limit. Setting a current size will
 *      immediately grow the buffer to be this size. If the size is less than the current buffer size, 
 *      the requested size will be ignored. ie. this call will not shrink the buffer. Setting a maxSize 
 *      will define a maximum limit for how big the buffer contents can grow. Set either argument to 
 *      -1 to be ignored.
 *  @param buf Buffer created via mprCreateBuf
 *  @param size Size to immediately make the buffer. If size is less than the current buffer size, it will be ignored.
 *      Set to -1 to ignore this parameter.
 *  @param maxSize Maximum size the buffer contents can grow to.
 *  @returns Zero if successful and otherwise a negative error code 
 *  @ingroup MprBuf
 */
extern int mprSetBufSize(MprBuf *buf, int size, int maxSize);

/**
 *  Get the buffer refill procedure
 *  @description Return the buffer refill callback function.
 *  @param buf Buffer created via mprCreateBuf
 *  @returns The refill call back function if defined.
 *  @ingroup MprBuf
 */
extern MprBufProc mprGetBufRefillProc(MprBuf *buf);

/**
 *  Set the buffer refill procedure
 *  @description Define a buffer refill procedure. The MprBuf module will not invoke or manage this refill procedure.
 *      It is simply stored to allow upper layers to use and provide their own auto-refill mechanism.
 *  @param buf Buffer created via mprCreateBuf
 *  @param fn Callback function to store.
 *  @param arg Callback data argument.
 *  @ingroup MprBuf
 */
extern void mprSetBufRefillProc(MprBuf *buf, MprBufProc fn, void *arg);

/*
 *  Format a date according to RFC822: (Fri, 07 Jan 2003 12:12:21 PDT)
 */
#define MPR_RFC_DATE        "%a, %d %b %Y %T %Z"
#define MPR_DEFAULT_DATE    "%a %b %d %T %Y %Z"

/**
 *  Date and Time Service
 *  @stability Evolving
 *  @see MprTime, mprDecodeLocalTime, mprDecodeUniversalTime, mprFormatLocalTime, mprFormatTime, mprParseTime
 *  @defgroup MprDate MprDate
 */
typedef struct MprDate { int dummy; } MprDate;

/**
 *  Mpr time structure.
 *  @description MprTime is the cross platform time abstraction structure. Time is stored as milliseconds
 *      since the epoch: 00:00:00 UTC Jan 1 1970. MprTime is typically a 64 bit quantity.
 *  @ingroup MprDate
 */
typedef int64 MprTime;
struct tm;

extern int mprCompareTime(MprTime t1, MprTime t2);
extern int mprCreateTimeService(MprCtx ctx);

/**
 *  Decode a time value into a tokenized local time value.
 *  @description Safe replacement for localtime. This call converts the time value to local time and formats 
 *      the as a struct tm.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param timep Pointer to a tm structure to hold the result
 *  @param time Time to format
 *  @ingroup MprDate
 */
extern void mprDecodeLocalTime(MprCtx ctx, struct tm *timep, MprTime time);

/**
 *  Decode a time value into a tokenized UTC time structure.
 *  @description Safe replacement for gmtime. This call converts the supplied time value
 *      to UTC time and parses the result into a tm structure.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param timep Pointer to a tm structure to hold the result.
 *  @param time The time to format
 *  @ingroup MprDate
 */
extern void mprDecodeUniversalTime(MprCtx ctx, struct tm *timep, MprTime time);

/**
 *  Convert a time value to local time and format as a string.
 *  @description Safe replacement for ctime. This call formats the time value supplied via \a timep.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param time Time to format. Use mprGetTime to retrieve the current time.
 *  @return The formatting time string
 *  @ingroup MprDate
 */
extern char * mprFormatLocalTime(MprCtx ctx, MprTime time);

/**
 *  Format a time value as a local time.
 *  @description This call formats the time value supplied via \a timep.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param fmt The time format to use.
 *  @param timep The time value to format.
 *  @return The formatting time string. Caller should free.
 *  @ingroup MprDate
 */
extern char *mprFormatTime(MprCtx ctx, cchar *fmt, struct tm *timep);

/**
 *  Get the system time.
 *  @description Get the system time in milliseconds.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @return Returns the time in milliseconds since boot.
 *  @ingroup MprDate
 */
extern MprTime  mprGetTime(MprCtx ctx);

/**
 *  Return the time remaining until a timeout has elapsed
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param mark Starting time stamp 
 *  @param timeout Time in milliseconds
 *  @return Time in milliseconds until the timeout elapses  
 *  @ingroup MprDate
 */
extern MprTime  mprGetRemainingTime(MprCtx ctx, MprTime mark, uint timeout);

/**
 *  Get the elapsed time since a time mark. Create the time mark with mprGetTime()
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param mark Starting time stamp 
 *  @returns the time elapsed since the mark was taken.
 */
extern MprTime  mprGetElapsedTime(MprCtx ctx, MprTime mark);

/*
 *  Convert a time structure into a time value
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param timep Pointer to a time structure
 *  @return a time value
 */
extern MprTime  mprMakeTime(MprCtx ctx, struct tm *timep);

/*
 *  Convert a time structure into a time value using UTC time.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param timep Pointer to a time structure
 *  @return a time value
 */
MprTime mprMakeUniversalTime(MprCtx ctx, struct tm *tm);

/*
    Constants for mprParseTime
 */
#define MPR_LOCAL_TIMEZONE     MAXINT   /* Use local timezone */
#define MPR_UTC_TIMEZONE        0       /* Use UTC timezone */

/*
 *  Parse a string into a time value
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param time Pointer to a time value to receive the parsed time value
 *  @param dateString String to parse
 *  @param timezone Timezone in which to interpret the date
 *  @param defaults Date default values to use for missing components
 *  @returns Zero if successful
 */
extern int mprParseTime(MprCtx ctx, MprTime *time, cchar *dateString, int timezone, struct tm *defaults);

extern int mprGetTimeZoneOffset(MprCtx ctx, MprTime when);

/**
 *  List Module.
 *  @description The MprList is a dynamic growable list suitable for storing pointers to arbitrary objects.
 *  @stability Evolving.
 *  @see MprList, mprAddItem, mprGetItem, mprCreateList, mprClearList, mprLookupItem, mprFree, 
 *      mprGetFirstItem, mprGetListCapacity, mprGetListCount, mprGetNextItem, mprGetPrevItem, 
 *      mprRemoveItem, mprRemoveItemByIndex, mprRemoveRangeOfItems, mprAppendList, mprSortList, 
 *      mprDupList, MprListCompareProc, mprFree, mprCreateKeyPair
 *  @defgroup MprList MprList
 */
typedef struct MprList {
    void    **items;                    /**< List item data */
    int     length;                     /**< Current length of the list contents */
    int     capacity;                   /**< Current list size */ 
    int     maxSize;                    /**< Maximum capacity */
} MprList;

/**
 *  List comparison procedure for sorting
 *  @description Callback function signature used by #mprSortList
 *  @param arg1 First list item to compare
 *  @param arg2 Second list item to compare
 *  @returns Return zero if the items are equal. Return -1 if the first arg is less than the second. Otherwise return 1.
 *  @ingroup MprList
 */
typedef int (*MprListCompareProc)(cvoid *arg1, cvoid *arg2);

/**
 *  Add an item to a list
 *  @description Add the specified item to the list. The list must have been previously created via 
 *      mprCreateList. The list will grow as required to store the item
 *  @param list List pointer returned from #mprCreateList
 *  @param item Pointer to item to store
 *  @return Returns a positive integer list index for the inserted item. If the item cannot be inserted due 
 *      to a memory allocation failure, -1 is returned
 *  @ingroup MprList
 */
extern int mprAddItem(MprList *list, cvoid *item);

/**
 *  Append a list
 *  @description Append the contents of one list to another. The list will grow as required to store the item
 *  @param list List pointer returned from #mprCreateList
 *  @param add List whose contents are added
 *  @return Returns a pointer to the original list if successful. Returns NULL on memory allocation errors.
 *  @ingroup MprList
 */
extern MprList *mprAppendList(MprList *list, MprList *add);

/**
 *  Create a list.
 *  @description Creates an empty list. MprList's can store generic pointers. They automatically grow as 
 *      required when items are added to the list. Callers should invoke mprFree when finished with the
 *      list to release allocated storage.
 *  @param ctx Any memory context allocated by the MPR.
 *  @return Returns a pointer to the list. 
 *  @ingroup MprList
 */
extern MprList *mprCreateList(MprCtx ctx);

/**
 *  Copy a list
 *  @description Copy the contents of a list into an existing list. The destination list is cleared first and 
 *      has its dimensions set to that of the source clist.
 *  @param dest Destination list for the copy
 *  @param src Source list
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprList
 */
extern int mprCopyList(MprList *dest, MprList *src);

/**
 *  Duplicate a list
 *  @description Copy the contents of a list into a new list. 
 *  @param ctx Memory context from which to allocate the list. See #mprAlloc
 *  @param src Source list to copy
 *  @return Returns a new list reference
 *  @ingroup MprList
 */
extern MprList *mprDupList(MprCtx ctx, MprList *src);

/**
 *  Clears the list of all items.
 *  @description Resets the list length to zero and clears all items. Existing items are not freed, they 
 *      are only removed from the list.
 *  @param list List pointer returned from mprCreateList.
 *  @ingroup MprList
 */
extern void mprClearList(MprList *list);

/**
 *  Find an item and return its index.
 *  @description Search for an item in the list and return its index.
 *  @param list List pointer returned from mprCreateList.
 *  @param item Pointer to value stored in the list.
 *  @ingroup MprList
 */
extern int mprLookupItem(MprList *list, cvoid *item);

/**
 *  Get the first item in the list.
 *  @description Returns the value of the first item in the list. After calling this routine, the remaining 
 *      list items can be walked using mprGetNextItem.
 *  @param list List pointer returned from mprCreateList.
 *  @ingroup MprList
 */
extern void *mprGetFirstItem(MprList *list);

/**
 *  Get the last item in the list.
 *  @description Returns the value of the last item in the list. After calling this routine, the remaining 
 *      list items can be walked using mprGetPrevItem.
 *  @param list List pointer returned from mprCreateList.
 *  @ingroup MprList
 */
extern void *mprGetLastItem(MprList *list);

/**
 *  Get an list item.
 *  @description Get an list item specified by its index.
 *  @param list List pointer returned from mprCreateList.
 *  @param index Item index into the list. Indexes have a range from zero to the lenghth of the list - 1.
 *  @ingroup MprList
 */
extern void *mprGetItem(MprList *list, int index);

/**
 *  Get the current capacity of the list.
 *  @description Returns the capacity of the list. This will always be equal to or greater than the list length.
 *  @param list List pointer returned from mprCreateList.
 *  @ingroup MprList
 */
extern int mprGetListCapacity(MprList *list);

/**
 *  Get the number of items in the list.
 *  @description Returns the number of items in the list. This will always be less than or equal to the list capacity.
 *  @param list List pointer returned from mprCreateList.
 *  @ingroup MprList
 */
extern int mprGetListCount(MprList *list);

/**
 *  Get the next item in the list.
 *  @description Returns the value of the next item in the list. Before calling
 *      this routine, mprGetFirstItem must be called to initialize the traversal of the list.
 *  @param list List pointer returned from mprCreateList.
 *  @param lastIndex Pointer to an integer that will hold the last index retrieved.
 *  @ingroup MprList
 */
extern void *mprGetNextItem(MprList *list, int *lastIndex);

/**
 *  Get the previous item in the list.
 *  @description Returns the value of the previous item in the list. Before 
 *      calling this routine, mprGetFirstItem and/or mprGetNextItem must be
 *      called to initialize the traversal of the list.
 *  @param list List pointer returned from mprCreateList.
 *  @param lastIndex Pointer to an integer that will hold the last index retrieved.
 *  @ingroup MprList
 */
extern void *mprGetPrevItem(MprList *list, int *lastIndex);

/**
 *  Initialize a list structure
 *  @description If a list is statically declared inside another structure, mprInitList can be used to 
 *      initialize it before use.
 *  @param list Reference to the MprList struct.
 *  @ingroup MprList
 */
extern void mprInitList(MprList *list);

/**
 *  Insert an item into a list at a specific position
 *  @description Insert the item into the list before the specified position. The list will grow as required 
 *      to store the item
 *  @param list List pointer returned from #mprCreateList
 *  @param index Location at which to store the item. The previous item at this index is moved up to make room.
 *  @param item Pointer to item to store
 *  @return Returns the position index (positive integer) if successful. If the item cannot be inserted due 
 *      to a memory allocation failure, -1 is returned
 *  @ingroup MprList
 */
extern int mprInsertItemAtPos(MprList *list, int index, cvoid *item);

/**
 *  Remove an item from the list
 *  @description Search for a specified item and then remove it from the list.
 *      Existing items are not freed, they are only removed from the list.
 *  @param list List pointer returned from mprCreateList.
 *  @param item Item pointer to remove. 
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprList
 */
extern int mprRemoveItem(MprList *list, void *item);

/**
 *  Remove an item from the list
 *  @description Removes the element specified by \a index, from the list. The
 *      list index is provided by mprInsertItem.
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprList
 */
extern int mprRemoveItemAtPos(MprList *list, int index);

/**
 *  Remove the last item from the list
 *  @description Remove the item at the highest index position.
 *      Existing items are not freed, they are only removed from the list.
 *  @param list List pointer returned from mprCreateList.
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprList
 */
extern int mprRemoveLastItem(MprList *list);

/**
 *  Remove a range of items from the list.
 *  @description Remove a range of items from the list. The range is specified
 *      from the \a start index up to and including the \a end index.
 *  @param list List pointer returned from mprCreateList.
 *  @param start Starting item index to remove (inclusive)
 *  @param end Ending item index to remove (inclusive)
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprList
 */
extern int mprRemoveRangeOfItems(MprList *list, int start, int end);

/**
 *  Set a list item
 *  @description Update the list item stored at the specified index
 *  @param list List pointer returned from mprCreateList.
 *  @param index Location to update
 *  @param item Pointer to item to store
 *  @return Returns the old item previously at that location index
 *  @ingroup MprList
 */
extern void *mprSetItem(MprList *list, int index, cvoid *item);

/**
 *  Define the list size limits
 *  @description Define the list initial size and maximum size it can grow to.
 *  @param list List pointer returned from mprCreateList.
 *  @param initialSize Initial size for the list. This call will allocate space for at least this number of items.
 *  @param maxSize Set the maximum limit the list can grow to become.
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprList
 */
extern int mprSetListLimits(MprList *list, int initialSize, int maxSize);

/**
 *  Sort a list
 *  @description Sort a list using the sort ordering dictated by the supplied compare function.
 *  @param list List pointer returned from mprCreateList.
 *  @param compare Comparison function. If null, then a default string comparison is used.
 *  @ingroup MprList
 */
extern void mprSortList(MprList *list, MprListCompareProc compare);

/**
 *  Key value pairs for use with MprList or MprHash
 *  @ingroup MprList
 */
typedef struct MprKeyValue {
    char        *key;               /**< Key string */
    char        *value;             /**< Associated value for the key */
} MprKeyValue;

/**
 *  Create a key / value pair
 *  @description Allocate and initialize a key value pair for use by the MprList or MprHash modules.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param key Key string
 *  @param value Key value string
 *  @returns An initialized MprKeyValue
 *  @ingroup MprList
 */
extern MprKeyValue *mprCreateKeyPair(MprCtx ctx, cchar *key, cchar *value);

/**
 *  Logging Services
 *  @stability Evolving
 *  @defgroup MprLog MprLog
 *  @see mprError, mprLog, mprSetLogHandler, mprSetLogLevel, mprUserError, mprRawLog, mprFatalError, MprLogHandler
 *      mprGetLogHandler, mprMemoryError, mprStaticAssert, mprStaticError
 */
typedef struct MprLog { int dummy; } MprLog;

/**
 *  Log handler callback type.
 *  @description Callback prototype for the log handler. Used by mprSetLogHandler to define 
 *      a message logging handler to process log and error messages. 
 *  @param file Source filename. Derived by using __FILE__.
 *  @param line Source line number. Derived by using __LINE__.
 *  @param flags Error flags.
 *  @param level Message logging level. Levels are 0-9 with zero being the most verbose.
 *  @param msg Message being logged.
 *  @ingroup MprLog
 */
typedef void (*MprLogHandler)(MprCtx ctx, int flags, int level, cchar *msg);

/**
 *  Set an MPR debug log handler.
 *  @description Defines a callback handler for MPR debug and error log messages. When output is sent to 
 *      the debug channel, the log handler will be invoked to accept the output message.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param handler Callback handler
 *  @param handlerData Callback handler data
 *  @ingroup MprLog
 */
extern void mprSetLogHandler(MprCtx ctx, MprLogHandler handler, void *handlerData);

/**
 *  Get the current MPR debug log handler.
 *  @description Get the log handler defined via #mprSetLogHandler
 *  @param ctx Any memory context allocated by the MPR.
 *  @returns A function of the signature #MprLogHandler
 *  @ingroup MprLog
 */
extern MprLogHandler mprGetLogHandler(MprCtx ctx);

/**
 *  Log an error message.
 *  @description Send an error message to the MPR debug logging subsystem. The 
 *      message will be to the log handler defined by #mprSetLogHandler. It 
 *      is up to the log handler to respond appropriately and log the message.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @ingroup MprLog
 */
extern void mprError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Log a fatal error message and exit.
 *  @description Send a fatal error message to the MPR debug logging subsystem and then exit the application by
 *      calling exit(). The message will be to the log handler defined by #mprSetLogHandler. It 
 *      is up to the log handler to respond appropriately and log the message.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @ingroup MprLog
 */
extern void mprFatalError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Log a memory error message.
 *  @description Send a memory error message to the MPR debug logging subsystem. The message will be 
 *      passed to the log handler defined by #mprSetLogHandler. It is up to the log handler to respond appropriately
 *      to the fatal message, the MPR takes no other action other than logging the message. Typically, a memory 
 *      message will be logged and the application will be shutdown. The preferred method of operation is to define
 *      a memory depletion callback via #mprCreate. This will be invoked whenever a memory allocation error occurs.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @ingroup MprLog
 */
extern void mprMemoryError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Write a message to the diagnostic log file.
 *  @description Send a message to the MPR logging subsystem.
 *  @param level Logging level for this message. The level is 0-9 with zero being the most verbose.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @remarks mprLog is highly useful as a debugging aid when integrating or when developing new modules. 
 *  @ingroup MprLog
 */
extern void mprLog(MprCtx ctx, int level, cchar *fmt, ...);

/**
 *  Write a raw log message to the diagnostic log file.
 *  @description Send a raw message to the MPR logging subsystem. Raw messages do not have any application prefix
 *      attached to the message and do not append a newline to the message.
 *  @param level Logging level for this message. The level is 0-9 with zero being the most verbose.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @remarks mprLog is highly useful as a debugging aid when integrating or when developing new modules. 
 *  @ingroup MprLog
 */
extern void mprRawLog(MprCtx ctx, int level, cchar *fmt, ...);

/**
 *  Output an assertion failed message.
 *  @description This will emit an assertion failed message to the standard error output. It will bypass the logging
 *      system.
 *  @param loc Source code location string. Use MPR_LOC to define a file name and line number string suitable for this
 *      parameter.
 *  @param msg Simple string message to output
 *  @ingroup MprLog
 */
extern void mprStaticAssert(cchar *loc, cchar *msg);

/**
 *  Write a message to the diagnostic log file without allocating any memory. Useful for log messages from within the
 *      memory allocator.
 *  @description Send a message to the MPR logging subsystem. This will not allocate any memory while formatting the 
 *      message. The formatted message string will be truncated in size to #MPR_MAX_STRING bytes. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @ingroup MprLog
 */
extern void mprStaticError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Display an error message to the user.
 *  @description Display an error message to the user and then send it to the 
 *      MPR debug logging subsystem. The message will be passed to the log 
 *      handler defined by mprSetLogHandler. It is up to the log handler to 
 *      respond appropriately and display the message to the user.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fmt Printf style format string. Variable number of arguments to 
 *  @param ... Variable number of arguments for printf data
 *  @ingroup MprLog
 */
extern void mprUserError(MprCtx ctx, cchar *fmt, ...);

/**
 *  Hash table entry structure.
 *  @description Each hash entry has a descriptor entry. This is used to manage the hash table link chains.
 *  @see MprHash, mprAddHash, mprAddDuplicateHash, mprCopyHash, mprCreateHash, mprGetFirstHash, mprGetNextHash,
 *      mprGethashCount, mprLookupHash, mprLookupHashEntry, mprRemoveHash, mprFree, mprCreateKeyPair
 *  @stability Evolving.
 *  @defgroup MprHash MprHash
 */
typedef struct MprHash {
    struct MprHash *next;               /**< Next symbol in hash chain */
    char            *key;               /**< Hash key */
    cvoid           *data;              /**< Pointer to symbol data */
    int             bucket;             /**< Hash bucket index */
} MprHash;

/**
 *  Hash table control structure
 */
typedef struct MprHashTable {
    MprHash         **buckets;          /**< Hash collision bucket table */
    int             hashSize;           /**< Size of the buckets array */
    int             count;              /**< Number of symbols in the table */
} MprHashTable;

/**
 *  Add a symbol value into the hash table
 *  @description Associate an arbitrary value with a string symbol key and insert into the symbol table.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @param key String key of the symbole entry to delete.
 *  @param ptr Arbitrary pointer to associate with the key in the table.
 *  @return Integer count of the number of entries.
 *  @ingroup MprHash
 */
extern MprHash *mprAddHash(MprHashTable *table, cchar *key, cvoid *ptr);

/**
 *  Add a duplicate symbol value into the hash table
 *  @description Add a symbol to the hash which may clash with an existing entry. Duplicate symbols can be added to
 *      the hash, but only one may be retrieved via #mprLookupHash. To recover duplicate entries walk the hash using
 *      #mprGetNextHash.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @param key String key of the symbole entry to delete.
 *  @param ptr Arbitrary pointer to associate with the key in the table.
 *  @return Integer count of the number of entries.
 *  @ingroup MprHash
 */
extern MprHash *mprAddDuplicateHash(MprHashTable *table, cchar *key, cvoid *ptr);

/**
 *  Copy a hash table
 *  @description Create a new hash table and copy all the entries from an existing table.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @return A new hash table initialized with the contents of the original hash table.
 *  @ingroup MprHash
 */
extern MprHashTable *mprCopyHash(MprCtx ctx, MprHashTable *table);

/**
 *  Create a hash table
 *  @description Creates a hash table that can store arbitrary objects associated with string key values.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param hashSize Size of the hash table for the symbol table. Should be a prime number.
 *  @return Returns a pointer to the allocated symbol table. Caller should use mprFree to dispose of the table 
 *      when complete.
 *  @ingroup MprHash
 */
extern MprHashTable *mprCreateHash(MprCtx ctx, int hashSize);

/**
 *  Return the first symbol in a symbol entry
 *  @description Prepares for walking the contents of a symbol table by returning the first entry in the symbol table.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @return Pointer to the first entry in the symbol table.
 *  @ingroup MprHash
 */
extern MprHash *mprGetFirstHash(MprHashTable *table);

/**
 *  Return the next symbol in a symbol entry
 *  @description Continues walking the contents of a symbol table by returning
 *      the next entry in the symbol table. A previous call to mprGetFirstSymbol
 *      or mprGetNextSymbol is required to supply the value of the \a last
 *      argument.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @param last Symbol table entry returned via mprGetFirstSymbol or mprGetNextSymbol.
 *  @return Pointer to the first entry in the symbol table.
 *  @ingroup MprHash
 */
extern MprHash *mprGetNextHash(MprHashTable *table, MprHash *last);

/**
 *  Return the count of symbols in a symbol entry
 *  @description Returns the number of symbols currently existing in a symbol table.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @return Integer count of the number of entries.
 *  @ingroup MprHash
 */
extern int mprGetHashCount(MprHashTable *table);

/**
 *  Lookup a symbol in the hash table.
 *  @description Lookup a symbol key and return the value associated with that key.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @param key String key of the symbole entry to delete.
 *  @return Value associated with the key when the entry was inserted via mprInsertSymbol.
 *  @ingroup MprHash
 */
extern cvoid *mprLookupHash(MprHashTable *table, cchar *key);

/**
 *  Lookup a symbol in the hash table and return the hash entry
 *  @description Lookup a symbol key and return the hash table descriptor associated with that key.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @param key String key of the symbole entry to delete.
 *  @return MprHash table structure for the entry
 *  @ingroup MprHash
 */
extern MprHash *mprLookupHashEntry(MprHashTable *table, cchar *key);

/**
 *  Remove a symbol entry from the hash table.
 *  @description Removes a symbol entry from the symbol table. The entry is looked up via the supplied \a key.
 *  @param table Symbol table returned via mprCreateSymbolTable.
 *  @param key String key of the symbole entry to delete.
 *  @return Returns zero if successful, otherwise a negative MPR error code is returned.
 *  @ingroup MprHash
 */
extern int mprRemoveHash(MprHashTable *table, cchar *key);

/*
 *  Prototypes for file system switch methods
 */
typedef bool            (*MprAccessFileProc)(struct MprFileSystem *fs, cchar *path, int omode);
typedef int             (*MprDeleteFileProc)(struct MprFileSystem *fs, cchar *path);
typedef int             (*MprDeleteDirProc)(struct MprFileSystem *fs, cchar *path);
typedef int             (*MprGetPathInfoProc)(struct MprFileSystem *fs, cchar *path, struct MprPath *info);
typedef char            *(*MprGetPathLinkProc)(struct MprFileSystem *fs, cchar *path);
typedef int             (*MprMakeDirProc)(struct MprFileSystem *fs, cchar *path, int perms);
typedef int             (*MprMakeLinkProc)(struct MprFileSystem *fs, cchar *path, cchar *target, int hard);
typedef int             (*MprCloseFileProc)(struct MprFile *file);
typedef int             (*MprReadFileProc)(struct MprFile *file, void *buf, uint size);
typedef long            (*MprSeekFileProc)(struct MprFile *file, int seekType, long distance);
typedef int             (*MprSetBufferedProc)(struct MprFile *file, int initialSize, int maxSize);
typedef int             (*MprWriteFileProc)(struct MprFile *file, cvoid *buf, uint count);

#if !DOXYGEN
/* Work around doxygen bug */
typedef struct MprFile* (*MprOpenFileProc)(MprCtx ctx, struct MprFileSystem *fs, cchar *path, int omode, int perms);
#endif

/**
 *  File system service
 *  @description The MPR provides a file system abstraction to support non-disk based file access such as flash or 
 *      other ROM based file systems. The MprFileSystem structure defines a virtual file system interface that
 *      will be invoked by the various MPR file routines.
 */
typedef struct MprFileSystem {
    MprAccessFileProc   accessPath;     /**< Virtual access file routine */
    MprDeleteFileProc   deletePath;     /**< Virtual delete file routine */
    MprGetPathInfoProc  getPathInfo;    /**< Virtual get file information routine */
    MprGetPathLinkProc  getPathLink;    /**< Virtual get the symbolic link target */
    MprMakeLinkProc     makeLink;       /**< Virtual make directory routine */
    MprMakeDirProc      makeDir;        /**< Virtual make link routine */
    MprOpenFileProc     openFile;       /**< Virtual open file routine */
    MprCloseFileProc    closeFile;      /**< Virtual close file routine */
    MprReadFileProc     readFile;       /**< Virtual read file routine */
    MprSeekFileProc     seekFile;       /**< Virtual seek file routine */
    MprSetBufferedProc  setBuffered;    /**< Virtual set buffered I/O routine */
    MprWriteFileProc    writeFile;      /**< Virtual write file routine */

    struct MprFile      *stdError;      /**< Standard error file */
    struct MprFile      *stdInput;      /**< Standard input file */
    struct MprFile      *stdOutput;     /**< Standard output file */

    bool                caseSensitive;  /**< Path comparisons are case sensitive */
    bool                hasDriveSpecs;  /**< Paths can have drive specifications */
    char                *separators;    /**< Filename path separators. First separator is the preferred separator. */
    char                *newline;       /**< Newline for text files */
    cchar               *root;          /**< Root file path */

#if BLD_WIN_LIKE
    char            *cygdrive;          /**< Cygwin drive root */
#endif

} MprFileSystem;


#if BLD_FEATURE_ROMFS
/*
 *  A RomInode is created for each file in the Rom file system.
 */
typedef struct  MprRomInode {
    char            *path;              /* File path */
    uchar           *data;              /* Pointer to file data */
    int             size;               /* Size of file */
    int             num;                /* Inode number */
} MprRomInode;

typedef struct MprRomFileSystem {
    MprFileSystem   fileSystem;
    MprHashTable    *fileIndex;
    MprRomInode     *romInodes;
    char            *root;
    int             rootLen;
} MprRomFileSystem;
#elif BREW


typedef struct MprBrewFileSystem {
    MprFileSystem   fileSystem;
    IFileMgr        *fileMgr;           /* File manager */
} MprBrewFileSystem;
#else

typedef MprFileSystem MprDiskFileSystem;
#endif

/*
 *  File system initialization routines
 */
extern MprFileSystem *mprCreateFileSystem(MprCtx ctx, cchar *path);
#if BLD_FEATURE_ROMFS
    extern MprRomFileSystem *mprCreateRomFileSystem(MprCtx ctx, cchar *path);
    extern int mprSetRomFileSystem(MprCtx ctx, MprRomInode *inodeList);
#elif BREW
    extern MprBrewFileSystem *mprCreateBrewFileSystem(MprCtx ctx, cchar *path);
#else
    extern MprDiskFileSystem *mprCreateDiskFileSystem(MprCtx ctx, cchar *path);
#endif

extern void mprAddFileSystem(MprCtx ctx, MprFileSystem *fs);
extern MprFileSystem *mprLookupFileSystem(MprCtx ctx, cchar *path);
extern void mprSetPathSeparators(MprCtx ctx, cchar *path, cchar *separators);
extern void mprSetPathNewline(MprCtx ctx, cchar *path, cchar *newline);

/**
 *  File I/O Module
 *  @description MprFile is the cross platform File I/O abstraction control structure. An instance will be
 *       created when a file is created or opened via #mprOpen.
 *  @stability Evolving.
 *  @see MprFile mprClose mprGets mprOpen mprPutc mprPuts mprRead mprSeek mprWrite mprWriteString mprWriteFormat
 *      mprFlush MprFile mprGetc mprDisableFileBuffering mprEnableFileBuffering mprGetFileSize 
 *      mprGetFilePosition mprPeekc
 *
 *  @defgroup MprFile MprFile
 */
typedef struct MprFile {
#if BLD_DEBUG
    cchar           *path;              /**< Filename */
#endif
    MprFileSystem   *fileSystem;        /**< File system owning this file */
    MprBuf          *buf;               /**< Buffer for I/O if buffered */
    MprOffset       pos;                /**< Current read position  */
    MprOffset       iopos;              /**< Raw I/O position  */
    MprOffset       size;               /**< Current file size */
    int             mode;               /**< File open mode */
    int             perms;              /**< File permissions */
#if BLD_FEATURE_ROMFS
    MprRomInode     *inode;             /**< Reference to ROM file */
#endif
#if BREW
    IFile           *fd;                /**< File handle */
#else
    int             fd;                 /**< File handle */
#endif
} MprFile;


/**
 *  Attach to an existing file descriptor
 *  @description Attach a file to an open file decriptor and return a file object.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param fd File descriptor to attach to
 *  @param name Descriptive name for the file.
 *  @param omode Posix style file open mode mask. The open mode may contain 
 *      the following mask values ored together:
 *      @li O_RDONLY Open read only
 *      @li O_WRONLY Open write only
 *      @li O_RDWR Open for read and write
 *      @li O_CREAT Create or re-create
 *      @li O_TRUNC Truncate
 *      @li O_BINARY Open for binary data
 *      @li O_TEXT Open for text data
 *      @li O_EXCL Open with an exclusive lock
 *      @li O_APPEND Open to append
 *  @return Returns an MprFile object to use in other file operations.
 *  @ingroup MprFile
 */
extern MprFile *mprAttachFd(MprCtx ctx, int fd, cchar *name, int omode);

/**
 *  Disable file buffering
 *  @description Disable any buffering of data when using the buffer.
 *  @param file File instance returned from #mprOpen
 *  @ingroup MprFile
 */
extern void mprDisableFileBuffering(MprFile *file);

/**
 *  Enable file buffering
 *  @description Enable data buffering when using the buffer.
 *  @param file File instance returned from #mprOpen
 *  @param size Size to allocate for the buffer.
 *  @param maxSize Maximum size the data buffer can grow to
 *  @ingroup MprFile
 */
extern int mprEnableFileBuffering(MprFile *file, int size, int maxSize);

/**
 *  Flush any buffered write data
 *  @description Write buffered write data and then reset the internal buffers.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @return Zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprFile
 */
extern int mprFlush(MprFile *file);

/**
 *  Return the current file position
 *  @description Return the current read/write file position.
 *  @param file A file object returned from #mprOpen
 *  @returns The current file offset position if successful. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern MprOffset mprGetFilePosition(MprFile *file);

/**
 *  Get the size of the file
 *  @description Return the current file size
 *  @param file A file object returned from #mprOpen
 *  @returns The current file size if successful. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern MprOffset mprGetFileSize(MprFile *file);

/**
 *  Read a line from the file.
 *  @description Read a single line from the file and advance the read position. Lines are delimited by the 
 *      newline character. The newline is not included in the returned buffer.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param buf Pre-allocated buffer to contain the line of data.
 *  @param size Size of \a buf.
 *  @return The number of characters read into \a buf.
 *  @ingroup MprFile
 */
extern char *mprGets(MprFile *file, char *buf, uint size);

/**
 *  Read a character from the file.
 *  @description Read a single character from the file and advance the read position.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @return If successful, return the character just read. Otherwise return a negative MPR error code.
 *      End of file is signified by reading 0.
 *  @ingroup MprFile
 */
extern int mprGetc(MprFile *file);

/**
 *  Return a file object for the Stdout I/O channel
 *  @param ctx Any memory context allocated by the MPR.
 *  @returns A file object
 */
extern MprFile *mprGetStdout(MprCtx ctx);

/**
 *  Return a file object for the Stdin I/O channel
 *  @param ctx Any memory context allocated by the MPR.
 *  @returns A file object
 */
extern MprFile *mprGetStdin(MprCtx ctx);

/**
 *  Return a file object for the Stderr I/O channel
 *  @param ctx Any memory context allocated by the MPR.
 *  @returns A file object
 */
extern MprFile *mprGetStderr(MprCtx ctx);

/**
 *  Open a file
 *  @description Open a file and return a file object.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param filename String containing the filename to open or create.
 *  @param omode Posix style file open mode mask. The open mode may contain 
 *      the following mask values ored together:
 *      @li O_RDONLY Open read only
 *      @li O_WRONLY Open write only
 *      @li O_RDWR Open for read and write
 *      @li O_CREAT Create or re-create
 *      @li O_TRUNC Truncate
 *      @li O_BINARY Open for binary data
 *      @li O_TEXT Open for text data
 *      @li O_EXCL Open with an exclusive lock
 *      @li O_APPEND Open to append
 *  @param perms Posix style file permissions mask.
 *  @return Returns an MprFile object to use in other file operations.
 *  @ingroup MprFile
 */
extern MprFile *mprOpen(MprCtx ctx, cchar *filename, int omode, int perms);

/**
 *  Non-destructively read a character from the file.
 *  @description Read a single character from the file without advancing the read position.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @return If successful, return the character just read. Otherwise return a negative MPR error code.
 *      End of file is signified by reading 0.
 *  @ingroup MprFile
 */
extern int mprPeekc(MprFile *file);

/**
 *  Write a character to the file.
 *  @description Writes a single character to the file. Output is buffered and is
 *      flushed as required or when mprClose is called.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param c Character to write
 *  @return One if successful, otherwise returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern int mprPutc(MprFile *file, int c);

/**
 *  Write a string to the file.
 *  @description Writes a string to the file. Output is buffered and is flushed as required or when mprClose is called.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param str String to write
 *  @return The number of characters written to the file. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern int mprPuts(MprFile *file, cchar *str);

/**
 *  Read data from a file.
 *  @description Reads data from a file. 
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param buf Buffer to contain the read data.
 *  @param size Size of \a buf in characters.
 *  @return The number of characters read from the file. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern int mprRead(MprFile *file, void *buf, uint size);

/**
 *  Seek the I/O pointer to a new location in the file.
 *  @description Move the position in the file to/from which I/O will be performed in the file. Seeking prior 
 *      to a read or write will cause the next I/O to occur at that location.
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param seekType Seek type may be one of the following three values:
 *      @li SEEK_SET    Seek to a position relative to the start of the file
 *      @li SEEK_CUR    Seek relative to the current position
 *      @li SEEK_END    Seek relative to the end of the file
 *  @param distance A positive or negative byte offset.
 *  @return Returns the new file position if successful otherwise a negative MPR error code is returned.
 *  @ingroup MprFile
 */
extern long mprSeek(MprFile *file, int seekType, long distance);

/**
 *  Write data to a file.
 *  @description Writes data to a file. 
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param buf Buffer containing the data to write.
 *  @param count Cound of characters in \a buf to write
 *  @return The number of characters actually written to the file. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern int mprWrite(MprFile *file, cvoid *buf, uint count);

/**
 *  Write a string to a file.
 *  @description Writes a string to a file. 
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param str String to write
 *  @return The number of characters actually written to the file. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern int mprWriteString(MprFile *file, cchar *str);

/**
 *  Write formatted data to a file.
 *  @description Writes a formatted string to a file. 
 *  @param file Pointer to an MprFile object returned via MprOpen.
 *  @param fmt Format string
 *  @return The number of characters actually written to the file. Returns a negative MPR error code on errors.
 *  @ingroup MprFile
 */
extern int mprWriteFormat(MprFile *file, cchar *fmt, ...);


/**
 *  Path (filename) Information
 *  @description MprPath is the cross platform Path (filename) information structure.
 *  @stability Evolving.
 *  @see MprPath mprComparePath mprCopyPath mprDeletePath mprGetAbsPath mprGetCurrentPath
 *      mprGetPathBase mprGetPathDir mprGetPathFiles mprGetPathExtension mprGetPathLink mprGetPathParent 
 *      mprGetPathNewline mprGetPathSeparators mprGetPortablePath mprGetRelPath mprGetTempPath  mprGetTransformedPath
 *      mprIsAbsPath mprIsRelPath mprMakeDir mprMakeLink mprGetNormalizedPath mprJoinPath mprJoinPathExt mprMapSeparators 
 *      mprPathExists mprSearchPath mprTruncatePath mprTrimExtension
 *      MprFile
 *  @defgroup MprPath MprPath
 */
typedef struct MprPath {
    MprTime         atime;              /**< Access time */
    MprTime         ctime;              /**< Create time */
    MprTime         mtime;              /**< Modified time */
    int64           size;               /**< File length */
    uint            inode;              /**< Inode number */
    bool            isDir;              /**< Set if directory */
    bool            isLink;             /**< Set if symbolic link */
    bool            isReg;              /**< Set if a regular file */
    bool            caseMatters;        /**< Case comparisons matter */
    int             perms;              /**< Permission mask */
    int             valid;              /**< Valid data bit */
    int             checked;            /**< Path has been checked */
} MprPath;

/**
 *  Directory entry description
 *  @description The MprGetDirList will create a list of directory entries.
 */
typedef struct MprDirEntry {
    char            *name;              /**< Name of the file */
    MprTime         lastModified;       /**< Time the file was last modified */
    MprOffset       size;               /**< Size of the file */
    bool            isDir;              /**< True if the file is a directory */
    bool            isLink;             /**< True if the file symbolic link */
} MprDirEntry;

/**
 *  Copy a file
 *  @description Create a new copy of a file with the specified open permissions mode.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param from Path of the existing file to copy
 *  @param to Name of the new file copy
 *  @param omode Posix style file open mode mask. See #mprOpen for the various modes.
 *  @returns True if the file exists and can be accessed
 *  @ingroup MprPath
 */
extern int mprCopyPath(MprCtx ctx, cchar *from, cchar *to, int omode);

/**
 *  Return the current working directory
 *  @param ctx Any memory context allocated by the MPR.
 *  @return Returns an allocated string with the current working directory as an absolute path.
 */
extern char *mprGetCurrentPath(MprCtx ctx);

/**
 *  Delete a file.
 *  @description Delete a file or directory.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path String containing the path to delete. 
 *  @return Returns zero if successful otherwise a negative MPR error code is returned.
 *  @ingroup MprPath
 */
extern int mprDeletePath(MprCtx ctx, cchar *path);

/**
 *  Convert a path to an absolute path
 *  @description Get an absolute (canonical) equivalent representation of a path. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path to examine
 *  @returns An absolute path. Caller should free via #mprFree
 *  @ingroup MprPath
 */
extern char *mprGetAbsPath(MprCtx ctx, cchar *path);

/**
 *  Get a path formatted according to the native O/S conventions.
 *  @description Get an equivalent absolute path formatted using the directory separators native to the O/S platform.
 *  On Windows, it will use backward slashes ("\") as the directory separator and will contain a drive specification.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns An allocated string containing the new path. Caller must free using #mprFree.
 *  @ingroup MprPath
 */
extern char *mprGetNativePath(MprCtx ctx, cchar *path);

/**
 *  Get the base portion of a path
 *  @description Get the base portion of a path by stripping off all directory components
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns A path without any directory portion. The path is a reference into the original file string and 
 *      should not be freed. 
 *  @ingroup MprPath
 */
extern char *mprGetPathBase(MprCtx ctx, cchar *path);

/**
 *  Get the directory portion of a path
 *  @description Get the directory portion of a path by stripping off the base name.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns A new string containing the directory name. Caller must free.
 *  @ingroup MprPath
 */
extern char *mprGetPathDir(MprCtx ctx, cchar *path);

/**
 *  Create a directory list of files.
 *  @description Get the list of files in a directory and return a list.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param dir Directory to list.
 *  @param enumDirs Set to true to enumerate directory entries as well as regular paths. 
 *  @returns A list (MprList) of directory paths. Each path is a regular string owned by the list object.
 *      Use #mprFree to free the memory for the list and directory paths.
 *  @ingroup MprPath
 */
extern MprList *mprGetPathFiles(MprCtx ctx, cchar *dir, bool enumDirs);

/**
 *  Get the file extension portion of a path
 *  @description Get the file extension portion of a path. The file extension is the portion starting with the last "."
 *      in the path. It thus includes "." as the first charcter.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns A path extension. The extension is a reference into the original file string and should not be freed.
 *  @ingroup MprPath
 */
extern cchar *mprGetPathExtension(MprCtx ctx, cchar *path);

/**
    Get the target of a symbolic link.
    @description Return the path pointed to by a symbolic link. Not all platforms support symbolic links.
    @param ctx Any memory context allocated by the MPR.
    @param path Path name to examine
    @returns A path representing the target of the symbolic link. Caller must free.
    @ingroup MprPath
 */
extern char *mprGetPathLink(MprCtx ctx, cchar *path);

/**
 *  Return information about a file represented by a path.
 *  @description Returns file status information regarding the \a path.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path String containing the path to query.
 *  @param info Pointer to a pre-allocated MprPath structure.
 *  @return Returns zero if successful, otherwise a negative MPR error code is returned.
 *  @ingroup MprPath
 */
extern int mprGetPathInfo(MprCtx ctx, cchar *path, MprPath *info);

/**
 *  Get the parent directory of a path
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns An allocated string containing the parent directory. Caller must free using #mprFree.
 *  @ingroup MprPath
 */
extern char *mprGetPathParent(MprCtx ctx, cchar *path);

/**
 *  Get the path directory separator.
 *  Return the directory separator characters used to separate directories on a given file system. Typically "/" or "\"
 *      The first entry is the default separator.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Use this path to specify either the root of the file system or a file on the file system.
 *  @returns The string of path separators. The first entry is the default separator.
 *  @ingroup MprPath
 */
extern cchar *mprGetPathSeparators(MprCtx ctx, cchar *path);

/**
 *  Get the file newline character string for a given path.
 *  Return the character string used to delimit new lines in text files.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Use this path to specify either the root of the file system or a file on the file system.
 *  @returns A string used to delimit new lines. This is typically "\n" or "\r\n"
 *  @ingroup MprPath
 */
extern cchar *mprGetPathNewline(MprCtx ctx, cchar *path);

/**
 *  Get a portable path 
 *  @description Get an equivalent absolute path that is somewhat portable. 
 *      This means it will use forward slashes ("/") as the directory separator.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns An allocated string containing the new path. Caller must free using #mprFree.
 *  @ingroup MprPath
 */
extern char *mprGetPortablePath(MprCtx ctx, cchar *path);

/**
 *  Get a relative path
 *  @description Get an equivalent path that is relative to the application's current working directory.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns An allocated string containing the relative directory. Caller must free using #mprFree.
 *  @ingroup MprPath
 */
extern char *mprGetRelPath(MprCtx ctx, cchar *path);

/**
 *  Make a temporary file.
 *  @description Thread-safe way to make a unique temporary file. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param tmpDir Base directory in which the temp file will be allocated.
 *  @return An allocated string containing the path of the temp file.
 *  @ingroup MprPath
 */
extern char *mprGetTempPath(MprCtx ctx, cchar *tmpDir);

/*
 *  Flags for mprGetTransformedPath
 */
#define MPR_PATH_ABS            0x1     /* Normalize to an absolute path */
#define MPR_PATH_REL            0x2     /* Normalize to an relative path */
#define MPR_PATH_CYGWIN         0x4     /* Normalize to a cygwin path */
#define MPR_PATH_NATIVE_SEP     0x8     /* Use native path separators */

/**
 *  Transform a path
 *  @description A path is transformed by cleaning and then transforming according to the flags.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path First path to compare
 *  @param flags Flags to modify the path representation.
 *  @returns A newly allocated, clean path. Caller should free via #mprFree
 *  @ingroup MprPath
 */
extern char *mprGetTransformedPath(MprCtx ctx, cchar *path, int flags);

/**
 *  Determine if a path is absolute
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns True if the path is absolue
 *  @ingroup MprPath
 */ 
extern bool mprIsAbsPath(MprCtx ctx, cchar *path);

/**
 *  Determine if a path is relative
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @returns True if the path is relative
 *  @ingroup MprPath
 */ 
extern bool mprIsRelPath(MprCtx ctx, cchar *path);

/**
 *  Join paths
 *  @description Resolve one path relative to another
 *  @param ctx Any memory context allocated by the MPR.
 *  @param dir Directory path name to test use as the base/dir.
 *  @param other Other path name to resolve against path.
 *  @returns Allocated string containing the resolved path.
 *  @ingroup MprPath
 */
extern char *mprJoinPath(MprCtx ctx, cchar *dir, cchar *other);

/**
 *  Join an extension to a path
 *  @description Add an extension to a path if it does not already have one.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param dir Directory path name to test use as the base/dir.
 *  @param ext Extension to add. Must have period prefix.
 *  @returns Allocated string containing the resolved path.
 *  @ingroup MprPath
 */
extern char *mprJoinPathExt(MprCtx ctx, cchar *dir, cchar *ext);

/**
 *  Make a directory
 *  @description Make a directory using the supplied path. Intermediate directories are created as required.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path String containing the directory pathname to create.
 *  @param makeMissing If true make all required intervening directory segments.
 *  @param perms Posix style file permissions mask.
 *  @return Returns zero if successful, otherwise a negative MPR error code is returned.
 *  @ingroup MprPath
 */
extern int mprMakeDir(MprCtx ctx, cchar *path, int perms, bool makeMissing);

/**
    Make a link
    @description Make a link to the specified target path. This will make symbolic or hard links depending on the value
        of the hard parameter
    @param ctx Any memory context allocated by the MPR.
    @param path String containing the directory pathname to create.
    @param target String containing the target file or directory to link to.
    @param hard If true, make a hard link, otherwise make a soft link.
    @return Returns zero if successful, otherwise a negative MPR error code is returned.
    @ingroup MprPath
 */
extern int mprMakeLink(MprCtx ctx, cchar *path, cchar *target, bool hard);

/**
 *  Normalize a path
 *  @description A path is normalized by redundant segments such as "./" and "../dir" and duplicate 
 *      path separators. Path separators are mapped. Paths are not converted to absolute paths.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path First path to compare
 *  @returns A newly allocated, clean path. Caller should free via #mprFree
 *  @ingroup MprPath
 */
extern char *mprGetNormalizedPath(MprCtx ctx, cchar *path);

/**
 *  Map the separators in a path.
 *  @description Map the directory separators in a path to the specified separators. This is useful to change from
 *      backward to forward slashes when dealing with Windows paths.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to examine
 *  @param separator Separator character to use.
 *  @returns An allocated string containing the parent directory. Caller must free using #mprFree.
 *  @ingroup MprPath
 */
extern void mprMapSeparators(MprCtx ctx, char *path, int separator);

extern bool mprIsPathSeparator(MprCtx ctx, cchar *path, cchar c);
extern int mprGetPathSeparator(MprCtx ctx, cchar *path);
extern cchar *mprGetFirstPathSeparator(MprCtx ctx, cchar *path);
extern cchar *mprGetLastPathSeparator(MprCtx ctx, cchar *path);

/**
 *  Determine if a file exists for a path name and can be accessed
 *  @description Test if a file can be accessed for a given mode
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to test
 *  @param omode Posix style file open mode mask. See #mprOpen for the various modes.
 *  @returns True if the file exists and can be accessed
 *  @ingroup MprPath
 */
extern bool mprPathExists(MprCtx ctx, cchar *path, int omode);

extern char *mprResolvePath(MprCtx ctx, cchar *path, cchar *other);

/**
 *  Compare two paths if they are the same
 *  @description Compare two paths to see if they are equal. This normalizes the paths to absolute paths first before
        comparing. It does handle case sensitivity appropriately.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path1 First path to compare
 *  @param path2 Second path to compare
 *  @returns True if the file exists and can be accessed
 *  @ingroup MprPath
 */
extern int mprSamePath(MprCtx ctx, cchar *path1, cchar *path2);

/**
 *  Compare two paths if they are the same for a given length.
 *  @description Compare two paths to see if they are equal. This normalizes the paths to absolute paths first before
        comparing. It does handle case sensitivity appropriately. The len parameter 
 *      if non-zero, specifies how many characters of the paths to compare.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path1 First path to compare
 *  @param path2 Second path to compare
 *  @param len How many characters to compare.
 *  @returns True if the file exists and can be accessed
 *  @ingroup MprPath
 */
extern int mprSamePathCount(MprCtx ctx, cchar *path1, cchar *path2, int len);

/*
 *  Flags for mprSearchForFile
 */
#define MPR_SEARCH_EXE      0x1         /* Search for an executable */

/*
 *  Search path separator
 */
#if BLD_WIN_LIKE
#define MPR_SEARCH_SEP      ";"
#define MPR_SEARCH_SEP_CHAR ';'
#else
#define MPR_SEARCH_SEP      ":"
#define MPR_SEARCH_SEP_CHAR ':'
#endif

/**
 *  Search for a path
 *  @description Search for a file using a given set of search directories
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Path name to locate. Must be an existing file or directory.
 *  @param flags Flags.
 *  @param search Variable number of directories to search.
 *  @returns Allocated string containing the full path name of the located file.
 *  @ingroup MprPath
 */
extern char *mprSearchPath(MprCtx ctx, cchar *path, int flags, cchar *search, ...);

/**
 *  Truncate a path
 *  @description Truncate a file to a given size
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path First path to compare
 *  @param size New maximum size for the file.
 *  @returns Zero if successful.
 *  @ingroup MprPath
 */
extern int mprTruncatePath(MprCtx ctx, cchar *path, int size);

/**
 *  Trim an extension from a path
 *  @description Trim a file extension (".ext") from a path name.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path First path to compare
 *  @returns An allocated string with the trimmed path.
 *  @ingroup MprPath
 */
extern char *mprTrimPathExtension(MprCtx ctx, cchar *path);


typedef struct MprOsService {
    int             dummy;
} MprOsService;

extern MprOsService *mprCreateOsService(MprCtx ctx);
extern int          mprStartOsService(MprOsService *os);
extern void         mprStopOsService(MprOsService *os);

typedef struct MprModuleService {
    MprList         *modules;
    char            *searchPath;
#if BLD_FEATURE_MULTITHREAD
    struct MprMutex *mutex;
#endif
} MprModuleService;


extern MprModuleService *mprCreateModuleService(MprCtx ctx);
extern int              mprStartModuleService(MprModuleService *os);
extern void             mprStopModuleService(MprModuleService *os);

/**
 *  Module start/stop point function signature
 *  @param mp Module object reference returned from #mprCreateModule
 *  @returns zero if successful, otherwise return a negative MPR error code.
 */ 
typedef int (*MprModuleProc)(struct MprModule *mp);

/**
 *  Loadable Module Service
 *  @description The MPR provides services to load and unload shared libraries.
 *  @see MprModule, mprGetModuleSearchPath, mprSetModuleSearchPath, mprLoadModule, mprUnloadModule, 
 *      mprCreateModule, mprLookupModule, MprModuleProc
 *  @stability Evolving.
 *  @defgroup MprModule MprModule
 */
typedef struct MprModule {
    char            *name;              /**< Unique module name */
    char            *version;           /**< Module version */
    void            *moduleData;        /**< Module specific data */
    void            *handle;            /**< O/S shared library load handle */
    MprModuleProc   start;              /**< Start the module service */
    MprModuleProc   stop;               /**< Stop the module service */
} MprModule;


/**
 *  Loadable module entry point signature. 
 *  @description Loadable modules can have an entry point that is invoked automatically when a module is loaded. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param path Actual path to the module
 *  @return a new MprModule structure for the module. Return NULL if the module can't be initialized.
 *  @ingroup MprModule
 */
typedef MprModule *(*MprModuleEntry)(MprCtx ctx, cchar *path);

/**
 *  Get the module search path
 *  @description Get the directory search path used by the MPR when loading dynamic modules. This is a colon separated (or 
 *      semicolon on Windows) set of directories.
 *  @param ctx Any memory context allocated by the MPR.
 *  @returns The module search path. Caller must not free.
 *  @ingroup MprModule
 */
extern cchar *mprGetModuleSearchPath(MprCtx ctx);

/**
 *  Set the module search path
 *  @description Set the directory search path used by the MPR when loading dynamic modules. This path string must
 *      should be a colon separated (or semicolon on Windows) set of directories. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param searchPath Colon separated set of directories
 *  @returns The module search path. Caller must not free.
 *  @ingroup MprModule
 */
extern void mprSetModuleSearchPath(MprCtx ctx, char *searchPath);

/**
 *  Create a module
 *  @description This call will create a module object for a loadable module. This should be invoked by the 
 *      module itself in its module entry point to register itself with the MPR.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param name Name of the module
 *  @param version Version string of the form: Major.Minor.patch
 *  @param moduleData to associate with this module
 *  @param start Start function to invoke to start module services
 *  @param stop Stop function to invoke to stop module services
 *  @returns A module object for this module
 *  @ingroup MprModule
 */
extern MprModule *mprCreateModule(MprCtx ctx, cchar *name, cchar *version, void *moduleData, MprModuleProc start,
        MprModuleProc stop);

/**
 *  Load a module
 *  @description Load a module into the MPR. This will load a dynamic shared object (shared library) and call the
 *      modules entry point. If the module has already been loaded, it this call will do nothing and will just
 *      return the already defined module object. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param filename Name of the module to load. The module will be searched using the defined module search path 
 *      (see #mprSetModuleSearchPath). The filename may or may not include a platform specific shared library extension such
 *      as .dll, .so or .dylib. By omitting the library extension, code can portably load shared libraries.
 *  @param entryPoint Name of function to invoke after loading the module.
 *  @returns A module object for this module created in the module entry point by calling #mprCreateModule
 *  @ingroup MprModule
 */
extern MprModule *mprLoadModule(MprCtx ctx, cchar *filename, cchar *entryPoint);

/**
 *  Search for a module on the current module path
 *  @param ctx Any memory context allocated by the MPR.
 *  @param module Name of the module to locate.
 *  @param path Pointer to a string that will receive the module path. Caller should free.
 *  @returns 0 if the module was found and path set to the location of the module.
 */
extern int mprSearchForModule(MprCtx ctx, cchar *module, char **path);

/**
 *  Lookup a module
 *  @description Lookup a module by name and return the module object.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param name Name of the module specified to #mprCreateModule.
 *  @returns A module object for this module created in the module entry point by calling #mprCreateModule
 *  @ingroup MprModule
 */
extern MprModule *mprLookupModule(MprCtx ctx, cchar *name);

/**
 *  Unload a module
 *  @description Unload a module from the MPR. This will unload a dynamic shared object (shared library). This routine
 *      is not fully supported by the MPR and is often fraught with issues. A module must usually be completely inactive 
 *      with no allocated memory when it is unloaded.
 *  @param mp Module object returned via #mprLookupModule
 *  @ingroup MprModule
 */
extern void mprUnloadModule(MprModule *mp);

/*
 *  Event flags
 */
#define MPR_EVENT_CONTINUOUS    0x1     /**< Auto reschedule the event */
#define MPR_EVENT_THREAD        0x2     /**< Run proc using worker thread */
#define MPR_EVENT_RUNNING       0x4     /**< Event currently executing */

/**
 *  Event callback function
 *  @ingroup MprEvent
 */
typedef void (*MprEventProc)(void *data, struct MprEvent *event);

/**
 *  Event object
 *  @description The MPR provides a powerful priority based eventing mechanism. Events are described by MprEvent objects
 *      which are created and queued via #mprCreateEvent. Each event may have a priority and may be one-shot or 
 *      be continuously rescheduled according to a specified period. The event subsystem provides the basis for 
 *      callback timers. 
 *  @see MprEvent, mprCreateEvent, mprCreateTimerEvent, mprRescheduleEvent, mprStopContinuousEvent, 
 *      mprRestartContinuousEvent, MprEventProc
 *  @defgroup MprEvent MprEvent
 */
typedef struct MprEvent {
    MprEventProc        proc;           /**< Callback procedure */
    MprTime             timestamp;      /**< When was the event created */
    int                 priority;       /**< Priority 0-99. 99 is highest */
    int                 period;         /**< Reschedule period */
    int                 flags;          /**< Event flags */
    MprTime             due;            /**< When is the event due */
    void                *data;          /**< Event private data */
    struct MprEvent     *next;          /**< Next event linkage */
    struct MprEvent     *prev;          /**< Previous event linkage */
    struct MprDispatcher *dispatcher;   /**< Event dispatcher service */
} MprEvent;

#define MPR_DISPATCHER_WAIT_EVENTS      0x1
#define MPR_DISPATCHER_WAIT_IO          0x2

/*
 *  Event Dispatcher
 */
typedef struct MprDispatcher {
    MprEvent        eventQ;             /* Event queue */
    MprEvent        timerQ;             /* Queue of future events */
    MprEvent        taskQ;              /* Task queue */
    MprTime         lastEventDue;       /* When the last event is due */
    MprTime         lastRan;            /* When last checked queues */
    MprTime         now;                /* Current notion of time */
    int             eventCounter;       /* Incremented for each event (wraps) */
    int             flags;              /* State flags */
#if BLD_FEATURE_MULTITHREAD
    struct MprMutex *mutex;             /* Multi-thread sync */
    struct MprCond  *cond;              /* Wakeup dispatcher */
    struct MprSpin  *spin;              /* Multi-thread sync */
#endif
} MprDispatcher;

extern void mprWakeDispatcher(MprDispatcher *dispatcher);

/**
 *  Create a new event dispatcher
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @returns a Dispatcher object that can manage events and be used with mprCreateEvent
 */
extern MprDispatcher *mprCreateDispatcher(MprCtx ctx);
extern MprDispatcher *mprGetDispatcher(MprCtx ctx);
extern int mprGetEventCounter(MprDispatcher *dispatcher);
extern bool mprMustWakeDispatcher(MprCtx ctx);

/*
 *  ServiceEvents parameters
 */
#define MPR_SERVICE_EVENTS      0x1     /**< Service events */
#define MPR_SERVICE_IO          0x2     /**< Service I/O events */
#define MPR_SERVICE_ONE_THING   0x4     /**< Wait for one event or one I/O */

/**
 *  Service events
 *  @description Service the event queue. This call will block for the given delay until an event is ready to be
 *      serviced. Flags may modify the calls behavior.
 *  @param dispatcher An event dispatcher created via mprCreateDispatcher.
 *  @param delay Time in milliseconds to block until an event occurs.
 *  @param flags If set to MPR_SERVICE_ONE_THING, this call will service at most one event. Otherwise set to zero.
 *  @returns A count of the number of events serviced
 *  @ingroup MprEvent
 */
extern int mprServiceEvents(MprDispatcher *dispatcher, int delay, int flags);
extern int mprPumpEvents(MprDispatcher *dispatcher, int delay);

extern void     mprDoEvent(MprEvent *event, void *worker);
extern MprEvent *mprGetNextEvent(MprDispatcher *dispatcher);
extern int      mprGetIdleTime(MprDispatcher *dispatcher);

/**
 *  Create a new event
 *  @description Create and queue a new event for service
 *  @param dispatcher Dispatcher object created via mprCreateDispatcher
 *  @param proc Function to invoke when the event is run
 *  @param period Time in milliseconds used by continuous events between firing of the event.
 *  @param priority Priority to associate with the event. Priorities are integer values between 0 and 100 inclusive with
 *      50 being a normal priority. Useful constants are: 
 *      @li MPR_LOW_PRIORITY
 *      @li MPR_NORMAL_PRIORITY
 *      @li MPR_HIGH_PRIORITY
 *  @param data Data to associate with the event and stored in event->data.
 *  @param flags Flags to modify the behavior of the event. Valid values are: MPR_EVENT_CONTINUOUS to create an 
 *      event which will be automatically rescheduled accoring to the specified period.
 *  @ingroup MprEvent
 */
extern MprEvent *mprCreateEvent(MprDispatcher *dispatcher, MprEventProc proc, int period, int priority, 
        void *data, int flags);

/**
 *  Remove an event
 *  @description Remove a queued event. This is useful to remove continuous events from the event queue.
 *  @param event Event object returned from #mprCreateEvent
 *  @ingroup MprEvent
 */
extern void mprRemoveEvent(MprEvent *event);

/**
 *  Stop an event
 *  @description Stop a continuous event and remove from the queue. The event object is not freed, but simply removed
 *      from the event queue.
 *  @param event Event object returned from #mprCreateEvent
 *  @ingroup MprEvent
 */
extern void mprStopContinuousEvent(MprEvent *event);

/**
 *  Restart an event
 *  @description Restart a continuous event after it has been stopped via #mprStopContinuousEvent. This call will 
 *      add the event to the event queue and it will run after the configured event period has expired.
 *  @param event Event object returned from #mprCreateEvent
 *  @ingroup MprEvent
 */
extern void mprRestartContinuousEvent(MprEvent *event);

/**
 *  Create a timer event
 *  @description Create and queue a timer event for service. This is a convenience wrapper to create continuous
 *      events over the #mprCreateEvent call.
 *  @param dispatcher Dispatcher object created via mprCreateDispatcher
 *  @param proc Function to invoke when the event is run
 *  @param period Time in milliseconds used by continuous events between firing of the event.
 *  @param priority Priority to associate with the event. Priorities are integer values between 0 and 100 inclusive with
 *      50 being a normal priority. Useful constants are: 
 *      @li MPR_LOW_PRIORITY
 *      @li MPR_NORMAL_PRIORITY
 *      @li MPR_HIGH_PRIORITY
 *  @param data Data to associate with the event and stored in event->data.
 *  @param flags Not used.
 *  @ingroup MprEvent
 */
extern MprEvent *mprCreateTimerEvent(MprDispatcher *dispatcher, MprEventProc proc, int period, int priority, 
        void *data, int flags);

/**
 *  Reschedule an event
 *  @description Reschedule a continuous event by modifying its period.
 *  @param event Event object returned from #mprCreateEvent
 *  @param period Time in milliseconds used by continuous events between firing of the event.
 *  @ingroup MprEvent
 */
extern void mprRescheduleEvent(MprEvent *event, int period);

#if BLD_FEATURE_XML
/*
 *  XML parser states. The states that are passed to the user handler have "U" appended to the comment.
 *  The error states (ERR and EOF) must be negative.
 */
#define MPR_XML_ERR                 -1      /* Error */
#define MPR_XML_EOF                 -2      /* End of input */
#define MPR_XML_BEGIN               1       /* Before next tag               */
#define MPR_XML_AFTER_LS            2       /* Seen "<"                      */
#define MPR_XML_COMMENT             3       /* Seen "<!--" (usr)        U    */
#define MPR_XML_NEW_ELT             4       /* Seen "<tag" (usr)        U    */
#define MPR_XML_ATT_NAME            5       /* Seen "<tag att"               */
#define MPR_XML_ATT_EQ              6       /* Seen "<tag att" =             */
#define MPR_XML_NEW_ATT             7       /* Seen "<tag att = "val"   U    */
#define MPR_XML_SOLO_ELT_DEFINED    8       /* Seen "<tag../>"          U    */
#define MPR_XML_ELT_DEFINED         9       /* Seen "<tag...>"          U    */
#define MPR_XML_ELT_DATA            10      /* Seen "<tag>....<"        U    */
#define MPR_XML_END_ELT             11      /* Seen "<tag>....</tag>"   U    */
#define MPR_XML_PI                  12      /* Seen "<?processingInst"  U    */
#define MPR_XML_CDATA               13      /* Seen "<![CDATA["         U    */

/*
 *  Lex tokens
 */
typedef enum MprXmlToken {
    MPR_XMLTOK_ERR,
    MPR_XMLTOK_TOO_BIG,                     /* Token is too big */
    MPR_XMLTOK_CDATA,
    MPR_XMLTOK_COMMENT,
    MPR_XMLTOK_INSTRUCTIONS,
    MPR_XMLTOK_LS,                          /* "<" -- Opening a tag */
    MPR_XMLTOK_LS_SLASH,                    /* "</" -- Closing a tag */
    MPR_XMLTOK_GR,                          /* ">" -- End of an open tag */
    MPR_XMLTOK_SLASH_GR,                    /* "/>" -- End of a solo tag */
    MPR_XMLTOK_TEXT,
    MPR_XMLTOK_EQ,
    MPR_XMLTOK_EOF,
    MPR_XMLTOK_SPACE,
} MprXmlToken;

typedef int (*MprXmlHandler)(struct MprXml *xp, int state, cchar *tagName, cchar* attName, cchar* value);
typedef int (*MprXmlInputStream)(struct MprXml *xp, void *arg, char *buf, int size);

/*
 *  Per XML session structure
 */
typedef struct MprXml {
    MprXmlHandler       handler;            /* Callback function */
    MprXmlInputStream   readFn;             /* Read data function */
    MprBuf              *inBuf;             /* Input data queue */
    MprBuf              *tokBuf;            /* Parsed token buffer */
    int                 quoteChar;          /* XdbAtt quote char */
    int                 lineNumber;         /* Current line no for debug */
    void                *parseArg;          /* Arg passed to mprXmlParse() */
    void                *inputArg;          /* Arg for mprXmlSetInputStream() */
    char                *errMsg;            /* Error message text */
} MprXml;

extern MprXml   *mprXmlOpen(MprCtx ctx, int initialSize, int maxSize);
extern void     mprXmlSetParserHandler(MprXml *xp, MprXmlHandler h);
extern void     mprXmlSetInputStream(MprXml *xp, MprXmlInputStream s, void *arg);
extern int      mprXmlParse(MprXml *xp);
extern void     mprXmlSetParseArg(MprXml *xp, void *parseArg);
extern void     *mprXmlGetParseArg(MprXml *xp);
extern cchar    *mprXmlGetErrorMsg(MprXml *xp);
extern int      mprXmlGetLineNumber(MprXml *xp);

#endif /* BLD_FEATURE_XML */


/**
 *  Condition variable for single and multi-thread synchronization. Condition variables can be used to coordinate 
 *  activities. These variables are level triggered in that a condition can be signalled prior to another thread 
 *  waiting. Condition variables can be used when single threaded but and will call mprServiceEvents to pump events
 *  until another callback invokes mprWaitForCond
 *  @ingroup MprSynch
 */
typedef struct MprCond {
    #if BLD_FEATURE_MULTITHREAD
        #if BLD_UNIX_LIKE
            pthread_cond_t cv;      /**< Unix pthreads condition variable */
        #elif BLD_WIN_LIKE
            HANDLE cv;              /* Windows event handle */
        #elif VXWORKS
            SEM_ID cv;              /* Condition variable */
        #else
            error("Unsupported OS");
        #endif
            struct MprMutex *mutex; /**< Thread synchronization mutex */
    #endif
    volatile int triggered;         /**< Value of the condition */
} MprCond;


/**
 *  Create a condition lock variable.
 *  @description This call creates a condition variable object that can be used in #mprWaitForCond and #mprSignalCond calls. 
 *      Use #mprFree to destroy the condition variable.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @ingroup MprSynch
 */
extern MprCond *mprCreateCond(MprCtx ctx);

/**
 *  Reset a condition variable. This sets the condition variable to the unsignalled condition.
 *  @param cond Condition variable object created via #mprCreateCond
 */
extern void mprResetCond(MprCond *cond);

/**
 *  Wait for a condition lock variable.
 *  @description Wait for a condition lock variable to be signaled. If the condition is signaled before the timeout
 *      expires this call will reset the condition variable and return. This way, it automatically resets the variable
 *      for future waiters.
 *  @param cond Condition variable object created via #mprCreateCond
 *  @param timeout Time in milliseconds to wait for the condition variable to be signaled.
 *  @return Zero if the event was signalled. Returns < 0 for a timeout.
 *  @ingroup MprSynch
 */
extern int mprWaitForCond(MprCond *cond, int timeout);

/**
 *  Wait for a condition lock variable and pump events while waiting.
 *  @description Wait for a condition lock variable to be signaled. If the condition is signaled before the timeout
 *      expires this call will reset the condition variable and return. This way, it automatically resets the variable
 *      for future waiters.
 *  @param cond Condition variable object created via #mprCreateCond
 *  @param timeout Time in milliseconds to wait for the condition variable to be signaled.
 *  @return Zero if the event was signalled. Returns < 0 for a timeout.
 *  @ingroup MprSynch
 */
extern int mprWaitForCondWithService(MprCond *cond, int timeout);

/**
 *  Signal a condition lock variable.
*   @description Signal a condition variable and set it to the \a triggered status. Existing or future callers of
*       #mprWaitForCond will be awakened.
 *  @param cond Condition variable object created via #mprCreateCond
 *  @ingroup MprSynch
 */
extern void mprSignalCond(MprCond *cond);

#if BLD_FEATURE_MULTITHREAD
/**
 *  Multithreaded Synchronization Services
 *  @see MprMutex, mprCreateStaticLock, mprFree, mprLock, mprTryLock, mprUnlock, mprGlobalLock, mprGlobalUnlock, 
 *      MprSpin, mprCreateSpinLock, MprCond, mprCreateCond, mprWaitForCond, mprSignalCond, mprFree
 *  @stability Evolving.
 *  @defgroup MprSynch MprSynch
 */
typedef struct MprSynch { int dummy; } MprSynch;

/**
 *  Multithreading lock control structure
 *  @description MprMutex is used for multithread locking in multithreaded applications.
 *  @ingroup MprSynch
 */
typedef struct MprMutex {
    #if BLD_WIN_LIKE
        CRITICAL_SECTION cs;            /**< Internal mutex critical section */
    #elif VXWORKS
        SEM_ID      cs;
    #elif BLD_UNIX_LIKE
        pthread_mutex_t  cs;
    #else
        error("Unsupported OS");
    #endif
} MprMutex;


/**
 *  Multithreading spin lock control structure
 *  @description    MprSpin is used for multithread locking in multithreaded applications.
 *  @ingroup MprSynch
 */
typedef struct MprSpin {
    #if USE_MPR_LOCK
        MprMutex            cs;
    #elif BLD_WIN_LIKE
        CRITICAL_SECTION    cs;            /**< Internal mutex critical section */
    #elif VXWORKS
        SEM_ID              cs;
    #elif MACOSX
        OSSpinLock          cs;
    #elif BLD_UNIX_LIKE && BLD_HAS_SPINLOCK
        pthread_spinlock_t  cs;
    #elif BLD_UNIX_LIKE
        pthread_mutex_t     cs;
    #else
        error("Unsupported OS");
    #endif
    #if BLD_DEBUG
        MprOsThread         owner;
    #endif
} MprSpin;


#define lock(arg) mprLock(arg->mutex)
#define unlock(arg) mprUnlock(arg->mutex)

/**
 *  Create a Mutex lock object.
 *  @description This call creates a Mutex lock object that can be used in #mprLock, #mprTryLock and #mprUnlock calls. 
 *      Use #mprFree to destroy the lock.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @ingroup MprSynch
 */
extern MprMutex *mprCreateLock(MprCtx ctx);

/**
 *  Initialize a statically allocated Mutex lock object.
 *  @description This call initialized a Mutex lock object without allocation. The object can then be used used 
 *      in #mprLock, #mprTryLock and #mprUnlock calls.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param mutex Reference to an MprMutex structure to initialize
 *  @returns A reference to the supplied mutex. Returns null on errors.
 *  @ingroup MprSynch
 */
extern MprMutex *mprInitLock(MprCtx ctx, MprMutex *mutex);

/**
 *  Attempt to lock access.
 *  @description This call attempts to assert a lock on the given \a lock mutex so that other threads calling 
 *      mprLock or mprTryLock will block until the current thread calls mprUnlock.
 *  @returns Returns zero if the successful in locking the mutex. Returns a negative MPR error code if unsuccessful.
 *  @ingroup MprSynch
 */
extern bool mprTryLock(MprMutex *lock);

/**
 *  Create a spin lock lock object.
 *  @description This call creates a spinlock object that can be used in #mprSpinLock, and #mprSpinUnlock calls. Spin locks
 *      using MprSpin are much faster than MprMutex based locks on some systems.
 *      Use #mprFree to destroy the lock.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @ingroup MprSynch
 */
extern MprSpin *mprCreateSpinLock(MprCtx ctx);

/**
 *  Initialize a statically allocated spinlock object.
 *  @description This call initialized a spinlock lock object without allocation. The object can then be used used 
 *      in #mprSpinLock and #mprSpinUnlock calls.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param lock Reference to a static #MprSpin  object.
 *  @returns A reference to the MprSpin object. Returns null on errors.
 *  @ingroup MprSynch
 */
extern MprSpin *mprInitSpinLock(MprCtx ctx, MprSpin *lock);

/**
 *  Attempt to lock access on a spin lock
 *  @description This call attempts to assert a lock on the given \a spin lock so that other threads calling 
 *      mprSpinLock or mprTrySpinLock will block until the current thread calls mprSpinUnlock.
 *  @returns Returns zero if the successful in locking the spinlock. Returns a negative MPR error code if unsuccessful.
 *  @ingroup MprSynch
 */
extern bool mprTrySpinLock(MprSpin *lock);

/*
 *  For maximum performance, use the spin lock/unlock routines macros
 */
#if !BLD_DEBUG
#define BLD_USE_LOCK_MACROS 1
#endif
#if BLD_USE_LOCK_MACROS && !DOXYGEN
    /*
     *  Spin lock macros
     */
    #if MACOSX
        #define mprSpinLock(lock)   OSSpinLockLock(&((lock)->cs))
        #define mprSpinUnlock(lock) OSSpinLockUnlock(&((lock)->cs))
    #elif BLD_UNIX_LIKE && BLD_HAS_SPINLOCK
        #define mprSpinLock(lock)   pthread_spin_lock(&((lock)->cs))
        #define mprSpinUnlock(lock) pthread_spin_unlock(&((lock)->cs))
    #elif BLD_UNIX_LIKE
        #define mprSpinLock(lock)   pthread_mutex_lock(&((lock)->cs))
        #define mprSpinUnlock(lock) pthread_mutex_unlock(&((lock)->cs))
    #elif BLD_WIN_LIKE
        #define mprSpinLock(lock)   EnterCriticalSection(&((lock)->cs))
        #define mprSpinUnlock(lock) LeaveCriticalSection(&((lock)->cs))
    #elif VXWORKS
        #define mprSpinLock(lock)   semTake((lock)->cs, WAIT_FOREVER)
        #define mprSpinUnlock(lock) semGive((lock)->cs)
    #endif

    /*
     *  Lock macros
     */
    #if BLD_UNIX_LIKE
        #define mprLock(lock)       pthread_mutex_lock(&((lock)->cs))
        #define mprUnlock(lock)     pthread_mutex_unlock(&((lock)->cs))
    #elif BLD_WIN_LIKE
        #define mprUnlock(lock)     LeaveCriticalSection(&((lock)->cs))
        #define mprLock(lock)       EnterCriticalSection(&((lock)->cs))
    #elif VXWORKS
        #define mprUnlock(lock)     semGive((lock)->cs)
        #define mprLock(lock)       semTake((lock)->cs, WAIT_FOREVER)
    #endif
#else

    /**
     *  Lock access.
     *  @description This call asserts a lock on the given \a lock mutex so that other threads calling mprLock will 
     *      block until the current thread calls mprUnlock.
     *  @ingroup MprSynch
     */
    extern void mprLock(MprMutex *lock);

    /**
     *  Unlock a mutex.
     *  @description This call unlocks a mutex previously locked via mprLock or mprTryLock.
     *  @ingroup MprSynch
     */
    extern void mprUnlock(MprMutex *lock);

    /**
     *  Lock a spinlock.
     *  @description This call asserts a lock on the given \a spinlock so that other threads calling mprSpinLock will
     *      block until the curren thread calls mprSpinUnlock.
     *  @ingroup MprSynch
     */
    extern void mprSpinLock(MprSpin *lock);

    /**
     *  Unlock a spinlock.
     *  @description This call unlocks a spinlock previously locked via mprSpinLock or mprTrySpinLock.
     *  @ingroup MprSynch
     */
    extern void mprSpinUnlock(MprSpin *lock);
#endif

/**
 *  Globally lock the application.
 *  @description This call asserts the application global lock so that other threads calling mprGlobalLock will 
 *      block until the current thread calls mprGlobalUnlock.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @ingroup MprSynch
 */
extern void mprGlobalLock(MprCtx ctx);

/**
 *  Unlock the global mutex.
 *  @description This call unlocks the global mutex previously locked via mprGlobalLock.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @ingroup MprSynch
 */
extern void mprGlobalUnlock(MprCtx ctx);

/*
 *  Thread service
 */
typedef struct MprThreadService {
    MprList         *threads;           /* List of all threads */
    struct MprThread *mainThread;       /* Main application Mpr thread id */
    MprMutex        *mutex;             /* Multi-thread sync */
    int             stackSize;          /* Default thread stack size */
} MprThreadService;


typedef void (*MprThreadProc)(void *arg, struct MprThread *tp);

extern MprThreadService *mprCreateThreadService(struct Mpr *mpr);
extern bool mprStopThreadService(MprThreadService *ts, int timeout);

/**
 *  Thread Service. 
 *  @description The MPR provides a cross-platform thread abstraction above O/S native threads. It supports 
 *      arbitrary thread creation, thread priorities, thread management and thread local storage. By using these
 *      thread primitives with the locking and synchronization primitives offered by #MprMutex, #MprSpin and 
 *      #MprCond - you can create cross platform multi-threaded applications.
 *  @stability Evolving
 *  @see MprThread, mprCreateThread, mprStartThread, mprGetThreadName, mprGetThreadPriority, 
 *      mprSetThreadPriority, mprGetCurrentThread, mprGetCurrentOsThread, mprSetThreadPriority, 
 *      mprSetThreadData, mprGetThreadData, mprCreateThreadLocal
 *  @defgroup MprThread MprThread
 */
typedef struct MprThread {
    MprOsThread     osThread;           /**< O/S thread id */

#if BLD_WIN_LIKE
    handle          threadHandle;       /**< Threads OS handle for WIN */
#endif
    void            *data;              /**< Data argument */
    MprThreadProc   entry;              /**< Users thread entry point */
    char            *name;              /**< Name of thead for trace */
    MprMutex        *mutex;             /**< Multi-thread synchronization */
    ulong           pid;                /**< Owning process id */
    int             priority;           /**< Current priority */
    int             stackSize;          /**< Only VxWorks implements */
    int             isMain;             /**< Is the main thread */
} MprThread;


/**
 *  Thread local data storage
 */
typedef struct MprThreadLocal {
#if BLD_UNIX_LIKE
    pthread_key_t   key;                /**< Data key */
#elif BLD_WIN_LIKE
    DWORD           key;
#else
    int             dummy;              /**< Prevents asserts in memory allocation */
#endif
} MprThreadLocal;


/**
 *  Create a new thread
 *  @description MPR threads are usually real O/S threads and can be used with the various locking services (#MprMutex,
 *      #MprCond, #MprSpin) to enable scalable multithreaded applications.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param name Unique name to give the thread
 *  @param proc Entry point function for the thread. #mprStartThread will invoke this function to start the thread
 *  @param data Thread private data stored in MprThread.data
 *  @param priority Priority to associate with the thread. Mpr thread priorities are are integer values between 0 
 *      and 100 inclusive with 50 being a normal priority. The MPR maps these priorities in a linear fashion onto 
 *      native O/S priorites. Useful constants are: 
 *      @li MPR_LOW_PRIORITY
 *      @li MPR_NORMAL_PRIORITY
 *      @li MPR_HIGH_PRIORITY
 *  @param stackSize Stack size to use for the thread. On VM based systems, increasing this value, does not 
 *      necessarily incurr a real memory (working-set) increase. Set to zero for a default stack size.
 *  @returns A MprThread object
 *  @ingroup MprThread
 */
extern MprThread *mprCreateThread(MprCtx ctx, cchar *name, MprThreadProc proc, void *data, int priority, int stackSize);

/**
 *  Start a thread
 *  @description Start a thread previously created via #mprCreateThread. The thread will begin at the entry function 
 *      defined in #mprCreateThread.
 *  @param thread Thread object returned from #mprCreateThread
 *  @return Returns zero if successful, otherwise a negative MPR error code.
 *  @ingroup MprThread
 */
extern int mprStartThread(MprThread *thread);

/**
 *  Get the thread name.
 *  @description MPR threads are usually real O/S threads and can be used with the various locking services (#MprMutex,
 *      #MprCond, #MprSpin) to enable scalable multithreaded applications.
 *  @param thread Thread object returned from #mprCreateThread
 *  @return Returns a string name for the thread. Caller must not free.
 *  @ingroup MprThread
 */
extern cchar *mprGetThreadName(MprThread *thread);

/**
 *  Get the thread priroity
 *  @description Get the current priority for the specified thread.
 *  @param thread Thread object returned by #mprCreateThread
 *  @returns An integer MPR thread priority between 0 and 100 inclusive.
 *  @ingroup MprThread
 */
extern int mprGetThreadPriority(MprThread *thread);

/**
 *  Set the thread priroity
 *  @description Set the current priority for the specified thread.
 *  @param thread Thread object returned by #mprCreateThread
 *  @param priority Priority to associate with the thread. Mpr thread priorities are are integer values between 0 
 *      and 100 inclusive with 50 being a normal priority. The MPR maps these priorities in a linear fashion onto 
 *      native O/S priorites. Useful constants are: 
 *      @li MPR_LOW_PRIORITY
 *      @li MPR_NORMAL_PRIORITY
 *      @li MPR_HIGH_PRIORITY
 *  @ingroup MprThread
 */
extern void mprSetThreadPriority(MprThread *thread, int priority);

/**
 *  Get the currently executing thread.
 *  @description Get the thread object for the currently executing O/S thread.
 *  @param ctx Any memory context allocated by the MPR.
 *  @return Returns a thread object representing the current O/S thread.
 *  @ingroup MprThread
 */
extern MprThread *mprGetCurrentThread(MprCtx ctx);

/**
 *  Get the O/S thread
 *  @description Get the O/S thread ID for the currently executing thread.
 *  @return Returns a platform specific O/S thread ID. On Unix, this is a pthread reference. On other systems it is
 *      a thread integer value.
 *  @ingroup MprThread
 */
extern MprOsThread mprGetCurrentOsThread();

/**
 *  Set the thread priroity for the current thread.
 *  @param ctx Any memory context allocated by the MPR.
 *  @description Set the current priority for the specified thread.
 *  @param priority Priority to associate with the thread. Mpr thread priorities are are integer values between 0 
 *      and 100 inclusive with 50 being a normal priority. The MPR maps these priorities in a linear fashion onto 
 *      native O/S priorites. Useful constants are: 
 *      @li MPR_LOW_PRIORITY
 *      @li MPR_NORMAL_PRIORITY
 *      @li MPR_HIGH_PRIORITY
 *  @ingroup MprThread
 */
extern void mprSetCurrentThreadPriority(MprCtx ctx, int priority);

/*
 *  Somewhat internal APIs
 */
extern int mprMapMprPriorityToOs(int mprPriority);
extern int mprMapOsPriorityToMpr(int nativePriority);
extern void mprSetThreadStackSize(MprCtx ctx, int size);
extern int mprSetThreadData(MprThreadLocal *tls, void *value);
extern void *mprGetThreadData(MprThreadLocal *tls);
extern MprThreadLocal *mprCreateThreadLocal(MprCtx ctx);

#else /* !BLD_FEATURE_MULTITHREAD */

typedef struct MprThreadLocal {
    int             dummy;
} MprThreadLocal;
typedef void *MprThread;

#define mprInitThreads(ctx, mpr)
#define mprTermThreads(mpr)
#define mprCreateLock(ctx)
#define mprLock(lock)
#define mprTryLock(lock) ((void*) 1)
#define mprUnlock(lock)
#define mprCreateSpinLock(ctx)
#define mprSpinLock(lock)
#define mprTrySpinLock(lock)
#define mprSpinUnlock(lock)
#define mprGlobalLock(mpr)
#define mprGlobalUnlock(mpr)
#define mprSetThreadData(tls, value)
#define mprGetThreadData(tls) NULL
#define mprCreateThreadLocal(ejs) ((void*) 1)
#define lock(arg) 
#define unlock(arg) 
#endif /* BLD_FEATURE_MULTITHREAD */

extern cchar *mprGetCurrentThreadName(MprCtx ctx);

/*
 *  Magic number to identify blocks. Only used in debug mode.
 */
#define MPR_ALLOC_MAGIC     0xe814ecab

/**
 *  Memory allocation error callback. Notifiers are called if mprSetNotifier has been called on a context and a 
 *  memory allocation fails. All notifiers up the parent context chain are called in order.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param size Size of memory allocation request that failed
 *  @param total Total memory allocations so far
 *  @param granted Set to true if the request was actually granted, but the application is now exceeding its redline
 *      memory limit.
 *  @ingroup MprMem
 */
typedef void (*MprAllocNotifier)(MprCtx ctx, int64 size, int64 total, bool granted);

/**
 *  Mpr memory block destructors prototype
 *  @param ctx Any memory context allocated by the MPR.
 *  @return Return zero if the memory was actually freed. Return non-zero to prevent the memory being freed.
 *  @ingroup MprMem
 */
typedef int (*MprDestructor)(MprCtx ctx);

#if BLD_DEBUG
#define BLD_FEATURE_MEMORY_DEBUG    1       /* Enable memory debug assist. Fill blocks, verifies block integrity. */
#define BLD_FEATURE_MEMORY_STATS    1       /* Include memory stats routines */
#else
#define BLD_FEATURE_MEMORY_STATS    1
#endif

/*
 *  MprBlk flags
 */
#define MPR_ALLOC_HAS_DESTRUCTOR    0x1     /* Block has a destructor to be called when freed */
#define MPR_ALLOC_HAS_ERROR         0x2     /* Memory context has had allocation errors */
#define MPR_ALLOC_IS_HEAP           0x4     /* Block is a heap context */
#define MPR_ALLOC_FROM_MALLOC       0x8     /* Block allocated from a malloc heap */

#define MPR_ALLOC_BIGGEST           0x0FFFFFFF /* Largest block that can be allocated */

/*
 *  Align blocks on 8 byte boundaries.
 */
#define MPR_ALLOC_ALIGN(x)  (((x) + 7 ) & ~7)
#define MPR_PAGE_ALIGN(x, pagesize) (((x) + (pagesize) - 1) & ~(pagesize - 1))


/**
 *  Memory Allocation Block Header.
 *  @ingroup MprMem
 */
typedef struct MprBlk {
#if BLD_DEBUG
    char            *name;                  /* Debug Name */
    int             seqno;                  /* Unique block allocation number */
#endif
    struct MprBlk   *parent;                /* Parent block */
    struct MprBlk   *children;              /* First child block. Flags stored in low order bits. */
    struct MprBlk   *next;                  /* Next sibling */
    struct MprBlk   *prev;                  /* Prior sibling */

    uint            size: 28;               /* Size of the block (not counting header) */
    uint            flags: 4;               /* Flags */

#if BLD_FEATURE_MEMORY_DEBUG
    /*
     *  For debug, we don't worry about this bloating the MprBlk and messing up alignment.
     */
    uint            magic;                  /* Unique signature */
#endif
} MprBlk;

#define MPR_ALLOC_HDR_SIZE      (MPR_ALLOC_ALIGN(sizeof(struct MprBlk)))
#define MPR_GET_BLK(ptr)        ((MprBlk*) (((char*) (ptr)) - MPR_ALLOC_HDR_SIZE))
#define MPR_GET_PTR(bp)         ((void*) (((char*) (bp)) + MPR_ALLOC_HDR_SIZE))
#define MPR_GET_BLK_SIZE(bp)    ((bp)->size)
#define MPR_SET_SIZE(bp, len)   ((bp)->size = (len))
#define mprGetBlockSize(ptr)    ((ptr) ? (MPR_GET_BLK_SIZE(MPR_GET_BLK(ptr)) - MPR_ALLOC_HDR_SIZE): 0)
#define MPR_HEAP_OVERHEAD       (MPR_ALLOC_HDR_SIZE + MPR_ALLOC_ALIGN(sizeof(MprRegion) + sizeof(MprHeap) + sizeof(MprDestructor)))
#define mprGetFirstChild(ctx)   (void*) (MPR_GET_BLK(ctx)->children)

/*
 *  Region of memory. Regions are used to describe chunks of memory used by Heaps.
 */
typedef struct MprRegion {
    struct MprRegion *next;                 /* Next region in chain */
    char            *memory;                /* Region memory data */
    char            *nextMem;               /* Pointer to next free byte in memory */
    int             vmSize;                 /* Size of virtual memory containing the region struct plus region memory */
    int             size;                   /* Original size of region */
} MprRegion;

/*
 *  Heap flags
 */
#define MPR_ALLOC_PAGE_HEAP     0x1         /* Page based heap. Used for allocating arenas and slabs */
#define MPR_ALLOC_ARENA_HEAP    0x2         /* Heap is an arena. All allocations are done from one or more regions */
#define MPR_ALLOC_SLAB_HEAP     0x4         /* Heap is a slab. Constant sized objects use slab heaps */
#define MPR_ALLOC_MALLOC_HEAP   0x8         /* Heap is a standard malloc heap */
#define MPR_ALLOC_FREE_CHILDREN 0x10        /* Heap must be accessed in a thread safe fashion */
#define MPR_ALLOC_THREAD_SAFE   0x20        /* Heap must be accessed in a thread safe fashion */

/*
 *  The heap context supports arena and slab based allocations. Layout of allocated heap blocks:
 *      HDR
 *      MprHeap
 *      MprRegion
 *      Heap Data
 *      Destructor
 */
typedef struct MprHeap {
    cchar           *name;                  /* Debugging name of the heap */
    MprDestructor   destructor;             /* Heap destructor routine */
    MprRegion       *region;                /* Current region of memory for allocation */
    MprRegion       *depleted;              /* Depleted regions. All useful memory has been allocated */
    int             flags;                  /* Heap flags */
    /*
     *  Slab allocation object information and free list
     */
    int             objSize;                /* Size of each heap object */
    MprBlk          *freeList;              /* Linked list of free objects */
    /*
     *  Stats
     */
    int            allocBytes;              /* Number of bytes allocated for this heap */
    int            peakAllocBytes;          /* Peak allocated (max allocBytes) */
    int            allocBlocks;             /* Number of alloced blocks for this heap */
    int            peakAllocBlocks;         /* Peak allocated blocks */
    int            totalAllocCalls;         /* Total count of allocation calls */
    int            freeListCount;           /* Count of objects on freeList */
    int            peakFreeListCount;       /* Peak count of blocks on the free list */
    int            reuseCount;              /* Count of allocations from the freelist */
    int            reservedBytes;           /* Virtual allocations for page heaps */

    MprAllocNotifier notifier;              /* Memory allocation failure callback */
    MprCtx         notifierCtx;             /* Memory block context for the notifier */

#if BLD_FEATURE_MULTITHREAD
    MprSpin         spin;
#endif
} MprHeap;


/*
 *  Memory allocation control
 */
typedef struct MprAlloc {
    MprHeap         pageHeap;               /* Page based heap for Arena allocations */
    int             inAllocException;       /* Recursive protect */
    uint            pageSize;               /* System page size */
    uint            errors;                 /* Allocation errors */
    uint            numCpu;                 /* Number of CPUs */
    int64           bytesAllocated;         /* Bytes currently allocated */
    int64           peakAllocated;          /* Peak bytes allocated */
    int64           peakStack;              /* Peak stack usage */
    int64           redLine;                /* Warn if allocation exceeds this level */
    int64           maxMemory;              /* Max memory to allocate */
    int64           rss;                    /* OS calculated resident stack size in bytes */
    int64           ram;                    /* System RAM size in bytes */
    int64           user;                   /* System user RAM size in bytes (excludes kernel) */
    void            *stackStart;            /* Start of app stack */
} MprAlloc;


#if BLD_WIN_LIKE || VXWORKS
#define MPR_MAP_READ        0x1
#define MPR_MAP_WRITE       0x2
#define MPR_MAP_EXECUTE     0x4
#else
#define MPR_MAP_READ        PROT_READ
#define MPR_MAP_WRITE       PROT_WRITE
#define MPR_MAP_EXECUTE     PROT_EXEC
#endif

extern struct Mpr *mprCreateAllocService(MprAllocNotifier cback, MprDestructor destructor);
extern MprHeap  *mprAllocArena(MprCtx ctx, cchar *name, uint arenaSize, bool threadSafe, MprDestructor destructor);
extern MprHeap  *mprAllocHeap(MprCtx ctx, cchar *name, uint arenaSize, bool threadSafe, MprDestructor destructor);
extern MprHeap  *mprAllocSlab(MprCtx ctx, cchar *name, uint objSize, uint count, bool threadSafe, MprDestructor destructor);
extern void     mprSetAllocNotifier(MprCtx ctx, MprAllocNotifier cback);
extern void     mprInitBlock(MprCtx ctx, void *ptr, uint size);

#if DOXYGEN
typedef void *Type;

/**
 *  Allocate a block of memory
 *  @description Allocates a block of memory using the supplied memory context \a ctx as the parent. #mprAlloc 
 *      manages a tree structure of memory blocks. Freeing a block via mprFree will release the allocated block
 *      and all child blocks.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param size Size of the memory block to allocate.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @ingroup MprMem
 */
extern void *mprAlloc(MprCtx ctx, uint size);

/**
 *  Allocate an object block of memory
 *  @description Allocates a block of memory using the supplied memory context \a ctx as the parent. #mprAllocWithDestructor
 *      associates a destructor function with an object. This function will be invoked when the object is freed. 
 *      Freeing a block will first call the destructor and if that returns zero, mprFree will release the allocated 
 *      block and all child blocks.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param size Size of the memory block to allocate.
 *  @param destructor Destructor function to invoke when the allocation is freed via #mprFree.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @stability Prototype. This function names are highly likely to be refactored.
 *  @ingroup MprMem
 */
extern void *mprAllocWithDestructor(MprCtx ctx, uint size, MprDestructor destructor);

/**
 *  Allocate an object block of memory and zero it.
 *  @description Allocates a zeroed block of memory using the supplied memory context \a ctx as the parent. 
        #mprAllocWithDestructor associates a destructor function with an object. This function will be invoked when the 
        object is freed. Freeing a block will first call the destructor and if that returns zero, mprFree will release the 
        allocated block and all child blocks.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param size Size of the memory block to allocate.
 *  @param destructor Destructor function to invoke when the allocation is freed via #mprFree.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @stability Prototype. This function names are highly likely to be refactored.
 *  @ingroup MprMem
 */
extern void *mprAllocWithDestructorZeroed(MprCtx ctx, uint size, MprDestructor destructor);

/**
 *  Allocate a zeroed block of memory
 *  @description Allocates a zeroed block of memory using the supplied memory context \a ctx as the parent. #mprAlloc 
 *      manages a tree structure of memory blocks. Freeing a block via mprFree will release the allocated block
 *      and all child blocks.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param size Size of the memory block to allocate.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @ingroup MprMem
 */
extern void *mprAllocZeroed(MprCtx ctx, uint size);

/**
 *  Reallocate a block
 *  @description Reallocates a block increasing its size. If the specified size is less than the current block size,
 *      the call will ignore the request and simply return the existing block.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param ptr Memory to reallocate. If NULL, call malloc.
 *  @param size New size of the required memory block.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to realloc and mprRealloc.
 *  @ingroup MprMem
 */
extern void *mprRealloc(MprCtx ctx, void *ptr, uint size);

/**
 *  Allocate an object of a given type
 *  @description Allocates a block of memory large enough to hold an instance of the specified type. This uses the 
 *      supplied memory context \a ctx as the parent. This is implemented as a macro
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param type Type of the object to allocate
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @stability Prototype. This function names are highly likely to be refactored.
 *  @ingroup MprMem
 */
extern void *mprAllocObj(MprCtx ctx, Type type) { return 0; }

/**
 *  Allocate a zeroed object of a given type
 *  @description Allocates a zeroed block of memory large enough to hold an instance of the specified type. This uses the 
 *      supplied memory context \a ctx as the parent. This is implemented as a macro
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param type Type of the object to allocate
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @stability Prototype. This function names are highly likely to be refactored.
 *  @ingroup MprMem
 */
extern void *mprAllocObjZeroed(MprCtx ctx, Type type) { return 0; }

/**
 *  Allocate an object of a given type with a destructor
 *  @description Allocates a block of memory large enough to hold an instance of the specified type with a destructor. 
 *      This uses the supplied memory context \a ctx as the parent. This is implemented as a macro.
 *      this call associates a destructor function with an object. This function will be invoked when the object is freed. 
 *      Freeing a block will first call the destructor and if that returns zero, mprFree will release the allocated 
 *      block and all child blocks.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param type Type of the object to allocate
 *  @param destructor Destructor function to invoke when the allocation is freed via #mprFree.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @stability Prototype. This function names are highly likely to be refactored.
 *  @ingroup MprMem
 */
extern void *mprAllocObjWithDestructor(MprCtx ctx, Type type, MprDestructor destructor)

/**
 *  Allocate a zeroed object of a given type with a destructor
 *  @description Allocates a zeroed block of memory large enough to hold an instance of the specified type with a 
 *      destructor. This uses the supplied memory context \a ctx as the parent. This is implemented as a macro.
 *      this call associates a destructor function with an object. This function will be invoked when the object is freed. 
 *      Freeing a block will first call the destructor and if that returns zero, mprFree will release the allocated 
 *      block and all child blocks.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param type Type of the object to allocate
 *  @param destructor Destructor function to invoke when the allocation is freed via #mprFree.
 *  @return Returns a pointer to the allocated block. If memory is not available the memory exhaustion handler 
 *      specified via mprCreate will be called to allow global recovery.
 *  @remarks Do not mix calls to malloc and mprAlloc.
 *  @stability Prototype. This function names are highly likely to be refactored.
 *  @ingroup MprMem
 */
extern void *mprAllocObjWithDestructorZeroed(MprCtx ctx, Type type, MprDestructor destructor) { return 0;}

/**
 *  Duplicate a block of memory.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @description Copy a block of memory into a newly allocated block.
 *  @param ptr Pointer to the block to duplicate.
 *  @param size Size of the block to copy.
 *  @return Returns an allocated block. Caller must free via #mprFree.
 *  @ingroup MprMem
 */
extern void *mprMemdup(MprCtx ctx, cvoid *ptr, uint size);

/**
 *  Duplicate a string.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @description Copy a string into a newly allocated block. The new block will be sized to the maximum of the 
 *      length of the existing string (plus a null) and the requested size.
 *  @param str Pointer to the block to duplicate.
 *  @param size Requested minimum size of the allocated block holding the duplicated string.
 *  @return Returns an allocated block. Caller must free via #mprFree.
 *  @ingroup MprMem
 */
extern char *mprStrndup(MprCtx ctx, cchar *str, uint size);

/**
 *  Safe replacement for strdup
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @description mprStrdup() should be used as a replacement for \b strdup wherever possible. It allows the 
 *      strdup to be copied to be NULL, in which case it will allocate an empty string. 
 *  @param str Pointer to string to duplicate. If \b str is NULL, allocate a new string containing only a 
 *      trailing NULL character.
 *  @return Returns an allocated string including trailing null.
 *  @remarks Memory allocated via mprStrdup() must be freed via mprFree().
 *  @ingroup MprMem
 */
extern char *mprStrdup(MprCtx ctx, cchar *str);

#else /* !DOXYGEN */


#if BLD_DEBUG
/**
 *  Set a static debug name for the memory object
 *  @param ptr The memory object
 *  @param name Static memory name. Must be persistant.
 *  @returns the memory object so this call can be chained.
 */
extern void *mprSetName(void *ptr, cchar *name);

/**
 *  Set a dynamic debug name for the memory object
 *  @param ptr The memory object
 *  @param name Memory name. This call will duplicate the name so the name may be transient.
 *  @returns the memory object so this call can be chained.
 */
extern void *mprSetDynamicName(void *ptr, cchar *name);

/*
 *  Get the debug memory name for a memory object
 *  @returns the debug name
 */
extern cchar *mprGetName(void *ptr);
#else

#define mprSetName(ptr, name) ptr
#define mprSetDynamicName(ptr, name) ptr
#define mprGetName(ptr) ""
#endif

extern void *_mprAlloc(MprCtx ctx, uint size);
extern MprBlk *_mprAllocBlock(MprCtx ctx, MprHeap *heap, MprBlk *parent, uint size);
extern void *_mprAllocWithDestructor(MprCtx ctx, uint size, MprDestructor destructor);
extern void *_mprAllocWithDestructorZeroed(MprCtx ctx, uint size, MprDestructor destructor);
extern void *_mprAllocZeroed(MprCtx ctx, uint size);
extern void *_mprRealloc(MprCtx ctx, void *ptr, uint size);
extern void *_mprMemdup(MprCtx ctx, cvoid *ptr, uint size);
extern char *_mprStrndup(MprCtx ctx, cchar *str, uint size);
extern char *_mprStrdup(MprCtx ctx, cchar *str);

/*
 *  Macros for typed based allocations
 */
#define mprAllocObj(ctx, type) \
    ((type*) mprSetName(_mprAlloc(ctx, sizeof(type)), MPR_LOC))
#define mprAllocObjZeroed(ctx, type) \
    ((type*) mprSetName(_mprAllocZeroed(ctx, sizeof(type)), MPR_LOC))
#define mprAllocObjWithDestructor(ctx, type, destructor) \
    ((type*) mprSetName(_mprAllocWithDestructor(ctx, sizeof(type), (MprDestructor) destructor), MPR_LOC))
#define mprAllocObjWithDestructorZeroed(ctx, type, destructor) \
    ((type*) mprSetName(_mprAllocWithDestructorZeroed(ctx, sizeof(type), (MprDestructor) destructor), MPR_LOC))

#define mprAlloc(ctx, size) \
    mprSetName(_mprAlloc(ctx, size), MPR_LOC)
#define mprAllocWithDestructor(ctx, size, destructor) \
    mprSetName(_mprAllocWithDestructor(ctx, size, destructor), MPR_LOC)
#define mprAllocWithDestructorZeroed(ctx, size, destructor) \
    mprSetName(_mprAllocWithDestructorZeroed(ctx, size, destructor), MPR_LOC)
#define mprAllocZeroed(ctx, size) \
    mprSetName(_mprAllocZeroed(ctx, size), MPR_LOC)
#define mprRealloc(ctx, ptr, size) \
    mprSetName(_mprRealloc(ctx, ptr, size), MPR_LOC)
#define mprMemdup(ctx, ptr, size) \
    mprSetName(_mprMemdup(ctx, ptr, size), MPR_LOC)
#define mprStrndup(ctx, str, size) \
    mprSetName(_mprStrndup(ctx, str, size), MPR_LOC)
#define mprStrdup(ctx, str) \
    mprSetName(_mprStrdup(ctx, str), MPR_LOC)

extern MprHeap *mprGetHeap(MprBlk *bp);
#endif

/**
 *  Format a string into an allocated buffer.
 *  @description This call will dynamically allocate a buffer up to the specified maximum size and will format the 
 *      supplied arguments into the buffer.  A trailing null will always be appended. The call returns
 *      the size of the allocated string excluding the null.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param maxSize Maximum size to allocate for the buffer including the trailing null.
 *  @param fmt Printf style format string
 *  @return Returns the number of characters in the string.
 *  @ingroup MprString
 */
extern char *mprAsprintf(MprCtx ctx, int maxSize, cchar *fmt, ...);

/**
 *  Allocate a buffer of sufficient length to hold the formatted string.
 *  @description This call will dynamically allocate a buffer up to the specified maximum size and will format 
 *      the supplied arguments into the buffer. A trailing null will always be appended. The call returns
 *      the size of the allocated string excluding the null.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param maxSize Maximum size to allocate for the buffer including the trailing null.
 *  @param fmt Printf style format string
 *  @param arg Varargs argument obtained from va_start.
 *  @return Returns the number of characters in the string.
 *  @ingroup MprString
 */
extern char *mprVasprintf(MprCtx ctx, int maxSize, cchar *fmt, va_list arg);

/**
 *  Append strings to an existing string and reallocate as required.
 *  @description Append a list of strings to an existing string. The list of strings is terminated by a 
 *      null argument. The call returns the size of the allocated block. 
 *  @param ctx Any memory context allocated by the MPR.
 *  @param max Maximum size of the result string.
 *  @param buf Existing string to reallocate. May be null.
 *  @param src Variable list of strings to append. The final string argument must be null.
 *  @return An allocated result string. Caller must free.
 *  @ingroup MprString
 */
extern char *mprReallocStrcat(MprCtx ctx, int max, char *buf, cchar *src, ...);

/**
 *  Free a block of memory.
 *  @description mprFree should be used to free memory allocated by mprAlloc, or mprRealloc. This call will ignore
 *      calls to free a null pointer, thus it is an acceptable idiom to free a pointer without testing its value for null.
 *      When mprFree is called it will first invoke any object destructor function for the allocated block. If this
 *      destructor returns zero, it will then proceed and free all allocated children before finally releasing the block.
 *  @param ptr Memory to free. If NULL, take no action.
 *  @return Returns zero if the block was actually freed. If the destructor prevented the freeing, a non-zero value will
 *      be returned. 
 *  @ingroup MprMem
 */
extern int mprFree(void *ptr);

/**
 *  Update the destructor for a block of memory.
 *  @description This call updates the destructor for a block of memory allocated via mprAllocWithDestructor.
 *  @param ptr Memory to free. If NULL, take no action.
 *  @param destructor Destructor function to invoke when #mprFree is called.
 *  @ingroup MprMem
 */
extern void mprSetDestructor(void *ptr, MprDestructor destructor);

/**
 *  Free all the children blocks allocated of a block
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 */
extern void mprFreeChildren(MprCtx ctx);

/**
 *  Reassign a block from its current parent context to a new context.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate. This will be the new owning context of the ptr.
 *  @param ptr Pointer to a block to reassign.
 */
extern int mprStealBlock(MprCtx ctx, cvoid *ptr);
extern int mprReparent(MprCtx ctx, cvoid *ptr);

#if BLD_FEATURE_MEMORY_DEBUG
extern void     mprValidateBlock(MprCtx ctx);
#endif
#if BLD_FEATURE_MEMORY_STATS
extern void     mprPrintAllocReport(MprCtx ctx, cchar *msg);
#endif


/**
 *  Determine if the MPR has encountered memory allocation errors.
 *  @description Returns true if the MPR has had a memory allocation error. Allocation errors occur if any
 *      memory allocation would cause the application to exceed the configured redline limit, or if any O/S memory
 *      allocation request fails.
 *  @param ctx Any memory context allocated by the MPR.
 *  @return TRUE if a memory allocation error has occurred. Otherwise returns FALSE.
 *  @ingroup MprMem
 */
extern bool mprHasAllocError(MprCtx ctx);

/**
 *  Reset the memory allocation error flag
 *  @description Reset the alloc error flag triggered.
 *  @param ctx Any memory context allocated by the MPR.
 *  @ingroup MprMem
 */
extern void mprResetAllocError(MprCtx ctx);

/**
 *  Set an memory allocation error condition on a memory context. This will set an allocation error condition on the
 *  given context and all its parents. This way, you can test the ultimate parent and detect if any memory allocation
 *  errors have occurred.
 *  @param ctx Any memory context allocated by the MPR.
 */
extern void mprSetAllocError(MprCtx ctx);

/**
 *  Get the memory parent of a block.
 *  @description Return the parent memory context for a block
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @return Return the memory owning this block
 *  @ingroup MprMem
 */
extern void *mprGetParent(MprCtx ctx);

/**
 *  Test is a pointer is a valid memory context. This is used to test if a block has been dynamically allocated.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 */
extern int mprIsValid(MprCtx ctx);

/**
 *  Monitory stack usage and check if the stack has grown since last monitoring.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @returns true if the stack has grown
 */
extern bool mprStackCheck(MprCtx ctx);

/**
 *  Configure the application memory limits
 *  @description Configure memory limits to constrain memory usage by the application. The memory allocation subsystem
 *      will check these limits before granting memory allocation requrests. The redLine is a soft limit that if exceeded
 *      will invoke the memory allocation callback, but will still honor the request. The maxMemory limit is a hard limit.
 *      The MPR will prevent allocations which exceed this maximum. The memory callback handler is defined via 
 *      the #mprCreate call.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @param redline Soft memory limit. If exceeded, the request will be granted, but the memory handler will be invoked.
 *  @param maxMemory Hard memory limit. If exceeded, the request will not be granted, and the memory handler will be invoked.
 *  @ingroup MprMem
 */
extern void mprSetAllocLimits(MprCtx ctx, int redline, int maxMemory);

/**
 *  Return the current allocation memory statistics block
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @returns a reference to the allocation memory statistics. Do not modify its contents.
 */
extern MprAlloc *mprGetAllocStats(MprCtx ctx);

/**
 *  Return the amount of memory currently used by the application. This only reports heap memory.
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @returns the amount of heap memory used by the application in bytes.
 */
extern int64 mprGetUsedMemory(MprCtx ctx);

/**
 *  Memory virtual memory into the applications address space.
 *  @param size of virtual memory to map. This size will be rounded up to the nearest page boundary.
 *  @param mode Mask set to MPR_MAP_READ | MPR_MAP_WRITE
 */
extern void *mprMapAlloc(MprCtx ctx, uint size, int mode);

/**
 *  Free (unpin) a mapped section of virtual memory
 *  @param ptr Virtual address to free. Should be page aligned
 *  @param size Size of memory to free in bytes
 */
extern void mprMapFree(void *ptr, uint size);

/**
 *  Get the current O/S virtual page size
 *  @param ctx Any memory context allocated by mprAlloc or mprCreate.
 *  @returns the page size in bytes
 */
extern int mprGetPageSize(MprCtx ctx);

/*
 *  Wait service.
 */
/*
 *  Standard wait for IO options
 */
#define MPR_READABLE           0x2          /* The channel is readable without blocking */
#define MPR_WRITABLE           0x4          /* The channel is writable without blocking */

/*
 *  Wait service flags
 */
#define MPR_BREAK_REQUESTED     0x1         /* Pending wakeup on service thread */
#define MPR_NEED_RECALL         0x2         /* A handler needs to be recalled */

#define MPR_READ_PIPE           0           /* Read side */
#define MPR_WRITE_PIPE          1           /* Write side */

/*
 *  Wait handler callback
 *  @params data User supplied callback data
 *  @params mask Mask set to MPR_READABLE | MPR_WRITABLE
 *  @returns non-zero if the wait handler has been deleted
 */
typedef int (*MprWaitProc)(void *data, int mask);

#if BLD_WIN_LIKE
typedef long (*MprMsgCallback)(HWND hwnd, uint msg, uint wp, long lp);
#endif

typedef struct MprWaitService {
    MprList         *handlers;              /* List of handlers */
    int             flags;                  /* State flags */
    int             maskGeneration;         /* Generation number for mask changes */
    int             lastMaskGeneration;     /* Last generation number for mask changes */
    int             rebuildMasks;           /* IO mask rebuild required */

#if LINUX || MACOSX || FREEBSD
    struct pollfd   *fds;                   /* File descriptors to poll on */
    int             fdsCount;               /* Count of fds */
    int             fdsSize;                /* Size of fds array */
    int             breakPipe[2];           /* Pipe to wakeup select when multithreaded */

#elif BLD_WIN_LIKE && !WINCE
    HWND            hwnd;                   /* Window handle */
    int             socketMessage;          /* Message id for socket events */
    MprMsgCallback  msgCallback;            /* Message handler callback */
#else
    /*
     *  Everyone else uses select (including WINCE)
     */
    fd_set          readMask;               /* Current read events mask */
    fd_set          writeMask;              /* Current write events mask */
    fd_set          selectReadMask;         /* Read events mask used for select() */
    fd_set          selectWriteMask;        /* Write events mask used for select() */
    int             maxfd;                  /* Highest socket in masks + 1 */
#if BLD_FEATURE_MULTITHREAD
    int             breakSock;              /* Socket to wakeup select when multithreaded */
    struct sockaddr_in breakAddress;        /* Address of wakeup socket */
#endif
#endif

#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;                 /* General multi-thread sync */
#endif

} MprWaitService;


extern MprWaitService *mprCreateWaitService(struct Mpr *mpr);
extern int  mprInitSelectWait(MprWaitService *ws);
extern int  mprStartWaitService(MprWaitService *ws);
extern int  mprStopWaitService(MprWaitService *ws);

#if BLD_WIN_LIKE
extern int  mprInitWindow(MprWaitService *ws);
extern void mprSetWinMsgCallback(MprWaitService *ws, MprMsgCallback callback);
extern void mprServiceWinIO(MprWaitService *ws, int sockFd, int winMask);
#endif

#if BLD_FEATURE_MULTITHREAD
    extern void mprSetServiceThread(MprCtx ctx, MprThread *thread);
    extern void mprSetWaitServiceThread(MprWaitService *ws, MprThread *thread);
    extern void mprWakeWaitService(MprCtx ctx);
    extern void mprWakeOsWaitService(MprCtx ctx);
#else
    #define mprWakeWaitService(ws)
#endif

extern int mprWaitForSingleIO(MprCtx ctx, int fd, int mask, int timeout);
extern int mprWaitForIO(MprWaitService *ws, int timeout);


/*
 *  Handler Flags
 */
#define MPR_WAIT_RECALL_HANDLER 0x1     /* Must recall the handler asap */
#define MPR_WAIT_THREAD         0x2     /* Run callback via thread worker */
#define MPR_WAIT_DESTROYING     0x4     /* Destroy in process */
#define MPR_WAIT_MASK_CHANGED   0x8     /* Wait masks have changed */

/**
 *  Wait Handler Service
 *  @description Wait handlers provide callbacks for when I/O events occur. They provide a wait to service many
 *      I/O file descriptors without requiring a thread per descriptor.
 *  @see mprSetWaitEvents, mprSetWaitCallback, mprDisableWaitEvents, mprEnableWaitEvents,
 *      mprRecallWaitHandler, MprWaitHandler, mprCreateEvent, mprServiceEvents, MprEvent
 *  @defgroup MprWaitHandler MprWaitHandler
 */
typedef struct MprWaitHandler {
    int             desiredMask;        /**< Mask of desired events */
    int             disableMask;        /**< Mask of disabled events */
    int             presentMask;        /**< Mask of current events */
    int             fd;                 /**< O/S File descriptor (sp->sock) */
    int             flags;              /**< Control flags */
    int             inUse;              /**< In-use counter. Used by callbacks */
    void            *handlerData;       /**< Argument to pass to proc */
#if BLD_FEATURE_MULTITHREAD
    int             priority;           /**< Thread priority */
    struct MprWorker *requiredWorker;   /**< Designate the required worker thread to run the callback */
    struct MprThread *thread;           /**< Thread executing the callback, set even if worker is null */
    MprCond         *callbackComplete;  /**< Signalled when a callback is complete */
#endif
    MprWaitService  *waitService;       /**< Wait service pointer */
    MprWaitProc     proc;               /**< Wait handler procedure */
    struct MprWaitHandler *next;        /**< List linkage */
    struct MprWaitHandler *prev;
} MprWaitHandler;


/**
 *  Create a wait handler
 *  @description Create a wait handler that will be invoked when I/O of interest occurs on the specified file handle
 *      The wait handler is registered with the MPR event I/O mechanism.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param fd File descriptor
 *  @param mask Mask of events of interest. This is made by oring MPR_READABLE and MPR_WRITABLE
 *  @param proc Callback function to invoke when an I/O event of interest has occurred.
 *  @param data Data item to pass to the callback
 *  @param priority MPR priority to associate with the callback. This is only used if the MPR_WAIT_THREAD is specified
 *      in the flags and the MPR is build multithreaded.
 *  @param flags Flags may be set to MPR_WAIT_THREAD if the callback function should be invoked using a thread from
 *      the worker thread pool.
 *  @returns A new wait handler registered with the MPR event mechanism
 *  @ingroup MprWaitHandler
 */
extern MprWaitHandler *mprCreateWaitHandler(MprCtx ctx, int fd, int mask, MprWaitProc proc, void *data,
        int priority, int flags);

/**
 *  Disconnect a wait handler from its underlying file descriptor. This is used to prevent further I/O wait events while
 *  still preserving the wait handler.
 *  @param wp Wait handler object
 */
extern void mprDisconnectWaitHandler(MprWaitHandler *wp);

/**
 *  Disable wait events
 *  @description Disable wait events for a given file descriptor.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @ingroup MprWaitHandler
 */
extern void mprDisableWaitEvents(MprWaitHandler *wp);

/**
 *  Enable wait events
 *  @description Enable wait events for a given file descriptor.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @ingroup MprWaitHandler
 */
extern void mprEnableWaitEvents(MprWaitHandler *wp);

/**
 *  Recall a wait handler
 *  @description Signal that a wait handler should be recalled at the earliest opportunity. This is useful
 *      when a protocol stack has buffered data that must be processed regardless of whether more I/O occurs. 
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @ingroup MprWaitHandler
 */
extern void mprRecallWaitHandler(MprWaitHandler *wp);

/**
 *  Define the events of interest for a wait handler
 *  @description Define the events of interest for a wait handler. The mask describes whether readable or writable
 *      events should be signalled to the wait handler. Disconnection events are passed via read events.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @param desiredMask Mask of desirable events (MPR_READABLE | MPR_WRITABLE)
 *  @param disableMask Mask to enable or disable events. Set to -1 to enable events, 0 to disable.
 *  @ingroup MprWaitHandler
 */
extern void mprSetWaitEvents(MprWaitHandler *wp, int desiredMask, int disableMask);

/**
 *  Define the wait handler callback
 *  @description This updates the callback function for the wait handler. Callback functions are originally specified
 *      via #mprCreateWaitHandler.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @param proc Callback function to invoke when an I/O event of interest has occurred.
 *  @param mask Mask of MPR_READABLE and MPR_WRITABLE
 *  @ingroup MprWaitHandler
 */
extern void mprSetWaitCallback(MprWaitHandler *wp, MprWaitProc proc, int mask);

/**
 *  Apply wait handler updates. While a wait handler is in use, wait event updates are buffered. This routine applies
 *      such buffered updates.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @param wakeup Wake up the service events thread. Typically it is safest to wake up the service events thread if the
 *      wait handler event masks have been modified. However, there are some cases where it can be useful to suppress
 *      this behavior.
 */
extern void mprUpdateWaitHandler(MprWaitHandler *wp, bool wakeup);

/**
 *  Invoke the wait handler callback
 *  @param wp Wait handler created via #mprCreateWaitHandler
 */
extern void mprInvokeWaitCallback(MprWaitHandler *wp);

#if BLD_FEATURE_MULTITHREAD
/**
 *  Dedicate a worker thread to a wait handler. This implements thread affinity and is required on some platforms
 *      where some APIs (waitpid on uClibc) cannot be called on a different thread.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @param worker Worker thread reference
 */
extern void mprDedicateWorkerToHandler(MprWaitHandler *wp, struct MprWorker *worker);

/**
 *  Release a worker thread. This releases a worker thread to be assignable to any other wait handler.
 *  @param wp Wait handler created via #mprCreateWaitHandler
 *  @param worker Worker thread reference
 */
extern void mprReleaseWorkerFromHandler(MprWaitHandler *wp, struct MprWorker *worker);
#endif

/**
 *  Socket I/O callback procedure. Proc returns non-zero if the socket has been deleted.
 */
typedef int (*MprSocketProc)(void *data, int mask);

/**
 *  Socket connection acceptance callback procedure
 */
typedef int (*MprSocketAcceptProc)(struct MprSocket *sp, void *data, cchar *ip, int port);

/*
 *  Socket service provider interface.
 */
typedef struct MprSocketProvider {
    cchar           *name;
#if BLD_FEATURE_SSL
    struct MprSsl   *defaultSsl;
#endif
    struct MprSocket *(*acceptSocket)(struct MprSocket *sp, bool invokeCallback);
    void            (*closeSocket)(struct MprSocket *socket, bool gracefully);
    int             (*configureSsl)(struct MprSsl *ssl);
    int             (*connectSocket)(struct MprSocket *socket, cchar *host, int port, int flags);
    struct MprSocket *(*createSocket)(MprCtx ctx, struct MprSsl *ssl);
    void            (*disconnectSocket)(struct MprSocket *socket);
    int             (*flushSocket)(struct MprSocket *socket);
    int             (*listenSocket)(struct MprSocket *socket, cchar *host, int port, MprSocketAcceptProc acceptFn, 
                        void *data, int flags);
    int             (*readSocket)(struct MprSocket *socket, void *buf, int len);
    int             (*writeSocket)(struct MprSocket *socket, void *buf, int len);
} MprSocketProvider;


/*
 *  Mpr socket service class
 */
typedef struct MprSocketService {
    MprList         *connections;
    int             maxClients;                 /* Maximum client side sockets */
    int             numClients;                 /* Count of client side sockets */
    int             next;

    MprSocketProvider *standardProvider;
    MprSocketProvider *secureProvider;

#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;
#endif
} MprSocketService;


extern MprSocketService *mprCreateSocketService(MprCtx ctx);
extern int  mprStartSocketService(MprSocketService *ss);
extern void mprStopSocketService(MprSocketService *ss);
extern void mprSetSecureProvider(MprCtx ctx, MprSocketProvider *provider);
extern bool mprHasSecureSockets(MprCtx ctx);

/**
 *  Set the maximum number of client sockets that are permissable
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param max New maximum number of client sockets.
 */
extern int mprSetMaxSocketClients(MprCtx ctx, int max);

/*
 *  Socket close flags
 */
#define MPR_SOCKET_GRACEFUL     1           /* Do a graceful shutdown */

/*
 *  Socket event types
 */
#define MPR_SOCKET_READABLE     0x2
#define MPR_SOCKET_WRITABLE     0x4
#define MPR_SOCKET_EXCEPTION    0x8

/*
 *  Socket Flags
 */
#define MPR_SOCKET_BLOCK        0x1         /**< Use blocking I/O */
#define MPR_SOCKET_BROADCAST    0x2         /**< Broadcast mode */
#define MPR_SOCKET_CLOSED       0x4         /**< MprSocket has been closed */
#define MPR_SOCKET_CONNECTING   0x8         /**< MprSocket has been closed */
#define MPR_SOCKET_DATAGRAM     0x10        /**< Use datagrams */
#define MPR_SOCKET_EOF          0x20        /**< Seen end of file */
#define MPR_SOCKET_LISTENER     0x40        /**< MprSocket is server listener */
#define MPR_SOCKET_NOREUSE      0x80        /**< Dont set SO_REUSEADDR option */
#define MPR_SOCKET_NODELAY      0x100       /**< Disable Nagle algorithm */
#define MPR_SOCKET_THREAD       0x400       /**< Process callbacks on a worker thread */
#define MPR_SOCKET_CLIENT       0x800       /**< Socket is a client */
#define MPR_SOCKET_PENDING      0x1000      /**< Pending buffered read data */
#define MPR_SOCKET_RUNNING      0x2000      /**< Socket is running callback */

/**
 *  Socket Service
 *  @description The MPR Socket service provides IPv4 and IPv6 capabilities for both client and server endpoints.
 *  Datagrams, Broadcast and point to point services are supported. The APIs can be used in both blocking and
 *  non-blocking modes.
 *  \n\n
 *  The socket service integrates with the MPR worker thread pool and eventing services. Socket connections can be handled
 *  by threads from the worker thread pool for scalable, multithreaded applications.
 *
 *  @stability Evolving
 *  @see MprSocket, mprCreateSocket, mprOpenClientSocket, mprOpenServerSocket, mprCloseSocket, mprFree, mprFlushSocket,
 *      mprWriteSocket, mprWriteSocketString, mprReadSocket, mprSetSocketCallback, mprSetSocketEventMask, 
 *      mprGetSocketBlockingMode, mprIsSocketEof, mprGetSocketFd, mprGetSocketPort, mprGetSocketBlockingMode, 
 *      mprSetSocketNoDelay, mprGetSocketError, mprParseIp, mprSendFileToSocket, mprSetSocketEof, mprIsSocketSecure
 *      mprWriteSocketVector
 *  @defgroup MprSocket MprSocket
 */
typedef struct MprSocket {
    MprSocketService *service;          /**< Socket service */
    MprSocketAcceptProc
                    acceptCallback;     /**< Accept callback */
    void            *acceptData;        /**< User accept callback data */
    int             currentEvents;      /**< Mask of ready events (FD_x) */
    int             error;              /**< Last error */
    int             handlerMask;        /**< Handler events of interest */
    int             handlerPriority;    /**< Handler priority */
    int             interestEvents;     /**< Mask of events to watch for */
    MprSocketProc   ioCallback;         /**< User I/O callback */
    void            *ioData;            /**< User io callback data */
    char            *ipAddr;            /**< Server side ip address */
    char            *clientIpAddr;      /**< Client side ip address */
    int             port;               /**< Port to listen on */
    int             waitForEvents;      /**< Events being waited on */
    MprWaitHandler  *handler;           /**< Wait handler */
    int             fd;                 /**< Actual socket file handle */
    int             flags;              /**< Current state flags */
    MprSocketProvider *provider;        /**< Socket implementation provider */
    struct MprSocket *listenSock;       /**< Listening socket */
    struct MprSslSocket *sslSocket;     /**< Extended ssl socket state. If set, then using ssl */
    struct MprSsl   *ssl;               /**< SSL configuration */
#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;             /**< Multi-thread sync */
#endif

} MprSocket;


/*
 *  Vectored write array
 */
typedef struct MprIOVec {
    char            *start;
    size_t          len;
} MprIOVec;


/**
 *  Flag for mprCreateSocket to use the default SSL provider
 */ 
#define MPR_SECURE_CLIENT ((struct MprSsl*) 1)

/**
 *  Create a socket
 *  @description Create a new socket
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param ssl An optional SSL context if the socket is to support SSL. Use the #MPR_SECURE_CLIENT define to specify
 *      that mprCreateSocket should use the default SSL provider.
 *  @return A new socket object
 *  @ingroup MprSocket
 */
extern MprSocket *mprCreateSocket(MprCtx ctx, struct MprSsl *ssl);

/**
 *  Open a client socket
 *  @description Open a client connection
 *  @param sp Socket object returned via #mprCreateSocket
 *  @param hostName Host or IP address to connect to.
 *  @param port TCP/IP port number to connect to.
 *  @param flags Socket flags may use the following flags ored together:
 *      @li MPR_SOCKET_BLOCK - to use blocking I/O. The default is non-blocking.
 *      @li MPR_SOCKET_BROADCAST - Use IPv4 broadcast
 *      @li MPR_SOCKET_DATAGRAM - Use IPv4 datagrams
 *      @li MPR_SOCKET_NOREUSE - Set NOREUSE flag on the socket
 *      @li MPR_SOCKET_NODELAY - Set NODELAY on the socket
 *      @li MPR_SOCKET_THREAD - Process callbacks on a separate thread.
 *  @return Zero if the connection is successful. Otherwise a negative MPR error code.
 *  @ingroup MprSocket
 */
extern int mprOpenClientSocket(MprSocket *sp, cchar *hostName, int port, int flags);

/**
 *  Disconnect a socket by closing its underlying file descriptor. This is used to prevent further I/O wait events while
 *  still preserving the socket object.
 *  @param sp Socket object
 */
extern void mprDisconnectSocket(MprSocket *sp);

/**
 *  Open a server socket
 *  @description Open a server socket and listen for client connections.
 *  @param sp Socket object returned via #mprCreateSocket
 *  @param ipAddr IP address to bind to. Set to 0.0.0.0 to bind to all possible addresses on a given port.
 *  @param port TCP/IP port number to connect to. 
 *  @param acceptFn Callback function to invoke to accept incoming client connections.
 *  @param data Opaque data reference to pass to the accept function.
 *  @param flags Socket flags may use the following flags ored together:
 *      @li MPR_SOCKET_BLOCK - to use blocking I/O. The default is non-blocking.
 *      @li MPR_SOCKET_BROADCAST - Use IPv4 broadcast
 *      @li MPR_SOCKET_DATAGRAM - Use IPv4 datagrams
 *      @li MPR_SOCKET_NOREUSE - Set NOREUSE flag on the socket
 *      @li MPR_SOCKET_NODELAY - Set NODELAY on the socket
 *      @li MPR_SOCKET_THREAD - Process callbacks on a separate thread.
 *  @return Zero if the connection is successful. Otherwise a negative MPR error code.
 *  @ingroup MprSocket
 */
extern int mprOpenServerSocket(MprSocket *sp, cchar *ipAddr, int port, MprSocketAcceptProc acceptFn, void *data, int flags);

/**
 *  Close a socket
 *  @description Close a socket. If the \a graceful option is true, the socket will first wait for written data to drain
 *      before doing a graceful close.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param graceful Set to true to do a graceful close. Otherwise, an abortive close will be performed.
 *  @ingroup MprSocket
 */
extern void mprCloseSocket(MprSocket *sp, bool graceful);

/**
 *  Flush a socket
 *  @description Flush any buffered data in a socket. Standard sockets do not use buffering and this call will do nothing.
 *      SSL sockets do buffer and calling mprFlushSocket will write pending written data.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return A count of bytes actually written. Return a negative MPR error code on errors.
 *  @ingroup MprSocket
 */
extern int mprFlushSocket(MprSocket *sp);

/**
 *  Write to a socket
 *  @description Write a block of data to a socket. If the socket is in non-blocking mode (the default), the write
 *      may return having written less than the required bytes. 
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param buf Reference to a block to write to the socket
 *  @param len Length of data to write. This may be less than the requested write length if the socket is in non-blocking
 *      mode. Will return a negative MPR error code on errors.
 *  @return A count of bytes actually written. Return a negative MPR error code on errors.
 *  @ingroup MprSocket
 */
extern int mprWriteSocket(MprSocket *sp, void *buf, int len);

/**
 *  Write to a string to a socket
 *  @description Write a string  to a socket. If the socket is in non-blocking mode (the default), the write
 *      may return having written less than the required bytes. 
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param str Null terminated string to write.
 *  @return A count of bytes actually written. Return a negative MPR error code on errors.
 *  @ingroup MprSocket
 */
extern int mprWriteSocketString(MprSocket *sp, cchar *str);

/**
 *  Read from a socket
 *  @description Read data from a socket. The read will return with whatever bytes are available. If none and the socket
 *      is in blocking mode, it will block untill there is some data available or the socket is disconnected.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param buf Pointer to a buffer to hold the read data. 
 *  @param size Size of the buffer.
 *  @return A count of bytes actually read. Return a negative MPR error code on errors.
 *  @ingroup MprSocket
 */
extern int mprReadSocket(MprSocket *sp, void *buf, int size);

/**
 *  Set the socket callback.
 *  @description Define a socket callback function to invoke in response to socket I/O events.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param fn Callback function.
 *  @param data Data to pass with the callback.
 *  @param mask Bit mask of events of interest. Set to MPR_READABLE and/or MPR_WRITABLE.
 *  @param priority Priority to associate with the event. Priorities are integer values between 0 and 100 inclusive with
 *      50 being a normal priority. (See #MPR_NORMAL_PRIORITY).
 *  @ingroup MprSocket
 */
extern void mprSetSocketCallback(MprSocket *sp, MprSocketProc fn, void *data, int mask, int priority);

/**
 *  Define the events of interest for a socket
 *  @description Define an event mask of interest for a socket. The mask is made by oring the MPR_READABLE and MPR_WRITABLE
 *      flags as requried
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param mask Set to true to do a graceful close. Otherwise, an abortive close will be performed.
 *  @ingroup MprSocket
 */
extern void mprSetSocketEventMask(MprSocket *sp, int mask);

/**
 *  Get the socket blocking mode.
 *  @description Return the current blocking mode setting.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return True if the socket is in blocking mode. Otherwise false.
 *  @ingroup MprSocket
 */
extern bool mprGetSocketBlockingMode(MprSocket *sp);

/**
 *  Test if the socket has buffered read data.
 *  @description Use this function to avoid waiting for incoming I/O if data is already buffered.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return True if the socket has pending read data.
 *  @ingroup MprSocket
 */
extern bool mprHasSocketPendingData(MprSocket *sp);

/**
 *  Test if the other end of the socket has been closed.
 *  @description Determine if the other end of the socket has been closed and the socket is at end-of-file.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return True if the socket is at end-of-file.
 *  @ingroup MprSocket
 */
extern bool mprIsSocketEof(MprSocket *sp);

/**
 *  Get the socket file descriptor.
 *  @description Get the file descriptor associated with a socket.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return The integer file descriptor used by the O/S for the socket.
 *  @ingroup MprSocket
 */
extern int mprGetSocketFd(MprSocket *sp);

/**
 *  Get the port used by a socket
 *  @description Get the TCP/IP port number used by the socket.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return The integer TCP/IP port number used by the socket.
 *  @ingroup MprSocket
 */
extern int mprGetSocketPort(MprSocket *sp);

/**
 *  Set the socket blocking mode.
 *  @description Set the blocking mode for a socket. By default a socket is in non-blocking mode where read / write
 *      calls will not block.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param on Set to zero to put the socket into non-blocking mode. Set to non-zero to enable blocking mode.
 *  @return The old blocking mode if successful or a negative MPR error code.
 *  @ingroup MprSocket
 */
extern int mprSetSocketBlockingMode(MprSocket *sp, bool on);

/**
 *  Set the socket delay mode.
 *  @description Set the socket delay behavior (nagle algorithm). By default a socket will partial packet writes
 *      a little to try to accumulate data and coalesce TCP/IP packages. Setting the delay mode to false may
 *      result in higher performance for interactive applications.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param on Set to non-zero to put the socket into no delay mode. Set to zero to enable the nagle algorithm.
 *  @return The old delay mode if successful or a negative MPR error code.
 *  @ingroup MprSocket
 */
extern int mprSetSocketNoDelay(MprSocket *sp, bool on);

/**
 *  Get a socket error code
 *  @description This will map a Windows socket error code into a posix error code.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return A posix error code. 
 *  @ingroup MprSocket
 */
extern int mprGetSocketError(MprSocket *sp);

#if !BLD_FEATURE_ROMFS
/**
 *  Send a file to a socket
 *  @description Write the contents of a file to a socket. If the socket is in non-blocking mode (the default), the write
 *      may return having written less than the required bytes. This API permits the writing of data before and after
 *      the file contents. 
 *  @param file File to write to the socket
 *  @param sock Socket object returned from #mprCreateSocket
 *  @param offset offset within the file from which to read data
 *  @param bytes Length of file data to write
 *  @param beforeVec Vector of data to write before the file contents
 *  @param beforeCount Count of entries in beforeVect
 *  @param afterVec Vector of data to write after the file contents
 *  @param afterCount Count of entries in afterCount
 *  @return A count of bytes actually written. Return a negative MPR error code on errors.
 *  @ingroup MprSocket
 */
extern MprOffset mprSendFileToSocket(MprSocket *sock, MprFile *file, MprOffset offset, int bytes, MprIOVec *beforeVec, 
    int beforeCount, MprIOVec *afterVec, int afterCount);
#endif

/**
 *  Set an EOF condition on the socket
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param eof Set to true to set an EOF condition. Set to false to clear it.
 */
extern void mprSetSocketEof(MprSocket *sp, bool eof);

/**
 *  Determine if the socket is secure
 *  @description Determine if the socket is using SSL to provide enhanced security.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @return True if the socket is using SSL, otherwise zero.
 *  @ingroup MprSocket
 */
extern bool mprIsSocketSecure(MprSocket *sp);

/**
 *  Write a vector to a socket
 *  @description Do scatter/gather I/O by writing a vector of buffers to a socket.
 *  @param sp Socket object returned from #mprCreateSocket
 *  @param iovec Vector of data to write before the file contents
 *  @param count Count of entries in beforeVect
 *  @return A count of bytes actually written. Return a negative MPR error code on errors.
 *  @ingroup MprSocket
 */
extern int mprWriteSocketVector(MprSocket *sp, MprIOVec *iovec, int count);

/**
 *  Enable socket events for a socket callback
 *  @param sp Socket object returned from #mprCreateSocket
 */
extern void mprEnableSocketEvents(MprSocket *sp);

/**
 *  Disable socket events for a socket callback
 *  @param sp Socket object returned from #mprCreateSocket
 */
extern void mprDisableSocketEvents(MprSocket *sp);

/**
 *  Parse an IP address. This parses a string containing an IP:PORT specification and returns the IP address and port 
 *  components. Handles ipv4 and ipv6 addresses. 
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param ipSpec An IP:PORT specification. The :PORT is optional. When an ipAddrPort contains an ipv6 port it should be 
 *  written as
 *      aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh:iiii    or
 *     [aaaa:bbbb:cccc:dddd:eeee:ffff:gggg:hhhh:iiii]:port
 *  @param ipAddr Pointer to receive a dynamically allocated IP string. Caller should free.
 *  @param port Pointer to an integer to receive the port value.
 *  @param defaultPort The default port number to use if the ipSpec does not contain a port
 */
extern int mprParseIp(MprCtx ctx, cchar *ipSpec, char **ipAddr, int *port, int defaultPort);

#if BLD_FEATURE_SSL
/*
 *  These are here to reduce namespace clutter, so users who want SSL don't have to include mprSsl.h and thus 
 *  pull in ssl headers.
 */
/**
 *  Load the SSL module.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param lazy Set to true to delay initialization until SSL is actually used.
 */
extern MprModule *mprLoadSsl(MprCtx ctx, bool lazy);

/**
 *  Configure SSL based on the parsed MprSsl configuration
 *  @param ssl MprSsl configuration
 */
extern void mprConfigureSsl(struct MprSsl *ssl);
#endif

#if BLD_FEATURE_MULTITHREAD

typedef struct MprWorkerStats {
    int             maxThreads;         /* Configured max number of threads */
    int             minThreads;         /* Configured minimum */
    int             numThreads;         /* Configured minimum */
    int             maxUse;             /* Max used */
    int             pruneHighWater;     /* Peak thread use in last minute */
    int             idleThreads;        /* Current idle */
    int             busyThreads;        /* Current busy */
} MprWorkerStats;

/**
 *  Worker Thread Service
 *  @description The MPR provides a worker thread pool for rapid starting and assignment of threads to tasks.
 *  @stability Evolving
 *  @see MprWorkerService, mprAvailableWorkers, mprSetMaxWorkers, mprSetMinWorkers
 *  @defgroup MprWorkerService MprWorkerService
 */
typedef struct MprWorkerService {
    int             nextTaskNum;        /* Unique next task number */
    MprList         *runningTasks;      /* List of executing tasks */
    int             stackSize;          /* Stack size for worker threads */
    MprList         *tasks;             /* Prioritized list of pending tasks */

    MprList         *busyThreads;       /* List of threads to service tasks */
    MprList         *idleThreads;       /* List of threads to service tasks */
    int             maxThreads;         /* Max # threads in worker pool */
    int             maxUseThreads;      /* Max threads ever used */
    int             minThreads;         /* Max # threads in worker pool */
    MprMutex        *mutex;             /* Per task synchronization */
    int             nextThreadNum;      /* Unique next thread number */
    int             numThreads;         /* Current number of threads in worker pool */
    int             pruneHighWater;     /* Peak thread use in last minute */
    struct MprEvent *pruneTimer;        /* Timer for excess threads pruner */
} MprWorkerService;


extern MprWorkerService *mprCreateWorkerService(MprCtx ctx);
extern int mprStartWorkerService(MprWorkerService *ws);
extern bool mprStopWorkerService(MprWorkerService *ws, int timeout);

/**
 *  Get the count of available worker threads
 *  Return the count of free threads in the worker thread pool.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @returns An integer count of worker threads.
 *  @ingroup MprWorkerService
 */
extern int mprGetAvailableWorkers(MprCtx ctx);

/**
 *  Set the default worker stack size
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param size Stack size in bytes
 */
extern void mprSetWorkerStackSize(MprCtx ctx, int size);

/**
 *  Set the minimum count of worker threads
 *  Set the count of threads the worker pool will have. This will cause the worker pool to pre-create at least this 
 *  many threads.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param count Minimum count of threads to use.
 *  @ingroup MprWorkerService
 */
extern void mprSetMinWorkers(MprCtx ctx, int count);

/**
 *  Set the maximum count of worker threads
 *  Set the maximum number of worker pool threads for the MPR. If this number if less than the current number of threads,
 *      excess threads will be gracefully pruned as they exit.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param count Maximum limit of threads to define.
 *  @ingroup MprWorkerService
 */
extern void mprSetMaxWorkers(MprCtx ctx, int count);

/**
 *  Get the maximum count of worker pool threads
 *  Get the maximum limit of worker pool threads. 
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @return The maximum count of worker pool threads.
 *  @ingroup MprWorkerService
 */
extern int mprGetMaxWorkers(MprCtx ctx);

extern void mprGetWorkerServiceStats(MprWorkerService *ps, MprWorkerStats *stats);

/*
 *  State
 */
#define MPR_WORKER_BUSY        0x1          /* Worker currently running to a callback */
#define MPR_WORKER_IDLE        0x2          /* Worker idle and available for work */
#define MPR_WORKER_PRUNED      0x4          /* Worker has been pruned and will be terminated */
#define MPR_WORKER_SLEEPING    0x8          /* Worker is sleeping (idle) on idleCond */

/*
 *  Flags
 */
#define MPR_WORKER_DEDICATED   0x1          /* Worker reserved and not part of the worker pool */

/**
 *  Worker thread callback signature
 *  @param data worker callback data. Set via mprStartWorker or mprActivateWorker
 *  @param worker Reference to the worker thread object
 */
typedef void (*MprWorkerProc)(void *data, struct MprWorker *worker);

/*
 *  Threads in the worker thread pool
 */
typedef struct MprWorker {
    MprWorkerProc   proc;                   /* Procedure to run */
    MprWorkerProc   cleanup;                /* Procedure to cleanup after run before sleeping */
    void            *data;
    int             priority;
    int             state;
    int             flags;
    MprThread       *thread;                /* Thread associated with this worker */
    MprWorkerService *workerService;        /* Worker service */
    MprCond         *idleCond;              /* Used to wait for work */
} MprWorker;

extern void mprActivateWorker(MprWorker *worker, MprWorkerProc proc, void *data, int priority);
extern int mprStartWorker(MprCtx ctx, MprWorkerProc proc, void *data, int priority);

/**
 *  Dedicate a worker thread to a current real thread. This implements thread affinity and is required on some platforms
 *      where some APIs (waitpid on uClibc) cannot be called on a different thread.
 *  @param worker Worker object
 *  @param worker Worker thread reference
 */
extern void mprDedicateWorker(MprWorker *worker);

/**
 *  Release a worker thread. This releases a worker thread to be assignable to any real thread.
 *  @param worker Worker object
 *  @param worker Worker thread reference
 */
extern void mprReleaseWorker(MprWorker *worker);

/*
 *  Get the worker object if the current thread is actually a worker thread.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @returns A worker thread object if the thread is a worker thread. Otherwise, NULL.
 */
extern MprWorker *mprGetCurrentWorker(MprCtx ctx);

#endif /* BLD_FEATURE_MULTITHREAD */
/**
 *  Deocde buffer using base-46 encoding.
 *  @param buffer Reference to a buffer containing that data to decode
 *  @param bufsize size of the buffer in bytes
 *  @param str String to decode
 *  @returns Zero if successful, otherwise returns MPR_ERR_WONT_FIT if the buffer is too small.
 * 
 */
extern int mprDecode64(char *buffer, int bufsize, cchar *str);

/**
 *  Encode buffer using base-46 encoding.
 *  @param buffer Reference to a buffer that will contain the encoded data.
 *  @param bufsize size of the buffer in bytes
 *  @param str String to encode
 *  @returns Zero if successful, otherwise returns MPR_ERR_WONT_FIT if the buffer is too small.
 */
extern void mprEncode64(char *buffer, int bufsize, cchar *str);

extern char *mprGetMD5Hash(MprCtx ctx, cchar *buf, int len, cchar *prefix);
extern int  mprCalcDigestNonce(MprCtx ctx, char **nonce, cchar *secret, cchar *etag, cchar *realm);
extern int  mprCalcDigest(MprCtx ctx, char **digest, cchar *userName, cchar *password, cchar *realm,
                cchar *uri, cchar *nonce, cchar *qop, cchar *nc, cchar *cnonce, cchar *method);

/**
 *  URI management
 *  @description The MPR provides routines for formatting and parsing URIs. Routines are also provided
 *      to escape dangerous characters for URIs as well as HTML content and shell commands.
 *  @stability Evolving
 *  @see MprHttp, mprFormatUri, mprEscapeCmd, mprEscapeHtml, mprUrlEncode, mprUrlDecode, mprValidateUrl
 *  @defgroup MprUri MprUri
 */
typedef struct MprUri {
    char        *originalUri;           /**< Original URI */
    char        *parsedUriBuf;          /**< Allocated storage for parsed uri */
    /*
     *  These are just pointers into the parsedUriBuf. None of these fields are Url decoded.
     */
    char        *scheme;                /**< URI scheme (http|https|...) */
    char        *host;                  /**< Url host name */
    int         port;                   /**< Port number */
    char        *url;                   /**< Url path name (without scheme, host, query or fragements) */
    char        *ext;                   /**< Document extension */
    char        *reference;             /**< Reference fragment */
    char        *query;                 /**< Query string */
    bool        secure;                 /**< Using https */
} MprUri;

/*
 *  Character escaping masks
 */
#define MPR_HTTP_ESCAPE_HTML            0x1
#define MPR_HTTP_ESCAPE_SHELL           0x2
#define MPR_HTTP_ESCAPE_URL             0x4

/**
 *  Parse a URI
 *  @description Parse a uri and return a tokenized MprUri structure.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param uri Uri string to parse
 *  @return A newly allocated MprUri structure. Caller must free using #mprFree.
 *  @ingroup MprUri
 */
extern MprUri *mprParseUri(MprCtx ctx, cchar *uri);

/**
 *  Format a URI
 *  @description Format a URI string using the input components.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param protocol Protocol string for the uri. Example: "http"
 *  @param host Host or IP address
 *  @param port TCP/IP port number
 *  @param path URL path
 *  @param query Additiona query parameters.
 *  @return A newly allocated uri string. Caller must free using #mprFree.
 *  @ingroup MprUri
 */
extern char *mprFormatUri(MprCtx ctx, cchar *protocol, cchar *host, int port, cchar *path, cchar *query);

/**
 *  Encode a string escaping typical command (shell) characters
 *  @description Encode a string escaping all dangerous characters that have meaning for the unix or MS-DOS command shells.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param cmd Command string to encode
 *  @param escChar Escape character to use when encoding the command.
 *  @return An allocated string containing the escaped command. Caller must free using #mprFree.
 *  @ingroup MprUri
 */
extern char *mprEscapeCmd(MprCtx ctx, cchar *cmd, int escChar);

/**
 *  Encode a string by escaping typical HTML characters
 *  @description Encode a string escaping all dangerous characters that have meaning in HTML documents
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param html HTML content to encode
 *  @return An allocated string containing the escaped HTML. Caller must free using #mprFree.
 *  @ingroup MprUri
 */
extern char *mprEscapeHtml(MprCtx ctx, cchar *html);

/**
 *  Get the mime type for an extension.
 *  This call will return the mime type from a limited internal set of mime types for the given path or extension.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param ext Path or extension to examine
 *  @returns Mime type. This is a static string. Caller must not free.
 */
extern cchar *mprLookupMimeType(MprCtx ctx, cchar *ext);

/**
 *  Encode a string by escaping URL characters
 *  @description Encode a string escaping all characters that have meaning for URLs.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param url URL to encode
 *  @return An allocated string containing the encoded URL. Caller must free using #mprFree.
 *  @ingroup MprUri
 */
extern char *mprUrlEncode(MprCtx ctx, cchar *url);

/**
 *  Decode a URL string by de-scaping URL characters
 *  @description Decode a string with www-encoded characters that have meaning for URLs.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param url URL to decode
 *  @return A reference to the buf argument.
 *  @ingroup MprUri
 */
extern char *mprUrlDecode(MprCtx ctx, cchar *url);

/**
 *  Validate a URL
 *  @description Validate and canonicalize a URL. This removes redundant "./" sequences and simplifies "../dir" 
 *      references. This operates in-situ and modifies the existing string.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param url Url string to validate
 *  @return A validated url.
 *  @ingroup MprUri
 */
extern char *mprValidateUrl(MprCtx ctx, char *url);

#if BLD_FEATURE_HTTP

#define MPR_HTTP_NAME                   "Embedthis-http/" BLD_VERSION

/*
 *  Standard HTTP/1.1 response codes. See url.c for the actual strings used for each code.
 */
#define MPR_HTTP_CODE_CONTINUE                  100
#define MPR_HTTP_CODE_OK                        200
#define MPR_HTTP_CODE_CREATED                   201
#define MPR_HTTP_CODE_ACCEPTED                  202
#define MPR_HTTP_CODE_NOT_AUTHORITATIVE         203
#define MPR_HTTP_CODE_NO_CONTENT                204
#define MPR_HTTP_CODE_RESET                     205
#define MPR_HTTP_CODE_PARTIAL                   206
#define MPR_HTTP_CODE_MOVED_PERMANENTLY         301
#define MPR_HTTP_CODE_MOVED_TEMPORARILY         302
#define MPR_HTTP_CODE_NOT_MODIFIED              304
#define MPR_HTTP_CODE_USE_PROXY                 305
#define MPR_HTTP_CODE_TEMPORARY_REDIRECT        307
#define MPR_HTTP_CODE_BAD_REQUEST               400
#define MPR_HTTP_CODE_UNAUTHORIZED              401
#define MPR_HTTP_CODE_PAYMENT_REQUIRED          402
#define MPR_HTTP_CODE_FORBIDDEN                 403
#define MPR_HTTP_CODE_NOT_FOUND                 404
#define MPR_HTTP_CODE_BAD_METHOD                405
#define MPR_HTTP_CODE_NOT_ACCEPTABLE            406
#define MPR_HTTP_CODE_REQUEST_TIME_OUT          408
#define MPR_HTTP_CODE_CONFLICT                  409
#define MPR_HTTP_CODE_GONE                      410
#define MPR_HTTP_CODE_LENGTH_REQUIRED           411
#define MPR_HTTP_CODE_REQUEST_TOO_LARGE         413
#define MPR_HTTP_CODE_REQUEST_URL_TOO_LARGE     414
#define MPR_HTTP_CODE_UNSUPPORTED_MEDIA_TYPE    415
#define MPR_HTTP_CODE_RANGE_NOT_SATISFIABLE     416
#define MPR_HTTP_CODE_EXPECTATION_FAILED        417
#define MPR_HTTP_CODE_INTERNAL_SERVER_ERROR     500
#define MPR_HTTP_CODE_NOT_IMPLEMENTED           501
#define MPR_HTTP_CODE_BAD_GATEWAY               502
#define MPR_HTTP_CODE_SERVICE_UNAVAILABLE       503
#define MPR_HTTP_CODE_GATEWAY_TIME_OUT          504
#define MPR_HTTP_CODE_BAD_VERSION               505
#define MPR_HTTP_CODE_INSUFFICIENT_STORAGE      507

/*
 *  Proprietary HTTP codes.
 */
#define MPR_HTTP_CODE_COMMS_ERROR               550
#define MPR_HTTP_CODE_CLIENT_ERROR              551

/*
 *  Overall HTTP service
 */
typedef struct MprHttpService {
    MprHashTable    *codes;                                 /* Http response code hash */
    MprList         *connections;                           /* Http connections */
    MprEvent        *timer;                                 /* Timeout event handle  */
    char            *secret;                                /* Random bytes to use in authentication */
    int             next;                                   /* Next sequence */
#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;                                 /* Mutli-thread sync */
#endif
} MprHttpService;

extern MprHttpService *mprCreateHttpService(MprCtx ctx);
extern int mprStartHttpService(MprHttpService *hs);
extern int mprStopHttpService(MprHttpService *hs);

/*
 *  Request states
 */
#define MPR_HTTP_STATE_BEGIN            1                   /* Ready for a new request */
#define MPR_HTTP_STATE_WAIT             2                   /* Waiting for the response */
#define MPR_HTTP_STATE_CONTENT          3                   /* Reading posted content */
#define MPR_HTTP_STATE_CHUNK            4                   /* Reading chunk specification (chunk length) */
#define MPR_HTTP_STATE_PROCESSING       5                   /* Reading posted content */
#define MPR_HTTP_STATE_COMPLETE         6                   /* Processing complete */

/*
 *  HTTP protocol versions
 */
#define MPR_HTTP_1_0                    0                   /* HTTP/1.0 */
#define MPR_HTTP_1_1                    1                   /* HTTP/1.1 */

/**
 *  Get the Http reponse code as a string
 *  @description Get the Http response code as a string.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @param code Http status code
 *  @return A reference to the response code string. Callers must not free this reference.
 *  @ingroup MprHttp
 */
extern cchar *mprGetHttpCodeString(MprCtx ctx, int code);

#endif /* BLD_FEATURE_HTTP */

#if BLD_FEATURE_HTTP_CLIENT
/*
 *  Callback flags
 */
#define MPR_HTTP_CALL_POST_DATA         1                   /* Time for caller to supply post data */
#define MPR_HTTP_CALL_RESPONSE_DATA     2                   /* Progressive reading of response data */
#define MPR_HTTP_CALL_COMPLETE          3                   /* Request is complete */

/**
 *  Http callback procedure
 *  Must not delete the http instance in the callback.
 *  @param http Http object created via #mprCreateHttp
 *  @param mask Event mask
 */
typedef void (*MprHttpProc)(void *arg, int mask);

/*
 *  mprHttpRequest flags
 */
#define MPR_HTTP_REQ_CHUNK_EMITTED      0x1     /**< Chunk boundary emitted */

/**
 *  HTTP Per-request structure
 */
typedef struct MprHttpRequest {
    struct MprHttp  *http;              /**< Reference to Http service object */
    char            *method;            /**< Request method GET, HEAD, POST, DELETE, OPTIONS, PUT, TRACE */
    MprUri          *uri;               /**< Request uri */
    MprHashTable    *headers;           /**< Headers keyword values */
    MprBuf          *outBuf;            /**< Request output buffer */
    char            *bodyData;          /**< Form post data */
    int             bodyLen;            /**< Length of bodyData */
    char            *formData;          /**< Form post data */
    int             formLen;            /**< Length of formData */
    int             sentCredentials;    /**< Credentials sent with request */
    int             flags;              /**< Request control flags */
    int             chunked;            /**< Enable chunked transfers. Set to -1 if not yet defined */
} MprHttpRequest;


/*
 *  Response flags
 */
#define MPR_HTTP_RESP_COMPLETE          0x4     /* Request complete */
#define MPR_HTTP_RESP_CHUNKED           0x8     /* Using HTTP/1.1 chunked for the response */
#define MPR_HTTP_RESP_BUFFER_SIZED      0x10    /* Input buffer has been resized */

/**
 *  HTTP Per-response structure
 */
typedef struct MprHttpResponse {
    struct MprHttp  *http;              /**< Reference to Http service object */
    MprBuf          *headerBuf;         /**< Header buffer */
    MprBuf          *chunkBuf;          /**< Holding buffer for the current chunk */
    MprBuf          *dataBuf;           /**< Current response data buffer */
    MprHashTable    *headers;           /**< Headers keyword values */
    int             length;             /**< Actual length of content received */
    int             contentLength;      /**< Content length header */
    int             contentRemaining;   /**< Remaining content data to read */
    int             chunkRemaining;     /**< Remaining content data to read in this chunk */
    int             code;               /**< Http response status code */
    char            *message;           /**< Http response status message */
    char            *protocol;          /**< Response protocol "HTTP/1.0" or "HTTP/1.1" */
    char            *location;          /**< Redirect location */
    char            *authAlgorithm;     /**< Authentication algorithm */
    char            *authStale;         /**< Stale handshake value */
    int             flags;              /**< Response control flags */
} MprHttpResponse;


/**
 *  Http per-connection structure. 
 *  @description The HTTP service provides a Http client with optional SSL capabilities. It supports 
 *      response chunking and ranged requests.
 *  @stability Prototype.
 *  @see mprAddHttpFormData mprAddHttpFormItem mprCreateHttp mprCreateHttpSecret mprDisconnectHttp mprEnableHttpUpload
 *      mprGetHttpChunked mprGetHttpCode mprGetHttpContentLength mprGetHttpDefaultHost mprGetHttpDefaultPort
 *      mprGetHttpError mprGetHttpFlags mprGetHttpHeader mprGetHttpHeaders mprGetHttpHeadersHash mprGetHttpMessage
 *      mprGetHttpState mprHttpRequest mprIsHttpComplete mprNeedHttpRetry mprReadHttp mprReadHttpString 
 *      mprResetHttpCredentials mprSetFormattedHttpHeader mprSetHttpBody mprSetHttpBufferSize mprSetHttpCallback 
 *      mprSetHttpChunked mprSetHttpContentLength mprSetHttpCredentials mprSetHttpDefaultHost mprSetHttpDefaultPort 
 *      mprSetHttpFollowRedirects mprSetHttpHeader mprSetHttpKeepAlive mprSetHttpProtocol mprSetHttpProxy 
 *      mprSetHttpRetries mprSetHttpTimeout mprWaitForHttp mprWaitForHttpResponse mprWriteHttp mprWriteHttpUploadData
 *      mprFinalizeHttpWriting
 *  @defgroup MprHttp MprHttp
 */
typedef struct MprHttp {
    MprHttpService  *service;           /**< Convenience reference to the http service */
    MprHttpRequest  *request;           /**< Request object */
    MprHttpResponse *response;          /**< Response object */
    MprSocket       *sock;              /**< Underlying socket handle */
    int             state;              /**< Connection state  */
    char            *currentHost;       /**< Current connection host */
    int             currentPort;        /**< Current connection port */
    char            *protocol;          /**< HTTP protocol to use */
    char            *defaultHost;       /**< Default target host (if unspecified) */
    char            *proxyHost;         /**< Proxy host to connect via */
    int             defaultPort;        /**< Default target port (if unspecified) */
    int             proxyPort;          /**< Proxy port to connect via */
    MprTime         timestamp;          /**< Timeout timestamp for last I/O  */
    int             timeoutPeriod;      /**< Timeout value */
    int             timedout;           /**< Request has timedout */
    int             retries;            /**< Max number of retry attempts */
    MprHttpProc     callback;           /**< Response callback structure  */
    void            *callbackArg;       /**< Argument to callback  */
    int             callbackMask;       /**< Events of interest to the callback */
    /*
     *  Auth details
     */
    char            *authCnonce;        /**< Digest authentication cnonce value */
    char            *authDomain;        /**< Authentication domain */
    char            *authNonce;         /**< Nonce value used in digest authentication */
    int             authNc;             /**< Digest authentication nc value */
    char            *authOpaque;        /**< Opaque value used to calculate digest session */
    char            *authRealm;         /**< Authentication realm */
    char            *authQop;           /**< Digest authentication qop value */
    char            *authType;          /**< Basic or Digest */
    char            *password;          /**< As the name says */
    char            *user;              /**< User account name */
    char            *error;             /**< Error message if failure  */
    char            *userHeaders;       /**< User headers */
    char            *boundary;          /**< Multipart mime boundary pattern */
    bool            useKeepAlive;       /**< Use connection keep-alive for all connections */
    bool            keepAlive;          /**< Use keep-alive for this connection */
    bool            followRedirects;    /**< Follow redirects */
    int             bufsize;            /**< Initial buffer size */
    int             bufmax;             /**< Maximum buffer size. -1 is no max */
    int             secure;             /**< Request uses SSL */
    int             protocolVersion;    /**< HTTP protocol version to request */
#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;             /**< Mutli-thread sync */
#endif
} MprHttp;


/**
 *  Add Http request form content
 *  @description Define request content that is formatted using www-form-urlencoded formatting. This is the 
 *      traditional way to post form data. For example: "name=John+Smith&City=Seattle". Multiple calls may be 
 *      made to this routine and the form data will be catenated.
 *  @param http Http object created via #mprCreateHttp
 *  @param form String containing the encoded form data.
 *  @param len Length of the form data
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprAddHttpFormData(MprHttp *http, cchar *form, int len);

/**
 *  Add a form item
 *  @description Add a key/value pair to the request form data.
 *  @param http Http object created via #mprCreateHttp
 *  @param key Name of the form entity to define.
 *  @param value Value of the form entity.
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprAddHttpFormItem(MprHttp *http, cchar *key, cchar *value);

/**
 *  Create a Http connection object
 *  @description Create a new http connection object. This creates an object that can be initialized and then
 *      used with mprHttpRequest
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @return A newly allocated MprHttp structure. Caller must free using #mprFree.
 *  @ingroup MprHttp
 */
extern MprHttp *mprCreateHttp(MprCtx ctx);

/**
 *  Create a Http secret
 *  @description Create http secret data that is used to seed SSL based communications.
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprCreateHttpSecret(MprCtx ctx);

/**
 *  Disconnect a Http connection
 *  @description Disconned any keep-alive connection associated with this http object.
 *  @param http Http object created via #mprCreateHttp
 *  @ingroup MprHttp
 */
extern void mprDisconnectHttp(MprHttp *http);

/**
 *  Enable Multipart-Mime file upload for this request. This will define a "Content-Type: multipart/form-data..."
 *  header and will create a mime content boundary for use to delimit the various upload content files and fields.
 *  @param http Http object created via #mprCreateHttp
 *  @ingroup MprHttp
 */
extern void mprEnableHttpUpload(MprHttp *http);

/**
 *  Finalize writing Http data
 *  @description Finalize writing Http data by writing the final chunk trailer if required. If using chunked transfers, 
 *  a null chunk trailer is required to signify the end of write data. 
 *  @param http Http object created via #mprCreateHttp
 *  @returns True if successful, otherwise a negative MPR error code.
 *  @ingroup MprHttp
 */
extern int mprFinalizeHttpWriting(MprHttp *http);

/**
 *  Return the http flags
 *  @description Get the current http flags. The valid flags are:
 *      @li MPR_HTTP_DONT_BLOCK  - For non-blocking connections
 *  @param http Http object created via #mprCreateHttp
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprGetHttpFlags(MprHttp *http);

/**
 *  Get the http state
 *  @description Get the current http state. The valid state are:
 *      @li  MPR_HTTP_STATE_BEGIN            - Ready for a new request
 *      @li  MPR_HTTP_STATE_WAIT             - Waiting for the response
 *      @li  MPR_HTTP_STATE_CONTENT          - Reading posted content
 *      @li  MPR_HTTP_STATE_CHUNK            - Reading chunk length
 *      @li  MPR_HTTP_STATE_PROCESSING       - Reading posted content
 *      @li  MPR_HTTP_STATE_COMPLETE         - Processing complete
 *  @param http Http object created via #mprCreateHttp
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprGetHttpState(MprHttp *http);

/**
 *  Get the default host
 *  @description A default host can be defined which will be used for URIs that omit a host specification.
 *  @param http Http object created via #mprCreateHttp
 *  @return A reference to the default host string. Callers must not free this reference.
 *  @ingroup MprHttp
 */
extern cchar *mprGetHttpDefaultHost(MprHttp *http);

/**
 *  Get the default port
 *  @description A default port can be defined which will be used for URIs that omit a host:port specification.
 *  @param http Http object created via #mprCreateHttp
 *  @return The default port number
 *  @ingroup MprHttp
 */
extern int mprGetHttpDefaultPort(MprHttp *http);

/**
 *  Get the Http response code
 *  @description Get the http response code for the last request.
 *  @param http Http object created via #mprCreateHttp
 *  @return An integer Http response code. Typically 200 is success.
 *  @ingroup MprHttp
 */
extern int mprGetHttpCode(MprHttp *http);

/**
 *  Get the Http response message
 *  @description Get the Http response message supplied on the first line of the Http response.
 *  @param http Http object created via #mprCreateHttp
 *  @return A reference to the response message string. Callers must not free this reference.
 *  @ingroup MprHttp
 */
extern cchar *mprGetHttpMessage(MprHttp *http);

/**
 *  Get the response content length
 *  @description Get the length of the response content (if any)
 *  @param http Http object created via #mprCreateHttp
 *  @return A count of the response content data in bytes.
 *  @ingroup MprHttp
 */
extern int mprGetHttpContentLength(MprHttp *http);

/**
 *  Get the Http error message
 *  @description Get a Http error message. Error messages may be generated for internal or client side errors.
 *  @param http Http object created via #mprCreateHttp
 *  @return A error string. The caller must not free this reference.
 *  @ingroup MprHttp
 */
extern cchar *mprGetHttpError(MprHttp *http);

/**
 *  Get a http response header.
 *  @description Get a http response header for a given header key.
 *  @param http Http object created via #mprCreateHttp
 *  @param key Name of the header to retrieve. This should be a lower case header name. For example: "Connection"
 *  @return Value associated with the header key or null if the key did not exist in the response.
 *  @ingroup MprHttp
 */
extern cchar *mprGetHttpHeader(MprHttp *http, cchar *key);

/**
 *  Get all the http response headers.
 *  @description Get all the response headers. The returned string formats all the headers in the form:
 *      key: value\\nkey2: value2\\n...
 *  @param http Http object created via #mprCreateHttp
 *  @return String containing all the headers. The caller must free this returned string.
 *  @ingroup MprHttp
 */
extern char *mprGetHttpHeaders(MprHttp *http);

/**
 *  Get the hash table of response Http headers
 *  @description Get the internal hash table of response headers
 *  @param http Http object created via #mprCreateHttp
 *  @return Hash table. See MprHash for how to access the hash table.
 *  @ingroup MprHttp
 */
extern MprHashTable *mprGetHttpHeadersHash(MprHttp *http);

/**
 *  Issue a new Http request and wait for completion.
 *  @description Start a new Http request on the http object and wait for it to complete. This routine blocks.
 *      Use mprStartHttpRequest() if you require non-blocking behavior.
 *  @param http Http object created via #mprCreateHttp
 *  @param method Http method to use. Valid methods include: "GET", "POST", "PUT", "DELETE", "OPTIONS" and "TRACE" 
 *  @param uri URI to fetch
 *  @return The HTTP response code if the request was successfully sent to the server and a response was received. Otherwise 
 *      a negative MPR error code is returned.
 *  @ingroup MprHttp
 */
extern int mprHttpRequest(MprHttp *http, cchar *method, cchar *uri);

/**
 *  Test if the Http request is complete.
 *  @param http Http object created via #mprCreateHttp
 *  @return true if the request is complete
 *  @ingroup MprHttp
 */
extern bool mprIsHttpComplete(MprHttp *http);

/**
 *  Test if the request needs a transparent retry to implement authentication or redirection. If authentication is required,
 *      a request must first be tried once to receive some authentication key information that must be resubmitted to gain
 *      access.
 *  @param http Http object created via #mprCreateHttp
 *  @param url Reference to a string to receive a redirection URL. Set to NULL if not redirection is required.
 *  @return true if the request needs to be retried.
 */
extern bool mprNeedHttpRetry(MprHttp *http, char **url);

/**
 *  Read Http response data. This will read available response data. If no callback has been defined, this call will block
 *      until some data is available to read.
 *  @param http Http object created via #mprCreateHttp
 *  @param buffer Buffer to receive read data
 *  @param size Size of buffer. 
 *  @ingroup MprHttp
 */
extern int mprReadHttp(MprHttp *http, char *buffer, int size);

/**
 *  Read response data as a string. This will read response data and return a string that the caller should free. If 
 *  no callback has been defined, this call will block until some data is available to read. If there is a callback 
 *  defined, the routine will return with whatever data is currently available.
 *  @param http Http object created via #mprCreateHttp
 *  @ingroup MprHttp
 */
extern char *mprReadHttpString(MprHttp *http);

/**
 *  Reset Http credentials.
 *  @description Reset any previously defined Http credentials (username/password) on this http object.
 *  @param http Http object created via #mprCreateHttp
 *  @ingroup MprHttp
 */
extern void mprResetHttpCredentials(MprHttp *http);

/**
 *  Set the http request body content
 *  @description Define content to be sent with the Http request. 
 *  @param http Http object created via #mprCreateHttp
 *  @param body Pointer to the body content.
 *  @param len Length of the body content
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprSetHttpBody(MprHttp *http, cchar *body, int len);

/**
 *  Define a Http callback.
 *  @description If define, the http callabck will be invoked as response data or state is received.
 *  @param http Http object created via #mprCreateHttp
 *  @param fn Callback function. 
 *  @param arg Data argument to provide to the callback function.
 *  @param mask Mask of events of interest. Use MPR_READABLE | MPR_WRITABLE.
 *  @ingroup MprHttp
 */
extern void mprSetHttpCallback(MprHttp *http, MprHttpProc fn, void *arg, int mask);

/**
 *  Return whether transfer chunked encoding will be used on this request
 *  @param http Http object created via #mprCreateHttp
 *  @returns true if chunk encoding will be used
 *  @ingroup MprHttp
 */ 
extern int mprGetHttpChunked(MprHttp *http);

/**
 *  Set whether transfer chunked encoding will be used on this request. If enabled, a "Transfer-Encoding: Chunked" will
 *  be added to the request headers and all write data will be delimited by chunk specifications.
 *  @param http Http object created via #mprCreateHttp
 *  @param enable Set to true to enable transfer chunk encoding.
 *  @returns The previous chunk enable setting.
 */ 
extern int mprSetHttpChunked(MprHttp *http, int enable);

/**
 *  Define a request content length. This will define a "Content-Length: NNN" request header.
 *  @param http Http object created via #mprCreateHttp
 *  @param length Numeric value for the content length header.
 *  @ingroup MprHttp
 */
extern void mprSetHttpContentLength(MprHttp *http, int length);

/**
 *  Set the Http credentials
 *  @description Define a username and password to use with Http authentication for sites that require it.
 *  @param http Http object created via #mprCreateHttp
 *  @param username String username
 *  @param password Un-encrypted password string
 *  @ingroup MprHttp
 */
extern void mprSetHttpCredentials(MprHttp *http, cchar *username, cchar *password);

/**
 *  Set the Http buffer size.
 *  @description Define an initial and maximum limit for the response content buffering. By default, the 
 *      buffer will grow to accomodate all response data.
 *  @param http Http object created via #mprCreateHttp
 *  @param initialSize Starting size of the response content buffer
 *  @param maxSize Maximum size of the response content buffer.
 *  @ingroup MprHttp
 */
extern void mprSetHttpBufferSize(MprHttp *http, int initialSize, int maxSize);

/**
 *  Define a default host
 *  @description Define a default host to use for connections if the URI does not specify a host
 *  @param http Http object created via #mprCreateHttp
 *  @param host Host or IP address
 *  @ingroup MprHttp
 */
extern void mprSetHttpDefaultHost(MprHttp *http, cchar *host);

/**
 *  Define a default port
 *  @description Define a default port to use for connections if the URI does not define a port
 *  @param http Http object created via #mprCreateHttp
 *  @param port Integer port number
 *  @ingroup MprHttp
 */
extern void mprSetHttpDefaultPort(MprHttp *http, int port);

/**
 *  Follow redirctions
 *  @description Enabling follow redirects enables the Http service to transparently follow 301 and 302 redirections
 *      and fetch the redirected URI.
 *  @param http Http object created via #mprCreateHttp
 *  @param follow Set to true to enable transparent redirections
 *  @ingroup MprHttp
 */
extern void mprSetHttpFollowRedirects(MprHttp *http, bool follow);

/**
 *  Add a request header using a format string
 *  @description Add a Http header to send with the request
 *  @param http Http object created via #mprCreateHttp
 *  @param overwrite Set to true to overwrite any previous header of the same key. Set to false to allow duplicate
 *      headers of the same key value.
 *  @param key Http header key
 *  @param fmt Format string
 *  @param ... Values for the format string
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprSetFormattedHttpHeader(MprHttp *http, bool overwrite, cchar *key, cchar *fmt, ...);

/**
 *  Add a request header
 *  @description Add a Http header to send with the request
 *  @param http Http object created via #mprCreateHttp
 *  @param overwrite Set to true to overwrite any previous header of the same key. Set to false to allow duplicate
 *      headers of the same key value.
 *  @param key Http header key
 *  @param value Http header value
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern int mprSetHttpHeader(MprHttp *http, bool overwrite, cchar *key, cchar *value);

/**
 *  Control Http Keep-Alive
 *  @description Http keep alive means that the TCP/IP connection is preserved accross multiple requests. This
 *      typically means much higher performance and better response. Http keep alive is enabled by default 
 *      for Http/1.1 (the default). Disable keep alive when talking
 *      to old, broken HTTP servers.
 *  @param http Http object created via #mprCreateHttp
 *  @param on Set to true to enable http keep-alive
 *  @return Zero if successful, otherwise a negative MPR error code
 *  @ingroup MprHttp
 */
extern void mprSetHttpKeepAlive(MprHttp *http, bool on);

/**
 *  Set the Http protocol variant.
 *  @description Set the Http protocol variant to use. 
 *  @param http Http object created via #mprCreateHttp
 *  @param protocol  String representing the protocol variant. Valid values are:
 *      @li "HTTP/1.0"
 *      @li "HTTP/1.1"
 *  Use HTTP/1.1 wherever possible.
 *  @ingroup MprHttp
 */
extern void mprSetHttpProtocol(MprHttp *http, cchar *protocol);

/**
 *  Define a Http proxy host
 *  @description Define a http proxy host to communicate via when accessing the net.
 *  @param http Http object created via #mprCreateHttp
 *  @param host Proxy host name or IP address
 *  @param port Proxy host port number.
 *  @ingroup MprHttp
 */
extern void mprSetHttpProxy(MprHttp *http, cchar *host, int port);

/**
 *  Set the Http retry count
 *  @description Define the number of retries before failing a request. It is normative for network errors
 *      to require that requests be sometimes retried. The default retries is set to (2).
 *  @param http Http object created via #mprCreateHttp
 *  @param retries Count of retries
 *  @ingroup MprHttp
 */
extern void mprSetHttpRetries(MprHttp *http, int retries);

/**
 *  Set the Http inactivity timeout
 *  @description Define an inactivity timeout after which the Http connection will be closed. 
 *  @param http Http object created via #mprCreateHttp
 *  @param timeout Timeout in milliseconds
 *  @returns the original timeout value
 *  @ingroup MprHttp
 */
extern int mprSetHttpTimeout(MprHttp *http, int timeout);

/**
 *  Start a new Http request.
 *  @description Start a new Http request on the http object and return. This routine does not block.
 *      Use mprHttpRequest() if you require blocking behavior. After starting the request, you can use mprWaitForHttp()
 *      or mprWaitForHttpResponse() to wait for the request to achieve a certain state or to complete.
 *  @param http Http object created via #mprCreateHttp
 *  @param method Http method to use. Valid methods include: "GET", "POST", "PUT", "DELETE", "OPTIONS" and "TRACE" 
 *  @param uri URI to fetch
 *  @return Zero if the request was successfully sent to the server. Otherwise a negative MPR error code is returned.
 *  @ingroup MprHttp
 */
extern int mprStartHttpRequest(MprHttp *http, cchar *method, cchar *uri);

/**
 *  Wait for http response data to drive the Http request/response to the requested state
 *  @param http Http object created via #mprCreateHttp
 *  @param state MPR_HTTP_STATE_XXX to wait for.
 *  @param timeout Timeout in milliseconds to wait 
 *  @ingroup MprHttp
 */
extern int mprWaitForHttp(MprHttp *http, int state, int timeout);

/**
 *  Wait for a http response to the request. This will wait until the response headers have been fully received.
 *  @param http Http object created via #mprCreateHttp
 *  @param timeout Timeout in milliseconds to wait 
 *  @ingroup MprHttp
 */
extern int mprWaitForHttpResponse(MprHttp *http, int timeout);

/**
 *  Write Http request body data
 *  @description Write request content data. This will use transfer chunk encoding if mprSetHttpChunked has been
 *      called. This routine will block until all the buffer is written. If a callback is defined, it may return "short"
 *      with less data written than requested. The caller can then retry when a Write callback event is received.
 *  @param http Http object created via #mprCreateHttp
 *  @param buf Pointer to buffer containing the data to write
 *  @param len Length of data to write
 *  @return Number of bytes successfully written.
 *  @ingroup MprHttp
 */
extern int mprWriteHttp(MprHttp *http, cchar *buf, int len);

/**
 *  Write Http upload body data
 *  @description Write files and form fields as request body data. This will use transfer chunk encoding. This routine 
 *      will block until all the buffer is written even if a callback is defined.
 *  @param http Http object created via #mprCreateHttp
 *  @param fileData List of string file names to upload
 *  @param formData List of strings containing "key=value" pairs. The form data should be already www-urlencoded.
 *  @return Number of bytes successfully written.
 *  @ingroup MprHttp
 */
extern int mprWriteHttpUploadData(MprHttp *http, MprList *formData, MprList *fileData);

#endif /* BLD_FEATURE_HTTP_CLIENT */

/* ********************************* MprCmd ************************************/
#if BLD_FEATURE_CMD

/*
 *  Child status structure. Designed to be async-thread safe.
 */
typedef struct MprCmdChild {
    ulong           pid;                /*  Process ID */
    int             exitStatus;         /*  Exit status */
} MprCmdChild;

#define MPR_CMD_EOF_COUNT       2
#define MPR_CMD_VXWORKS_EOF     "_ _EOF_ _"
#define MPR_CMD_VXWORKS_EOF_LEN 9

/*
 *  Channels for clientFd and serverFd
 */
#define MPR_CMD_STDIN           0       /* Stdout for the client side */
#define MPR_CMD_STDOUT          1       /* Stdin for the client side */
#define MPR_CMD_STDERR          2       /* Stderr for the client side */
#define MPR_CMD_MAX_PIPE        3

/*
 *  Cmd procs must return the number of bytes read or -1 for errors.
 */
struct MprCmd;
typedef int    (*MprCmdProc)(struct MprCmd *cmd, int channel, void *data);

/*
 *  Flags
 */
#define MPR_CMD_NEW_SESSION     0x1     /* Create a new session on unix */
#define MPR_CMD_SHOW            0x2     /* Show the window of the created process on windows */
#define MPR_CMD_DETACH          0x4     /* Detach the child process and don't wait */
#define MPR_CMD_IN              0x1000  /* Connect to stdin */
#define MPR_CMD_OUT             0x2000  /* Capture stdout */
#define MPR_CMD_ERR             0x4000  /* Capture stdout */

typedef struct MprCmdFile {
    char            *name;
    int             fd;
    int             clientFd;
#if BLD_WIN_LIKE
    HANDLE          handle;
#endif
} MprCmdFile;

/**
 *  Command execution Service
 *  @description The MprCmd service enables execution of local commands. It uses three full-duplex pipes to communicate
 *      read, write and error data with the command. 
 *  @stability Evolving.
 *  @see mprGetCmdBuf mprCreateCmd mprIsCmdRunning mprStartCmd mprGetCmdExitStatus mprGetCmdFd mprMakeCmdIO 
 *      mprReadCmdPipe mprReapCmd mprRunCmd mprRunCmdV mprWaitForCmd mprWriteCmdPipe mprCloseCmdFd 
 *      mprDisableCmdEvents mprDisconnectCmd mprEnableCmdEvents mprPollCmdPipes mprSetCmdCallback mprSetCmdDir 
 *      mprSetCmdEnv mprStopCmd
 *  @defgroup MprCmd MprCmd
 */
typedef struct MprCmd {
    char            *program;           /* Program path name */
    char            **argv;             /* List of args. Null terminated */
    char            **env;              /* List of environment variables. Null terminated */
    char            *dir;               /* Current working dir for the process */
    int             argc;               /* Count of args in argv */
    int             status;             /* Command exit status */
    int             flags;              /* Control flags (userFlags not here) */
    int             eofCount;           /* Count of end-of-files */
    int             requiredEof;        /* Number of EOFs required for an exit */
    MprTime         timestamp;          /**< Timeout timestamp for last I/O  */
    int             timeoutPeriod;      /**< Timeout value */
    int             timedout;           /**< Request has timedout */
    MprCond         *completeCond;      /* Completion condition */
    MprCmdFile      files[MPR_CMD_MAX_PIPE]; /* Stdin, stdout for the command */
    MprWaitHandler  *handlers[MPR_CMD_MAX_PIPE];
    MprCmdProc      callback;           /* Handler for client output and completion */
    void            *callbackData;
    MprBuf          *stdoutBuf;         /* Standard output from the client */
    MprBuf          *stderrBuf;         /* Standard error output from the client */
    MprTime         lastActivity;       /* Time of last I/O */

    int             pid;                /* Process ID of the created process */
    void            *userData;          /* User data storage */
    int             userFlags;          /* User flags storage */

#if BLD_WIN_LIKE
    HANDLE          thread;             /* Handle of the primary thread for the created process */
    HANDLE          process;            /* Process handle for the created process */
    char            *command;           /* Windows command line */          
#endif

#if VXWORKS
    /*
     *  Don't use MprCond so we can build single-threaded and still use MprCmd
     */
    SEM_ID          startCond;          /* Synchronization semaphore for task start */
    SEM_ID          exitCond;           /* Synchronization semaphore for task exit */
#endif
#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;             /* Multithread sync */
    MprThread       *parent;            /* Parent process thread */
#endif
} MprCmd;


/**
 *  Close the command channel
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to close. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @ingroup MprCmd
 */
extern void mprCloseCmdFd(MprCmd *cmd, int channel);

/**
 *  Create a new Command object 
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @returns A newly allocated MprCmd object. Use mprFree to close and free.
 *  @ingroup MprCmd
 */
extern MprCmd *mprCreateCmd(MprCtx ctx);

/**
 *  Disconnect a command its underlying I/O channels. This is used to prevent further I/O wait events while
 *  still preserving the MprCmd object.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @ingroup MprCmd
 */
extern void mprDisconnectCmd(MprCmd *cmd);

/**
 *  Disable command I/O events. This disables events on a given channel.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to close. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @ingroup MprCmd
 */
extern void mprDisableCmdEvents(MprCmd *cmd, int channel);

/**
 *  Enable command I/O events. This enables events on a given channel.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to close. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @ingroup MprCmd
 */
extern void mprEnableCmdEvents(MprCmd *cmd, int channel);

/**
 *  Get the command exit status
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param status Reference to an integer to receive the command exit status. This is typically zero for success, but this
 *      is platform specific.
 *  @ingroup MprCmd
 */
extern int mprGetCmdExitStatus(MprCmd *cmd, int *status);

/**
 *  Get the underlying file descriptor for an I/O channel
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to close. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @return The file descriptor 
 *  @ingroup MprCmd
 */
extern int mprGetCmdFd(MprCmd *cmd, int channel);

/**
 *  Get the underlying buffer for a channel
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to close. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @return A reference to the MprBuf buffer structure
 *  @ingroup MprCmd
 */
extern MprBuf *mprGetCmdBuf(MprCmd *cmd, int channel);

/**
 *  Test if the command is still running.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @return True if the command is still running
 *  @ingroup MprCmd
 */
extern bool mprIsCmdRunning(MprCmd *cmd);

/**
 *  Make the I/O channels to send and receive data to and from the command.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int mprMakeCmdIO(MprCmd *cmd);

/**
 *  Make the I/O channels to send and receive data to and from the command.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to read from. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @param buf Buffer to read into
 *  @param bufsize Size of buffer
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int mprReadCmdPipe(MprCmd *cmd, int channel, char *buf, int bufsize);

/**
 *  Reap the command. This waits for and collect the command exit status. 
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param timeout Time in milliseconds to wait for the command to complete and exit.
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int mprReapCmd(MprCmd *cmd, int timeout);

/**
 *  Run a command using a string command line. This starts the command via mprStartCmd() and waits for its completion.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param command Command line to run
 *  @param out Reference to a string to receive the stdout from the command. Caller must free.
 *  @param err Reference to a string to receive the stderr from the command. Caller must free.
 *  @param flags Flags to modify execution. Valid flags are:
 *      MPR_CMD_NEW_SESSION     Create a new session on Unix
 *      MPR_CMD_SHOW            Show the commands window on Windows
 *      MPR_CMD_IN              Connect to stdin
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int mprRunCmd(MprCmd *cmd, cchar *command, char **out, char **err, int flags);

/**
 *  Run a command using an argv[] array of arguments. This invokes mprStartCmd() and waits for its completion.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param argc Count of arguments in argv
 *  @param argv Command arguments array
 *  @param out Reference to a string to receive the stdout from the command. Caller must free.
 *  @param err Reference to a string to receive the stderr from the command. Caller must free.
 *  @param flags Flags to modify execution. Valid flags are:
 *      MPR_CMD_NEW_SESSION     Create a new session on Unix
 *      MPR_CMD_SHOW            Show the commands window on Windows
 *      MPR_CMD_IN              Connect to stdin
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int mprRunCmdV(MprCmd *cmd, int argc, char **argv, char **out, char **err, int flags);

/**
 *  Define a callback to be invoked to receive response data from the command.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param callback Function of the signature MprCmdProc which will be invoked for receive notification
 *      for data from the commands stdout and stderr channels. MprCmdProc has the signature:
 *      int callback(MprCmd *cmd, int channel, void *data) {}
 *  @param data User defined data to be passed to the callback.
 *  @ingroup MprCmd
 */
extern void mprSetCmdCallback(MprCmd *cmd, MprCmdProc callback, void *data);

/**
 *  Set the home directory for the command
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param dir String directory path name.
 *  @ingroup MprCmd
 */
extern void mprSetCmdDir(MprCmd *cmd, cchar *dir);

/**
 *  Set the command environment
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param env Array of environment strings. Each environment string should be of the form: "KEY=VALUE". The array
 *      must be null terminated.
 *  @ingroup MprCmd
 */
extern void mprSetCmdEnv(MprCmd *cmd, cchar **env);

/**
 *  Start the command. This starts the command but does not wait for its completion. Once started, mprWriteCmdPipe
 *  can be used to write to the command and response data can be received via mprReadCmdPipe.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param argc Count of arguments in argv
 *  @param argv Command arguments array
 *  @param envp Array of environment strings. Each environment string should be of the form: "KEY=VALUE". The array
 *      must be null terminated.
 *  @param flags Flags to modify execution. Valid flags are:
 *      MPR_CMD_NEW_SESSION     Create a new session on Unix
 *      MPR_CMD_SHOW            Show the commands window on Windows
 *      MPR_CMD_IN              Connect to stdin
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int  mprStartCmd(MprCmd *cmd, int argc, char **argv, char **envp, int flags);

/**
 *  Stop the command. The command is immediately killed.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @ingroup MprCmd
 */
extern void mprStopCmd(MprCmd *cmd);

/**
 *  Wait for the command to complete.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param timeout Time in milliseconds to wait for the command to complete and exit.
 *  @return Zero if successful. Otherwise a negative MPR error code.
 *  @ingroup MprCmd
 */
extern int mprWaitForCmd(MprCmd *cmd, int timeout);

/**
 *  Poll for I/O on the command pipes. This is only used on windows which can't adequately detect EOF on a named pipe.
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param timeout Time in milliseconds to wait for the command to complete and exit.
 *  @ingroup MprCmd
 */
extern void mprPollCmdPipes(MprCmd *cmd, int timeout);

/**
 *  Write data to an I/O channel
 *  @param cmd MprCmd object created via mprCreateCmd
 *  @param channel Channel number to read from. Should be either MPR_CMD_STDIN, MPR_CMD_STDOUT or MPR_CMD_STDERR.
 *  @param buf Buffer to read into
 *  @param bufsize Size of buffer
 *  @ingroup MprCmd
 */
extern int mprWriteCmdPipe(MprCmd *cmd, int channel, char *buf, int bufsize);

#endif /* BLD_FEATURE_CMD */
/* *********************************** Mpr ************************************/
/*
 *  Mpr flags
 */
#define MPR_EXITING                 0x1     /* App is exiting */
#define MPR_STOPPED                 0x2     /* Mpr services stopped */
#define MPR_STARTED                 0x4     /* Mpr services started */
#define MPR_SSL_PROVIDER_LOADED     0x8     /* SSL provider loaded */

/**
 *  Primary MPR application control structure
 *  @description The Mpr structure stores critical application state information and is the root memory allocation
 *      context block. It is used as the MprCtx context for other memory allocations and is thus
 *      the ultimate parent of all allocated memory.
 *  @stability Evolving.
 *  @see mprGetApp, mprCreateEx, mprIsExiting, mprSignalExit, mprTerminate, mprGetKeyValue, mprRemoveKeyValue,
 *      mprSetDebugMode, mprGetErrorMsg, mprGetOsError, mprGetError, mprBreakpoint
 *  @defgroup Mpr Mpr
 */
typedef struct Mpr {
    MprHeap         heap;                   /**< Top level memory pool */
    MprHeap         pageHeap;               /**< Heap for arenas and slabs. Always page oriented */
    MprAlloc        alloc;                  /**< Memory allocation statistics */

    bool            debugMode;              /**< Run in debug mode (no timers) */
    int             logLevel;               /**< Log trace level */
    MprLogHandler   logHandler;             /**< Current log handler callback */
    void            *logHandlerData;        /**< Handle data for log handler */
    MprHashTable    *timeTokens;            /**< Date/Time parsing tokens */
    MprHashTable    *mimeTypes;             /**< Date/Time parsing tokens */
    char            *name;                  /**< Product name */
    char            *title;                 /**< Product title */
    char            *version;               /**< Product version */
    int             argc;                   /**< Count of command line args */
    char            **argv;                 /**< Application command line args */
    char            *domainName;            /**< Domain portion */
    char            *hostName;              /**< Host name (fully qualified name) */
    char            *ipAddr;                /**< Public IP Address */
    char            *serverName;            /**< Server name portion (no domain) */
    char            *appPath;               /**< Path name of application executable */
    char            *appDir;                /**< Path of directory containing app executable */
    int             flags;                  /**< Processing state */
    int             timezone;               /**< Minutes west of Greenwich without DST */
    int             hasDedicatedService;    /**< Running a dedicated events thread */
    int             multiThread;            /**< Is running multi-threaded */

    /*
     *  Service pointers
     */
    struct MprFileSystem    *fileSystem;    /**< File system service object */
    struct MprOsService     *osService;     /**< O/S service object */

    struct MprDispatcher    *dispatcher;    /**< Event service object */
    struct MprWorkerService *workerService; /**< Worker service object */
    struct MprWaitService   *waitService;   /**< IO Waiting service object */
    struct MprSocketService *socketService; /**< Socket service object */
#if BLD_FEATURE_HTTP
    struct MprHttpService   *httpService;   /**< HTTP service object */
#endif
#if BLD_FEATURE_CMD
    struct MprCmdService    *cmdService;    /**< Command service object */
#endif

    struct MprModuleService *moduleService; /**< Module service object */
    void            *ejsService;            /**< Ejscript service */

#if BLD_FEATURE_MULTITHREAD
    struct MprThreadService *threadService; /**< Thread service object */
    MprOsThread     serviceThread;          /**< Service OS */
    MprOsThread     mainOsThread;           /**< Main OS thread ID */

    MprMutex        *mutex;                 /**< Thread synchronization */
    MprSpin         *spin;                  /**< Quick thread synchronization */
#endif

#if BLD_WIN_LIKE
    long            appInstance;            /**< Application instance (windows) */
#endif
#if BREW
    uint            classId;                /**< Brew class ID */
    IShell          *shell;                 /**< Brew shell object */
    IDisplay        *display;               /**< Brew display object */
    ITAPI           *tapi;                  /**< TAPI object */
    int             displayHeight;          /**< Display height */
    int             displayWidth;           /**< Display width */
    char            *args;                  /**< Command line args */
#endif
} Mpr;


#if BLD_UNIX_LIKE
#define BLD_HAS_GLOBAL_MPR 1
#else
#define BLD_HAS_GLOBAL_MPR 0
#endif

#if DOXYGEN || !BLD_HAS_GLOBAL_MPR || BLD_WIN_LIKE
/**
 *  Return the MPR control instance.
 *  @description Return the MPR singleton control object. 
 *  @param ctx Any memory allocation context created by MprAlloc
 *  @return Returns the MPR control object.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern struct Mpr *mprGetMpr(MprCtx ctx);
#else

extern Mpr  *_globalMpr;                /* Mpr singleton */
#define mprGetMpr(ctx) _globalMpr
#endif

/**
 *  Create an instance of the MPR.
 *  @description Initializes the MPR and creates an Mpr control object. The Mpr Object manages Mpr facilities 
 *      and is the top level memory context. It may be used wherever a MprCtx parameter is required. This 
 *      function must be called prior to calling any other Mpr API.
 *  @param argc Count of command line args
 *  @param argv Command line arguments for the application. Arguments may be passed into the Mpr for retrieval
 *      by the unit test framework.
 *  @param cback Memory allocation failure notification callback.
 *  @return Returns a pointer to the Mpr object. 
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern Mpr *mprCreate(int argc, char **argv, MprAllocNotifier cback);

/**
 *  Create an instance of the MPR.
 *  @description Alternate API to create and initialize the MPR. The Mpr object manages Mpr facilities and 
 *      is the top level memory context. It may be used wherever a MprCtx parameter is required. This 
 *      function, or #mprCreate must be called prior to calling any other Mpr API.
 *  @param argc Count of arguments supplied in argv
 *  @param argv Program arguments. The MPR can store the program arguments for retrieval by other parts of the program.
 *  @param cback Callback function to be invoked on memory allocation errors. Set to null if not required.
 *  @param shell Optional reference to an O/S implementation dependent shell object. Used by Brew.
 *  @return Returns a pointer to the Mpr object. 
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern Mpr *mprCreateEx(int argc, char **argv, MprAllocNotifier cback, void *shell);

#define MPR_START_EVENTS_THREAD     0x1

/**
 *  Start the Mpr services
 *  @param mpr Mpr object created via mprCreateMpr
 *  @param startEventsThread Set to true to start an events thread.
 */
extern int mprStart(Mpr *mpr, int startEventsThread);

/**
 *  Stop the MPR and shutdown all services. After this call, the MPR cannot be used.
 *  @param mpr Mpr object created via mprCreateMpr
 *  @return True if all services have been successfully stopped. Otherwise false.
 */
extern bool mprStop(Mpr *mpr);

/**
 *  Signal the MPR to exit gracefully.
 *  @description Set the must exit flag for the MPR.
 *  @param ctx Any memory context allocated by the MPR.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern void mprSignalExit(MprCtx ctx);

/**
 *  Determine if the MPR should exit
 *  @description Returns true if the MPR should exit gracefully.
 *  @param ctx Any memory context allocated by the MPR.
 *  @returns True if the App has been instructed to exit.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern bool mprIsExiting(MprCtx ctx);

/**
 *  Set the application name, title and version
 *  @param ctx Any memory context allocated by the MPR.
 *  @param name One word, lower case name for the app.
 *  @param title Pascal case multi-word descriptive name.
 *  @param version Version of the app. Major-Minor-Patch. E.g. 1.2.3.
 *  @returns Zero if successful. Otherwise a negative MPR error code.
 */
extern int mprSetAppName(MprCtx ctx, cchar *name, cchar *title, cchar *version);

/**
 *  Get the application name defined via mprSetAppName
 *  @returns the one-word lower case application name defined via mprSetAppName
 */
extern cchar *mprGetAppName(MprCtx ctx);

extern char     *mprGetAppPath(MprCtx ctx);
extern char     *mprGetAppDir(MprCtx ctx);
extern cchar    *mprGetAppTitle(MprCtx ctx);
extern cchar    *mprGetAppVersion(MprCtx ctx);
extern void     mprSetHostName(MprCtx ctx, cchar *s);
extern cchar    *mprGetHostName(MprCtx ctx);
extern void     mprSetServerName(MprCtx ctx, cchar *s);
extern cchar    *mprGetServerName(MprCtx ctx);
extern void     mprSetDomainName(MprCtx ctx, cchar *s);
extern cchar    *mprGetDomainName(MprCtx ctx);
extern void     mprSetIpAddr(MprCtx ctx, cchar *s);
extern cchar    *mprGetIpAddr(MprCtx ctx);

/**
 *  Get the debug mode.
 *  @description Returns whether the debug mode is enabled. Some modules
 *      observe debug mode and disable timeouts and timers so that single-step
 *      debugging can be used.
 *  @param ctx Any memory context allocated by the MPR.
 *  @return Returns true if debug mode is enabled, otherwise returns false.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern bool     mprGetDebugMode(MprCtx ctx);

extern int      mprGetLogLevel(MprCtx ctx);

/**
 *  Return the O/S error code.
 *  @description Returns an O/S error code from the most recent system call. 
 *      This returns errno on Unix systems or GetLastError() on Windows..
 *  @return The O/S error code.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern int mprGetOsError();

/**
 *  Return the error code for the most recent system or library operation.
 *  @description Returns an error code from the most recent system call. 
 *      This will be mapped to be either a POSIX error code or an MPR error code.
 *  @return The mapped error code.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern int mprGetError();

extern int      mprMakeArgv(MprCtx ctx, cchar *prog, cchar *cmd, int *argc, char ***argv);

/** 
 *  Turn on debug mode.
 *  @description Debug mode disables timeouts and timers. This makes debugging
 *      much easier.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param on Set to true to enable debugging mode.
 *  @stability Evolving.
 */
extern void     mprSetDebugMode(MprCtx ctx, bool on);

/**
 *  Set the current logging level.
 *  @description This call defines the maximum level of messages that will be
 *      logged. Calls to mprLog specify a message level. If the message level
 *      is greater than the defined logging level, the message is ignored.
 *  @param ctx Any memory context allocated by the MPR.
 *  @param level New logging level. Must be 0-9 inclusive.
 *  @return Returns the previous logging level.
 *  @stability Evolving.
 *  @ingroup MprLog
 */
extern void     mprSetLogLevel(MprCtx ctx, int level);

extern void     mprSleep(MprCtx ctx, int msec);

#if BREW
extern uint     mprGetClassId(MprCtx ctx);
extern void     *mprGetDisplay(MprCtx ctx);
extern void     *mprGetShell(MprCtx ctx);
extern void     mprSetClassId(MprCtx ctx, uint classId);
extern void     mprSetDisplay(MprCtx ctx, void *display);
extern void     mprSetShell(MprCtx ctx, void *shell);
#endif

#if BLD_WIN_LIKE
extern int      mprReadRegistry(MprCtx ctx, char **buf, int max, cchar *key, cchar *val);
extern int      mprWriteRegistry(MprCtx ctx, cchar *key, cchar *name, cchar *value);
#endif

extern int      mprStartEventsThread(Mpr *mpr);

/**
 *  Terminate the MPR.
 *  @description Terminates the MPR and disposes of all allocated resources. The mprTerminate
 *      function will recursively free all memory allocated by the MPR.
 *  @param ctx Any memory context object returned by #mprAlloc.
 *  @param graceful Shutdown gracefully waiting for all events to drain. Otherise exit immediately
 *      without waiting for any threads or events to complete.
 *  @stability Evolving.
 *  @ingroup Mpr
 */
extern void     mprTerminate(MprCtx ctx, bool graceful);

extern bool     mprIsService(Mpr *mpr);
extern void     mprSetPriority(Mpr *mpr, int pri);
extern void     mprWriteToOsLog(MprCtx ctx, cchar *msg, int flags, int level);

#if BLD_WIN_LIKE
extern HWND     mprGetHwnd(MprCtx ctx);
extern void     mprSetHwnd(MprCtx ctx, HWND h);
extern long     mprGetInst(MprCtx ctx);
extern void     mprSetInst(MprCtx ctx, long inst);
extern void     mprSetSocketMessage(MprCtx ctx, int message);
#endif

extern int      mprGetRandomBytes(MprCtx ctx, char *buf, int size, int block);

extern int      mprGetEndian(MprCtx ctx);

/* ******************************** Unicode ***********************************/

#if WIN || WINCE
extern char* mprToAsc(MprCtx ctx, cuni *u);
extern uni* mprToUni(MprCtx ctx, cchar* a);
#endif

/* ****************************************************************************/
/*
   External dependencies
 */
extern char *dtoa(double d, int mode, int ndigits, int* decpt, int* sign, char** rve);
extern void freedtoa(char* ptr);

#ifdef __cplusplus
}
#endif
#endif /* _h_MPR */

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
/************************************************************************/
/*
 *  End of file "../src/include/mpr.h"
 */
/************************************************************************/



/************************************************************************/
/*
 *  Start of file "../src/include/mprTest.h"
 */
/************************************************************************/

/*
 *  mprTest.h - Header for the Embedthis Unit Test Framework
 *  
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */


#ifndef _h_MPR_TEST
#define _h_MPR_TEST 1



#if BLD_FEATURE_TEST
/*
 *  Tunable constants
 */
#define MPR_TEST_POLL_NAP   25
#define MPR_TEST_SLEEP      (60 * 1000)
#define MPR_TEST_MAX_STACK  (16)

/*
 *  Unit test definition structures
 */
struct MprTestGroup;
typedef void        (*MprTestProc)(struct MprTestGroup *tp);

typedef struct MprTestCase {
    char            *name;
    int             level;
    MprTestProc     proc;
    int             (*init)(struct MprTestGroup *gp);
    int             (*term)(struct MprTestGroup *gp);
} MprTestCase;

typedef struct MprTestDef {
    char                *name;
    struct MprTestDef   **groupDefs;
    int                 (*init)(struct MprTestGroup *gp);
    int                 (*term)(struct MprTestGroup *gp);
    MprTestCase         caseDefs[32];
} MprTestDef;


/*
 *  Assert macros for use by unit tests
 */
#undef  assert
#define assert(C)   assertTrue(gp, MPR_LOC, C, #C)

#define MPR_TEST(level, functionName) { #functionName, level, functionName, 0, 0 }

typedef struct MprTestService {
    int             argc;                   /* Count of arguments */
    char            **argv;                 /* Arguments for test */
    int             activeThreadCount;      /* Currently active test threads */
    char            *commandLine;
    bool            continueOnFailures;     /* Keep testing on failures */
    bool            debugOnFailures;        /* Break to the debugger */
    int             echoCmdLine;            /* Echo the command line */
    int             firstArg;               /* Count of arguments */
    MprList         *groups;                /* Master list of test groups */
    int             iterations;             /* Times to run the test */
    bool            singleStep;             /* Pause between tests */
    cchar           *name;                  /* Name for entire test */
    int             numThreads;             /* Number of test threads */
    int             workers;                /* Count of worker threads */
    MprTime         start;                  /* When testing began */
    int             testDepth;              /* Depth of entire test */
    MprList         *perThreadGroups;       /* Per thread copy of groups */
    int             totalFailedCount;       /* Total count of failing tests */
    int             totalTestCount;         /* Total count of all tests */
    MprList         *testFilter;            /* Test groups to run */
    int             verbose;                /* Output activity trace */

#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;                 /* Multi-thread sync */
#endif
} MprTestService;

extern MprTestService *mprCreateTestService(MprCtx ctx);
extern int          mprParseTestArgs(MprTestService *ts, int argc, char **argv);
extern int          mprRunTests(MprTestService *sp);
extern void         mprReportTestResults(MprTestService *sp);

/*
 *  A test group is a group of tests to cover a unit of functionality. A test group may contain other test groups.
 */
typedef struct MprTestGroup {
    char            *name;                  /* Name of test */
    char            *fullName;              /* Fully qualified name of test */
    int             testDepth;              /* Depth at which test should run */
    bool            skip;                   /* Skip this test */
    bool            skipWarned;             /* Warned that test will be skipped */

    bool            success;                /* Result of last run */
    int             failedCount;            /* Total failures of this test */
    int             testCount;              /* Count of tests */
    MprList         *failures;              /* List of all failures */

    MprTestService  *service;               /* Reference to the service */
    struct MprTestGroup *parent;            /* Parent test group */
    struct MprTestGroup *root;              /* Top level test group parent */

    MprList         *groups;                /* List of groups */
    MprList         *cases;                 /* List of tests in this group */
    MprTestDef      *def;                   /* Test definition ref */

    MprCond         *cond;                  /* Multi-thread sync */
    MprCond         *cond2;                 /* Second multi-thread sync */

#if BLD_FEATURE_HTTP_CLIENT
    MprHttp         *http;                  /* Http connection for this group */
    char            *content;               /* Cached response content */
#endif
    void            *data;                  /* Test specific data */
    MprCtx          ctx;                    /* Memory context for unit tests to use */
    
#if BLD_FEATURE_MULTITHREAD
    MprMutex        *mutex;                 /* Multi-thread sync */
#endif

} MprTestGroup;


extern MprTestGroup *mprAddTestGroup(MprTestService *ts, MprTestDef *def);
extern void         mprResetTestGroup(MprTestGroup *gp);
extern bool         assertTrue(MprTestGroup *gp, cchar *loc, bool success, cchar *msg);
extern void         mprSignalTestComplete(MprTestGroup *gp);
extern void         mprSignalTest2Complete(MprTestGroup *gp);
extern bool         mprWaitForTestToComplete(MprTestGroup *gp, int timeout);
extern bool         mprWaitForTest2ToComplete(MprTestGroup *gp, int timeout);


typedef struct MprTestFailure {
    char            *loc;
    char            *message;
} MprTestFailure;


#endif /* BLD_FEATURE_TEST */
#endif /* _h_MPR_TEST */


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
/************************************************************************/
/*
 *  End of file "../src/include/mprTest.h"
 */
/************************************************************************/



/************************************************************************/
/*
 *  Start of file "../src/include/pcre.h"
 */
/************************************************************************/

/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* This is the public header file for the PCRE library, to be #included by
applications that call the PCRE functions.

           Copyright (c) 1997-2008 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#ifndef _PCRE_H
#define _PCRE_H

/* The current PCRE version information. */

#define PCRE_MAJOR          7
#define PCRE_MINOR          7
#define PCRE_PRERELEASE     
#define PCRE_DATE           2008-05-07

/* When an application links to a PCRE DLL in Windows, the symbols that are
imported have to be identified as such. When building PCRE, the appropriate
export setting is defined in pcre_internal.h, which includes this file. So we
don't change existing definitions of PCRE_EXP_DECL and PCRECPP_EXP_DECL. */

#if BLD_FEATURE_STATIC
#define PCRE_STATIC
#endif

#if BLD_ALL_IN_ONE
    /*
     *  When building all-in-one, we must use internal definitions
     */
    #ifndef PCRE_EXP_DECL
    #  ifdef _WIN32
    #    ifndef PCRE_STATIC
    #      define PCRE_EXP_DECL       extern __declspec(dllexport)
    #      define PCRE_EXP_DEFN       __declspec(dllexport)
    #      define PCRE_EXP_DATA_DEFN  __declspec(dllexport)
    #    else
    #      define PCRE_EXP_DECL       extern
    #      define PCRE_EXP_DEFN
    #      define PCRE_EXP_DATA_DEFN
    #    endif
    #  else
    #    ifdef __cplusplus
    #      define PCRE_EXP_DECL       extern "C"
    #    else
    #      define PCRE_EXP_DECL       extern
    #    endif
    #    ifndef PCRE_EXP_DEFN
    #      define PCRE_EXP_DEFN       PCRE_EXP_DECL
    #    endif
    #    ifndef PCRE_EXP_DATA_DEFN
    #      define PCRE_EXP_DATA_DEFN
    #    endif
    #  endif
    #endif
    
    /* EMBEDTHIS */
    #undef PCRE_EXP_DEFN
    #define PCRE_EXP_DEFN 
    #undef PCRE_EXP_DATA_DEFN
    #define PCRE_EXP_DATA_DEFN 
    #undef PCRE_EXP_DECL
    #define PCRE_EXP_DECL extern
#else
    #if defined(_WIN32) && !defined(PCRE_STATIC)
    #  ifndef PCRE_EXP_DECL
    #    define PCRE_EXP_DECL  extern __declspec(dllimport)
    #  endif
    #  ifdef __cplusplus
    #    ifndef PCRECPP_EXP_DECL
    #      define PCRECPP_EXP_DECL  extern __declspec(dllimport)
    #    endif
    #    ifndef PCRECPP_EXP_DEFN
    #      define PCRECPP_EXP_DEFN  __declspec(dllimport)
    #    endif
    #  endif
    #endif
    
    /* By default, we use the standard "extern" declarations. */
    
    #ifndef PCRE_EXP_DECL
    #  ifdef __cplusplus
    #    define PCRE_EXP_DECL  extern "C"
    #  else
    #    define PCRE_EXP_DECL  extern
    #  endif
    #endif
    
    #ifdef __cplusplus
    #  ifndef PCRECPP_EXP_DECL
    #    define PCRECPP_EXP_DECL  extern
    #  endif
    #  ifndef PCRECPP_EXP_DEFN
    #    define PCRECPP_EXP_DEFN
    #  endif
    #endif

    /* EMBEDTHIS */
    #undef PCRE_EXP_DEFN
    #define PCRE_EXP_DEFN 
    #undef PCRE_EXP_DATA_DEFN
    #define PCRE_EXP_DATA_DEFN 
    #undef PCRE_EXP_DECL
    #define PCRE_EXP_DECL extern
#endif

/* Have to include stdlib.h in order to ensure that size_t is defined;
it is needed here for malloc. */

#include <stdlib.h>

/* Allow for C++ users */

#ifdef __cplusplus
extern "C" {
#endif

/* Options */

#define PCRE_CASELESS           0x00000001
#define PCRE_MULTILINE          0x00000002
#define PCRE_DOTALL             0x00000004
#define PCRE_EXTENDED           0x00000008
#define PCRE_ANCHORED           0x00000010
#define PCRE_DOLLAR_ENDONLY     0x00000020
#define PCRE_EXTRA              0x00000040
#define PCRE_NOTBOL             0x00000080
#define PCRE_NOTEOL             0x00000100
#define PCRE_UNGREEDY           0x00000200
#define PCRE_NOTEMPTY           0x00000400
#define PCRE_UTF8               0x00000800
#define PCRE_NO_AUTO_CAPTURE    0x00001000
#define PCRE_NO_UTF8_CHECK      0x00002000
#define PCRE_AUTO_CALLOUT       0x00004000
#define PCRE_PARTIAL            0x00008000
#define PCRE_DFA_SHORTEST       0x00010000
#define PCRE_DFA_RESTART        0x00020000
#define PCRE_FIRSTLINE          0x00040000
#define PCRE_DUPNAMES           0x00080000
#define PCRE_NEWLINE_CR         0x00100000
#define PCRE_NEWLINE_LF         0x00200000
#define PCRE_NEWLINE_CRLF       0x00300000
#define PCRE_NEWLINE_ANY        0x00400000
#define PCRE_NEWLINE_ANYCRLF    0x00500000
#define PCRE_BSR_ANYCRLF        0x00800000
#define PCRE_BSR_UNICODE        0x01000000
#define PCRE_JAVASCRIPT_COMPAT  0x02000000

/* Exec-time and get/set-time error codes */

#define PCRE_ERROR_NOMATCH         (-1)
#define PCRE_ERROR_NULL            (-2)
#define PCRE_ERROR_BADOPTION       (-3)
#define PCRE_ERROR_BADMAGIC        (-4)
#define PCRE_ERROR_UNKNOWN_OPCODE  (-5)
#define PCRE_ERROR_UNKNOWN_NODE    (-5)  /* For backward compatibility */
#define PCRE_ERROR_NOMEMORY        (-6)
#define PCRE_ERROR_NOSUBSTRING     (-7)
#define PCRE_ERROR_MATCHLIMIT      (-8)
#define PCRE_ERROR_CALLOUT         (-9)  /* Never used by PCRE itself */
#define PCRE_ERROR_BADUTF8        (-10)
#define PCRE_ERROR_BADUTF8_OFFSET (-11)
#define PCRE_ERROR_PARTIAL        (-12)
#define PCRE_ERROR_BADPARTIAL     (-13)
#define PCRE_ERROR_INTERNAL       (-14)
#define PCRE_ERROR_BADCOUNT       (-15)
#define PCRE_ERROR_DFA_UITEM      (-16)
#define PCRE_ERROR_DFA_UCOND      (-17)
#define PCRE_ERROR_DFA_UMLIMIT    (-18)
#define PCRE_ERROR_DFA_WSSIZE     (-19)
#define PCRE_ERROR_DFA_RECURSE    (-20)
#define PCRE_ERROR_RECURSIONLIMIT (-21)
#define PCRE_ERROR_NULLWSLIMIT    (-22)  /* No longer actually used */
#define PCRE_ERROR_BADNEWLINE     (-23)

/* Request types for pcre_fullinfo() */

#define PCRE_INFO_OPTIONS            0
#define PCRE_INFO_SIZE               1
#define PCRE_INFO_CAPTURECOUNT       2
#define PCRE_INFO_BACKREFMAX         3
#define PCRE_INFO_FIRSTBYTE          4
#define PCRE_INFO_FIRSTCHAR          4  /* For backwards compatibility */
#define PCRE_INFO_FIRSTTABLE         5
#define PCRE_INFO_LASTLITERAL        6
#define PCRE_INFO_NAMEENTRYSIZE      7
#define PCRE_INFO_NAMECOUNT          8
#define PCRE_INFO_NAMETABLE          9
#define PCRE_INFO_STUDYSIZE         10
#define PCRE_INFO_DEFAULT_TABLES    11
#define PCRE_INFO_OKPARTIAL         12
#define PCRE_INFO_JCHANGED          13
#define PCRE_INFO_HASCRORLF         14

/* Request types for pcre_config(). Do not re-arrange, in order to remain
compatible. */

#define PCRE_CONFIG_UTF8                    0
#define PCRE_CONFIG_NEWLINE                 1
#define PCRE_CONFIG_LINK_SIZE               2
#define PCRE_CONFIG_POSIX_MALLOC_THRESHOLD  3
#define PCRE_CONFIG_MATCH_LIMIT             4
#define PCRE_CONFIG_STACKRECURSE            5
#define PCRE_CONFIG_UNICODE_PROPERTIES      6
#define PCRE_CONFIG_MATCH_LIMIT_RECURSION   7
#define PCRE_CONFIG_BSR                     8

/* Bit flags for the pcre_extra structure. Do not re-arrange or redefine
these bits, just add new ones on the end, in order to remain compatible. */

#define PCRE_EXTRA_STUDY_DATA             0x0001
#define PCRE_EXTRA_MATCH_LIMIT            0x0002
#define PCRE_EXTRA_CALLOUT_DATA           0x0004
#define PCRE_EXTRA_TABLES                 0x0008
#define PCRE_EXTRA_MATCH_LIMIT_RECURSION  0x0010

/* Types */

struct real_pcre;                 /* declaration; the definition is private  */
typedef struct real_pcre pcre;

/* When PCRE is compiled as a C++ library, the subject pointer type can be
replaced with a custom type. For conventional use, the public interface is a
const char *. */

#ifndef PCRE_SPTR
#define PCRE_SPTR const char *
#endif

/* The structure for passing additional data to pcre_exec(). This is defined in
such as way as to be extensible. Always add new fields at the end, in order to
remain compatible. */

typedef struct pcre_extra {
  unsigned long int flags;        /* Bits for which fields are set */
  void *study_data;               /* Opaque data from pcre_study() */
  unsigned long int match_limit;  /* Maximum number of calls to match() */
  void *callout_data;             /* Data passed back in callouts */
  const unsigned char *tables;    /* Pointer to character tables */
  unsigned long int match_limit_recursion; /* Max recursive calls to match() */
} pcre_extra;

/* The structure for passing out data via the pcre_callout_function. We use a
structure so that new fields can be added on the end in future versions,
without changing the API of the function, thereby allowing old clients to work
without modification. */

typedef struct pcre_callout_block {
  int          version;           /* Identifies version of block */
  /* ------------------------ Version 0 ------------------------------- */
  int          callout_number;    /* Number compiled into pattern */
  int         *offset_vector;     /* The offset vector */
  PCRE_SPTR    subject;           /* The subject being matched */
  int          subject_length;    /* The length of the subject */
  int          start_match;       /* Offset to start of this match attempt */
  int          current_position;  /* Where we currently are in the subject */
  int          capture_top;       /* Max current capture */
  int          capture_last;      /* Most recently closed capture */
  void        *callout_data;      /* Data passed in with the call */
  /* ------------------- Added for Version 1 -------------------------- */
  int          pattern_position;  /* Offset to next item in the pattern */
  int          next_item_length;  /* Length of next item in the pattern */
  /* ------------------------------------------------------------------ */
} pcre_callout_block;

/* Indirection for store get and free functions. These can be set to
alternative malloc/free functions if required. Special ones are used in the
non-recursive case for "frames". There is also an optional callout function
that is triggered by the (?) regex item. For Virtual Pascal, these definitions
have to take another form. */

#ifndef VPCOMPAT
PCRE_EXP_DECL void *(*pcre_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre_free)(void *);
PCRE_EXP_DECL void *(*pcre_stack_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre_stack_free)(void *);
PCRE_EXP_DECL int   (*pcre_callout)(pcre_callout_block *);
#else   /* VPCOMPAT */
PCRE_EXP_DECL void *pcre_malloc(size_t);
PCRE_EXP_DECL void  pcre_free(void *);
PCRE_EXP_DECL void *pcre_stack_malloc(size_t);
PCRE_EXP_DECL void  pcre_stack_free(void *);
PCRE_EXP_DECL int   pcre_callout(pcre_callout_block *);
#endif  /* VPCOMPAT */

/* Exported PCRE functions */

PCRE_EXP_DECL pcre *pcre_compile(const char *, int, const char **, int *,
                  const unsigned char *);
PCRE_EXP_DECL pcre *pcre_compile2(const char *, int, int *, const char **,
                  int *, const unsigned char *);
PCRE_EXP_DECL int  pcre_config(int, void *);
PCRE_EXP_DECL int  pcre_copy_named_substring(const pcre *, const char *,
                  int *, int, const char *, char *, int);
PCRE_EXP_DECL int  pcre_copy_substring(const char *, int *, int, int, char *,
                  int);
PCRE_EXP_DECL int  pcre_dfa_exec(const pcre *, const pcre_extra *,
                  const char *, int, int, int, int *, int , int *, int);
PCRE_EXP_DECL int  pcre_exec(const pcre *, const pcre_extra *, PCRE_SPTR,
                   int, int, int, int *, int);
PCRE_EXP_DECL void pcre_free_substring(const char *);
PCRE_EXP_DECL void pcre_free_substring_list(const char **);
PCRE_EXP_DECL int  pcre_fullinfo(const pcre *, const pcre_extra *, int,
                  void *);
PCRE_EXP_DECL int  pcre_get_named_substring(const pcre *, const char *,
                  int *, int, const char *, const char **);
PCRE_EXP_DECL int  pcre_get_stringnumber(const pcre *, const char *);
PCRE_EXP_DECL int  pcre_get_stringtable_entries(const pcre *, const char *,
                  char **, char **);
PCRE_EXP_DECL int  pcre_get_substring(const char *, int *, int, int,
                  const char **);
PCRE_EXP_DECL int  pcre_get_substring_list(const char *, int *, int,
                  const char ***);
PCRE_EXP_DECL int  pcre_info(const pcre *, int *, int *);
PCRE_EXP_DECL const unsigned char *pcre_maketables(void);
PCRE_EXP_DECL int  pcre_refcount(pcre *, int);
PCRE_EXP_DECL pcre_extra *pcre_study(const pcre *, int, const char **);
PCRE_EXP_DECL const char *pcre_version(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* End of pcre.h */
/************************************************************************/
/*
 *  End of file "../src/include/pcre.h"
 */
/************************************************************************/

