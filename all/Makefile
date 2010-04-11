#
#	Makefile to build the all-in-one Ejscript distribution
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include 		.makedep

COMPILE			=
TARGETS			+= ejs.h ejs.slots.h ec.h ejsLib.c ecLib.c ejs.c ejsc.c ejsmod.c ejssql.c ejsweb.c sqlite3.c sqlite3.h ejs.es
EJS_HEADERS		+= $(patsubst %,../src/include/%.h, ejsTune ejsByteCode ejsByteCodeTable ejsVm ejsCore ejsModule ejs ejsWeb)
SLOT_HEADERS	+= $(shell find ../src/include -name 'ejs*slots.h')
EC_HEADERS		+= $(patsubst %,../src/include/%.h, ecCompiler)
EJS_SOURCES		+= $(shell find ../src/types ../src/vm -name '*.c') 
EC_SOURCES		+= $(shell find ../src/compiler -name '*.c') 
ES_SOURCES		+= $(shell find ../src/es/core ../src/es/db ../src/es/events ../src/es/io ../src/es/sys ../src/es/web \
				    ../src/es/xml -name '*.es' | \
					egrep -v 'Archive|old|Socket.es|sample.es|XmlGlobal.es|ejsweb.es|Jsdb.es|Mysql.es') 
EJSMOD_HEADERS	+= ../src/ejsmod/ejsmod.h
EJSMOD_SOURCES	+= $(shell find ../src/ejsmod -name '*.c')
GATEWAYS		+= ejsApache.c ejsAppweb.c ejsCgi.c
TARGETS			+= $(GATEWAYS)

ifeq	($(BLD_FEATURE_EJS_DB),1)
	DB_SOURCES		+= ../src/db/ejsSqlite.c 
	SQLITE_HEADERS	+= ../src/include/sqlite3.h
	SQLITE_SOURCES	+= ../src/db/sqlite/sqlite3.c
endif
ifeq	($(BLD_FEATURE_EJS_WEB),1)
	TARGETS		+= ejsweb.es
	WEB_SOURCES	+= $(shell find ../src/web -name '*.c' | egrep -v '/gateways|/proto/')
endif

compileExtra:		$(TARGETS) dummy

ejs.h: $(EJS_HEADERS)
	@$(call log) "[Generate]" all/ejs.h
	@all-in-one $(EJS_HEADERS) | egrep -v '#include.*ejs|#include.*ec|#include.*sql' >ejs.h
	@echo '#include "ejs.slots.h"' >>ejs.h

ejs.slots.h: $(SLOT_HEADERS)
	@$(call log) "[Generate]" all/ejs.slots.h
	@all-in-one $(SLOT_HEADERS) >ejs.slots.h

ec.h: $(EC_HEADERS)
	@$(call log) "[Generate]" all/ejs.slots.h
	all-in-one $(EC_HEADERS) | egrep -v '#include.*ejs|#include.*ec|#include.*sql' >ec.h

ejsLib.c: $(EJS_SOURCES) $(DB_SOURCES) $(WEB_SOURCES)
	@$(call log) "[Generate]" all/ejsLib.c
	echo '#define EJS_DEFINE_OPTABLE 1' >ejsLib.c
	echo '#include "ejs.h"' >>ejsLib.c
	all-in-one $(EJS_SOURCES) $(DB_SOURCES) $(WEB_SOURCES) | csplit -s -f tmpEjsLib. - '/#include.*ejsByteGoto.h/'
	(cat tmpEjsLib.00 ; cat ../src/include/ejsByteGoto.h ; \
	 if [ "$(BLD_OS)" = MACOSX ] ; then tail +2 tmpEjsLib.01 ; else tail --lines=+2 tmpEjsLib.01 ; fi ) | \
	 egrep -v '#inc.*ejs|#inc.*ec|#inc.*sqlConf' >>ejsLib.c
	@rm -f tmpEjsLib.*

ecLib.c: $(EJS_HEADERS) $(EC_HEADERS) $(EC_SOURCES)
	@$(call log) "[Generate]" all/ecLib.c
	echo '#include "ejs.h"' >ecLib.c
	echo '#include "ec.h"' >>ecLib.c
	all-in-one $(EC_SOURCES) | egrep -v '#inc.*ejs|#inc.*ec|#inc.*sql' >>ecLib.c

ejsc.c: $(EJS_HEADERS) $(EC_HEADERS)
	@$(call log) "[Generate]" all/ejsc.c
	echo '#include "ejs.h"' >ejsc.c
	echo '#include "ec.h"' >>ejsc.c
	all-in-one ../src/cmd/ejsc.c | egrep -v '#inc.*ejs|#inc.*ec|#inc.*sql' >>ejsc.c

ejs.c: $(EJS_HEADERS) $(EC_HEADERS)
	@$(call log) "[Generate]" all/ejsc.c
	echo '#include "ejs.h"' >ejs.c
	echo '#include "ec.h"' >>ejs.c
	all-in-one ../src/cmd/ejs.c $(EC_SOURCES) | egrep -v '#inc.*ejs|#inc.*ec|#inc.*sql' >>ejs.c

ejsmod.c: $(EJSMOD_HEADERS) $(EJSMOD_SOURCES)
	@$(call log) "[Generate]" all/ejsmod.c
	echo '#include "ejs.h"' >ejsmod.c
	all-in-one $(EJSMOD_HEADERS) $(EJSMOD_SOURCES) | egrep -v '#include.*ejs|#include.*ec|#include.*sql' >>ejsmod.c

ejsweb.c: ../src/cmd/ejsweb.c
	@$(call log) "[Generate]" all/ejsweb.c
	echo '#include "ejs.h"' >ejsweb.c
	echo '#include "ec.h"' >>ejsweb.c
	all-in-one ../src/cmd/ejsweb.c | egrep -v '#inc.*ejs|#inc.*ec|#inc.*sql' >>ejsweb.c

ejssql.c: ../src/db/sqlite/ejssql.c
	@$(call log) "[Generate]" all/ejssql.c
	all-in-one $(SQLITE_HEADERS) ../src/db/sqlite/ejssql.c | egrep -v '#include.*ejs|#include.*sqlite3' >ejssql.c

sqlite3.c: $(SQLITE_SOURCES) $(SQLITE_HEADERS)
	@$(call log) "[Generate]" all/sqlite3.c
	all-in-one $(SQLITE_HEADERS) $(SQLITE_SOURCES) | egrep -v '#include.*ejs|#include.*sqlite3' >sqlite3.c

sqlite3.h: $(BLD_INC_DIR)/sqlite3.h
	@$(call log) "[Generate]" all/sqlite3.h
	cp $(BLD_INC_DIR)/sqlite3.h sqlite3.h

ejs.es: $(ES_SOURCES)
	@$(call log) "[Generate]" all/ejs.es
	all-in-one $(ES_SOURCES)  | egrep -v 'use strict' >ejs.es

ejsweb.es: ../src/es/web/ejsweb.es
	@$(call log) "[Generate]" all/ejsweb.es
	cp ../src/es/web/ejsweb.es .
	cp -r $(BLD_LIB_DIR)/default-web .

$(GATEWAYS): ../src/web/gateways/$@
	cp ../src/web/gateways/$@ .
	cp ../src/appweb/ejswebserver.conf .
	cp ../src/appweb/mime.types .
	chmod 644 ejswebserver.conf mime.types

clobberExtra:
	rm -f *.es *.c *.h
