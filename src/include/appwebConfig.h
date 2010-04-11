/*
 *  appwebConfig.h -- Local Appweb configuration
 *
 *  Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#ifndef _h_APPWEB_CONFIG_h
#define _h_APPWEB_CONFIG_h 1

#include "buildConfig.h"

/*
 *  Local appweb configuration
 */
#define BLD_FEATURE_ACCESS_LOG 1
#define BLD_FEATURE_AUTH 1
#define BLD_FEATURE_AUTH_DIGEST 1
#define BLD_FEATURE_AUTH_FILE 1
#define BLD_FEATURE_AUTH_PAM 0
#define BLD_FEATURE_CGI 1
#define BLD_FEATURE_CHUNK 1
#define BLD_FEATURE_CMD 1
#define BLD_FEATURE_CONFIG_PARSE 1
#define BLD_FEATURE_DIR 1
#define BLD_FEATURE_EGI 0
#define BLD_FEATURE_LOG 1
#define BLD_FEATURE_PHP 0
#define BLD_FEATURE_RUN_AS_SERVICE 0
#define BLD_FEATURE_FILE 1
#define BLD_FEATURE_NET 1
#define BLD_FEATURE_RANGE 1
#define BLD_FEATURE_UPLOAD 1

#if BLD_FEATURE_ROMFS
#define BLD_FEATURE_SEND 0
#else
#define BLD_FEATURE_SEND 1
#endif

#endif /* _h_APPWEB_CONFIG_h */
