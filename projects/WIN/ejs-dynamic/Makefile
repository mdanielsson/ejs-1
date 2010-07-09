#
#	Makefile for Windows Visual Studio projects
#
#	Copyright (c) Embedthis Software LLC, 2003-2009. All Rights Reserved.
#

include     .makedep

# include     buildConfig.override

S  			= ../../../src
MPR_HDR 	= $(BLD_TOP)/buildConfig.h $(shell find $(S)/include -name 'mpr*.h')
EJS_HDR 	= $(shell find $(S)/include -name 'ejs*.h')
EC_HDR  	= $(shell find $(S)/include -name 'ec*.h')
APP_HDR 	= $(S)/include/appweb.h $(S)/include/http.h
MPR_SRC 	= $(shell find $(S)/mpr -name '*.c' | egrep -v 'http.c|makerom.c')
EJS_SRC 	= $(shell find $(S)/types $(S)/vm $(S)/db -name '*.c' | egrep -v 'ejssql.c')
WEB_SRC 	= $(shell find $(S)/web -name '*.c' | egrep -v 'gateways')
EC_SRC 		= $(shell find $(S)/compiler -name '*.c' | egrep -v 'XXXXX')
EJSMOD_SRC 	= $(shell find $(S)/ejsmod -name '*.c' | egrep -v 'XXXXX')
APPWEB_SRC	= $(S)/appweb/appwebLib.c
LIBS		= ws2_32.lib
PROJECTS	= libmpr.vcproj libsqlite3.vcproj libmprssl.vcproj libpcre.vcproj libejs.vcproj libec.vcproj \
			  ejswebserver.vcproj ejscgi.vcproj ejsmod.vcproj ejsvm.vcproj ejsc.vcproj ejs.vcproj ejssql.vcproj \
              ejsweb.vcproj ejs.web.vcproj

MAKE_IFLAGS += -I$(BLD_TOP)/src/include/master $(BLD_REGEXP_IFLAGS) $(BLD_MPR_IFLAGS)
ifeq ($(BLD_FEATURE_OPENSSL),1)
	SSL_IFLAGS	+= $(BLD_OPENSSL_IFLAGS)
endif
ifeq ($(BLD_FEATURE_MATRIXSSL),1)
	SSL_IFLAGS	+= $(BLD_MATRIXSSL_IFLAGS)
endif

compileExtra: prep $(PROJECTS)

prep:
	if ls ../../../bin/*.def >/dev/null 2>&1 ; then \
		cp ../../../bin/*.def . ; \
	fi

libmpr.vcproj: Makefile buildConfig.override
	@makeVsProj --project libmpr.vcproj --library mpr --dflags "$(DFLAGS)" \
		--def libmpr.def --libs "$(LIBS)" $(MPR_HDR) $(MPR_SRC)
	
libmprssl.vcproj: Makefile buildConfig.override
	@makeVsProj --project libmprssl.vcproj --library mprSsl --dflags "$(DFLAGS)" \
		--iflags "$(SSL_IFLAGS)" --def libmprssl.def --libs "libmpr.lib $(LIBS)" \
		$(MPR_HDR) $(S)/mpr/mprSsl.c
	
libpcre.vcproj: Makefile buildConfig.override
	@makeVsProj --project libpcre.vcproj --library pcre --dflags "$(DFLAGS)" \
		--def libpcre.def --libs "libmpr.lib $(LIBS)" $(MPR_HDR) $(S)/mpr/mprPcre.c
	
libsqlite3.vcproj: Makefile buildConfig.override
	@makeVsProj --project libsqlite3.vcproj --library sqlite3 --dflags "$(DFLAGS)" \
		--def libsqlite3.def --libs "libmpr.lib $(LIBS)" $(MPR_HDR) $(S)/db/sqlite/sqlite3.c
	
#
#	Ejscript core library
#
libejs.vcproj: Makefile buildConfig.override
	@makeVsProj --project libejs.vcproj --library ejs --dflags "$(DFLAGS)" \
		--def libejs.def --libs "libmpr.lib libsqlite3.lib $(LIBS)" $(MPR_HDR) $(EJS_HDR) $(EJS_SRC)
	
#
#	Ejscript compiler library
#
libec.vcproj: Makefile buildConfig.override
	@makeVsProj --project libec.vcproj --library ec --dflags "$(DFLAGS)" \
		--def libec.def --libs "libmpr.lib libsqlite3.lib libejs.lib $(LIBS)" $(MPR_HDR) $(EJS_HDR) $(EC_SRC)

ejs.web.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejs.web.vcproj --library ec --dflags "$(DFLAGS)" \
		--def ejs.web.def --libs "libmpr.lib libsqlite3.lib libejs.lib $(LIBS)" $(MPR_HDR) $(EJS_HDR) $(WEB_SRC)

ejsc.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejsc.vcproj --exe ejsc.exe --search "$(BLD_EJS_LIBPATHS)" --dflags "$(DFLAGS)" \
		--custom ejsc.custom --libs "$(LIBS) libec.lib libejs.lib libmpr.lib" $(S)/cmd/ejsc.c

ejsmod.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejsmod.vcproj --exe ejsmod.exe --search "$(BLD_EJS_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libejs.lib libmpr.lib" $(EJSMOD_SRC)

ejsvm.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejsvm.vcproj --exe ejsvm.exe --search "$(BLD_EJS_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libejs.lib libmpr.lib" $(S)/cmd/ejsvm.c

ejs.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejs.vcproj --exe ejs.exe --search "$(BLD_EJS_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libec.lib libejs.lib libmpr.lib" $(S)/cmd/ejs.c

ejssql.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejssql.vcproj --exe ejssql.exe --search "$(BLD_MPR_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libsqlite3.lib" $(S)/db/sqlite/ejssql.c

ejsweb.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejsweb.vcproj --exe ejsweb.exe --search "$(BLD_MPR_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libejs.lib libmpr.lib" $(S)/cmd/ejsweb.c

ejswebserver.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejswebserver.vcproj --exe ejswebserver.exe --search "$(BLD_EJS_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) ejs.web.lib libejs.lib libmpr.lib" $(S)/appweb/appweb.c $(S)/appweb/appwebLib.c \
        $(S)/web/gateways/ejsAppweb.c 

ejscgi.vcproj: Makefile buildConfig.override
	@makeVsProj --project ejscgi.vcproj --exe ejscgi.exe --search "$(BLD_EJS_LIBPATHS)" --dflags "$(DFLAGS)" \
		--libs "$(LIBS) ejs.web.lib libejs.lib libmpr.lib" $(S)/web/gateways/ejsCgi.c
