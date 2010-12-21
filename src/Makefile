#
#	Makefile for the Ejscript source code.
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#
#	Building strategy is:
#	1. Build the core types (PRE_DIRS)
#	2. Build the core libraries (PRE_TARGETS)
#	3. Build the compiler and tools (MID_DIRS)
#	4. Generate the slot files (MID_DIRS - es)
#	5. See if ejs.slots.h changed, if so, clean and redo from step 1
#	6. Build remaining commands: ejssql, ejscgi, appweb and modules (TARGETS)
#

include 		.makedep

PRE_DIRS		+= include
ifeq ($(BLD_MPR_BUILTIN),1)
	PRE_DIRS	+= mpr utils
endif
PRE_DIRS		+= types regex vm db
ifeq ($(BLD_FEATURE_EJS_WEB),1)
	PRE_DIRS	+= web 
endif

MID_DIRS		+= compiler cmd ejsmod es
ifeq ($(BLD_FEATURE_APPWEB),1)
	POST_DIRS	+= appweb
endif

#
#	Targets. This builds the following targets:
#		Libraries:   ejs, sqlite3, ejsdb, ejsweb
#		Executables: ejssql, ejscgi
#		Modules:     mod_ejs (Appweb and Apache ejs modules)
#
PRE_TARGETS			+= $(BLD_LIB_DIR)/libejs$(BLD_LIB)
ifeq ($(BLD_FEATURE_MYSQL),1)
	PRE_TARGETS		+= $(BLD_MOD_DIR)/ejs.db.mysql$(BLD_LIB) 
endif
ifeq ($(BLD_FEATURE_SQLITE),1)
	PRE_TARGETS		+= $(BLD_LIB_DIR)/libsqlite3$(BLD_LIB) 
	PRE_TARGETS		+= $(BLD_MOD_DIR)/ejs.db.sqlite$(BLD_LIB) 
	ifeq ($(BUILD_NATIVE_OR_COMPLETE_CROSS),1)
		TARGETS		+= $(BLD_BIN_DIR)/ejssql$(BLD_EXE) 
	endif
endif
ifeq ($(BLD_FEATURE_EJS_WEB),1)
	ifeq ($(BLD_FEATURE_STATIC),0)
		PRE_TARGETS	+= $(BLD_MOD_DIR)/ejs.web$(BLD_LIB)
	endif
	ifeq ($(BUILD_CROSS_OR_COMPLETE_NATIVE),1)
		TARGETS		+= $(BLD_BIN_DIR)/ejscgi$(BLD_EXE)
	endif
	ifeq ($(BLD_FEATURE_APPWEB),1)
		TARGETS		+= $(BLD_BIN_DIR)/ejswebserver$(BLD_EXE)
		ifeq ($(BLD_UNIX_LIKE),1)
			TARGETS	+= $(BLD_BIN_DIR)/angel$(BLD_EXE)
		endif
#		ifneq	($(BLD_APPWEB_DIR),)
#			TARGETS	+= $(BLD_BIN_DIR)/mod_appweb_ejs$(BLD_SHOBJ)
#		endif
	endif
	## ifeq ($(BLD_FEATURE_APACHE),1)
	##	TARGETS		+= mod_ejs.la
	##endif
endif

#
#	If static building: include objects from db and web in the ejs library
#
ifeq ($(BLD_FEATURE_STATIC),1)
	ifeq ($(BLD_FEATURE_EJS_DB),1)
		EJS_SOURCES	+= $(wildcard db/*.c)
	endif
	ifeq ($(BLD_FEATURE_EJS_WEB),1)
		EJS_SOURCES	+= $(shell find web -name '*.c' | egrep -v '/gateways/|/proto/')
	endif
else
    # CYGWIN needs SHOBJ
	WEB_LIBS	+= ejs.web$(BLD_SHOBJ) 
endif

EJS_SOURCES		+= $(shell find types vm -name '*.c') 
EJS_OBJECTS		+= $(patsubst %.c,$(BLD_OBJ_DIR)/%$(BLD_OBJ),$(notdir $(EJS_SOURCES)))
WEB_SOURCES		+= $(shell find web -name '*.c' | egrep -v '/gateways/|/proto/') 
WEB_OBJECTS		+= $(patsubst %.c,$(BLD_OBJ_DIR)/%$(BLD_OBJ),$(notdir $(WEB_SOURCES)))

APPWEB_OBJECTS	+= appweb appwebLib ejsAppweb$(BLD_OBJ)
ifeq ($(BLD_FEATURE_ROMFS),1)
APPWEB_OBJECTS	+= romFiles$(BLD_OBJ)
endif

compileFirst:
	@cp include/ejs.slots.h .ejs.slots.h

compileExtra: $(PRE_TARGETS)
	@+T=compile ; D="$(MID_DIRS)" ; $(DO_RECURSE) 
#
#	Build again if the slots change.
#
	@if ! diff include/ejs.slots.h .ejs.slots.h >/dev/null ; then \
		$(call log) "[Note]" "Core slots may have changed, rebuilding compiler with current slot definitions." ; \
		rm -f $(BLD_LIB_DIR)/libejs$(BLD_LIB) ; \
		T=compile ; D="$(PRE_DIRS)" ; $(DO_RECURSE) ; \
		$(MAKE) TRACE=$(TRACE) $(BLD_LIB_DIR)/libejs$(BLD_LIB) ; \
		T=compile ; D="$(MID_DIRS)" ; $(DO_RECURSE) ; \
	fi
	@if [ "$(RETRY)" = "" ] ; then \
		$(MAKE) RETRY=1 TRACE=$(TRACE) -C ./es modules ; \
	fi

compileFinal: $(TARGETS)

#
#	libejs - Core Ejscript language library
#
$(BLD_LIB_DIR)/libejs$(BLD_LIB): $(EJS_OBJECTS) $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --library $(BLD_LIB_DIR)/libejs --libs "$(BLD_EJS_WITHLIBS)" $(EJS_OBJECTS)

#
#	ejs.web - Ejscript web framework (only if shared)
#
$(BLD_MOD_DIR)/ejs.web$(BLD_LIB): $(WEB_OBJECTS)
	bld --library $(BLD_MOD_DIR)/ejs.web$(BLD_LIB) --libs "ejs $(BLD_MPR_LIBS)" $(WEB_OBJECTS)

#
#	libsqlite3 - Sqlite3 library
#
$(BLD_LIB_DIR)/libsqlite3$(BLD_LIB): $(BLD_OBJ_DIR)/sqlite3$(BLD_OBJ)
	bld --library $(BLD_LIB_DIR)/libsqlite3 --libs "$(BLD_MPR_LIBS)" sqlite3

$(BLD_MOD_DIR)/ejs.db.mysql$(BLD_LIB): $(BLD_OBJ_DIR)/ejsSqlite$(BLD_OBJ) 
	bld --library $(BLD_MOD_DIR)/ejs.db.mysql$(BLD_LIB) --libs "ejs $(BLD_MPR_LIBS) $(BLD_MYSQL_LIBS)" ejsMysql$(BLD_OBJ)

$(BLD_MOD_DIR)/ejs.db.sqlite$(BLD_LIB): $(BLD_OBJ_DIR)/ejsSqlite$(BLD_OBJ) 
	bld --library $(BLD_MOD_DIR)/ejs.db.sqlite$(BLD_LIB) --libs "ejs $(BLD_MPR_LIBS) $(BLD_SQLITE_LIBS)" ejsSqlite$(BLD_OBJ)


#
#	ejssql - Sqlite command program
#
$(BLD_BIN_DIR)/ejssql$(BLD_EXE): $(BLD_LIB_DIR)/libsqlite3$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejssql$(BLD_EXE) --libs "$(BLD_MPR_LIBS)" ejssql sqlite3

#	#
#	#	mod_appweb_ejs - Appweb ejs module and handler
#	#
#	$(BLD_BIN_DIR)/mod_appweb_ejs$(BLD_SHOBJ): $(BLD_OBJ_DIR)/ejsAppweb$(BLD_OBJ) $(BLD_LIB_DIR)/libejs$(BLD_LIB)
#		bld --shared --library $(BLD_MOD_DIR)/mod_appweb_ejs --libs "$(BLD_MPR_LIBS)" \
#			$(BLD_OBJ_DIR)/ejsAppweb$(BLD_OBJ)

#
#	ejscgi - CGI gateway program
#
$(BLD_BIN_DIR)/ejscgi$(BLD_EXE): $(BLD_OBJ_DIR)/ejsCgi$(BLD_OBJ) $(BLD_LIB_DIR)/libejs$(BLD_LIB)
	bld --shared --exe $(BLD_BIN_DIR)/ejscgi$(BLD_EXE) --libs "$(BLD_EJS_LIBS)" --modules "$(WEB_LIBS)" \
        $(BLD_OBJ_DIR)/ejsCgi$(BLD_OBJ)

#
#	Appweb web server
#
$(BLD_BIN_DIR)/ejswebserver$(BLD_EXE): $(patsubst %,$(BLD_OBJ_DIR)/%$(BLD_OBJ),appweb appwebLib ejsAppweb) \
		$(BLD_LIB_DIR)/libejs$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejswebserver$(BLD_EXE) --libs "$(BLD_EJS_LIBS)" --modules "$(WEB_LIBS)" $(APPWEB_OBJECTS)

$(BLD_BIN_DIR)/angel$(BLD_EXE): $(patsubst %,$(BLD_OBJ_DIR)/%$(BLD_OBJ),angel) \
		$(BLD_LIB_DIR)/libejs$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/angel$(BLD_EXE) --libs "$(BLD_MPR_LIBS)" angel$(BLD_OBJ)

#
#	FUTURE
#
apache_install:
	priv apxs -ie mod_ejs.la
	priv apachectl restart

mod_ejs.la: ejsApache.c Makefile
	cp ejsApache.c mod_ejs.c
	apxs -c $(_IFLAGS) -D ENABLE_COMPILE=1 -Wc,"-arch $(BLD_CPU)" -Wl,"-arch $(BLD_CPU)" mod_ejs.c
	cp .libs/mod_ejs.so $(BLD_BIN_DIR)

cleanExtra:
	rm -f mod_ejs* *.mod */*.mod

#
#	LoadModule ejs_module modules/mod_ejs.so
#	<Location /ejs>
#	SetHandler ejs
#	</Location
#
#	apachectl restart

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
