#
#	Makefile for the Ejscript script code. 
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include 	.makedep

#
#	Order does influence slot allocation. Put the core types first.
#
CORE_FILES	:= \
	core/Object.es \
	core/Boolean.es \
	core/Null.es \
	core/Number.es \
	core/String.es \
	core/Void.es \
	core/Frame.es \
	core/Function.es \
	core/Array.es \
	core/Block.es \
	core/ByteArray.es \
	core/Date.es \
	core/Error.es \
	core/Iterator.es \
	core/JSON.es \
	core/Math.es \
	core/Name.es \
	core/Namespace.es \
	core/Reflect.es \
	core/RegExp.es \
	core/Stream.es \
	core/Type.es \
	core/Global.es \
	events/Event.es \
	events/Dispatcher.es \
	events/Timer.es \
	xml/XML.es \
	xml/XMLList.es \
	io/BinaryStream.es \
	io/File.es \
	io/FileSystem.es \
	io/Http.es \
	io/Path.es \
	io/TextStream.es \
	io/XMLHttp.es \
	sys/App.es \
	sys/Cmd.es \
	sys/Compat.es \
	sys/Config.es \
	sys/Debug.es \
	sys/GC.es \
	sys/Logger.es \
	sys/Memory.es \
	sys/System.es \
	sys/Unix.es \
	sys/Worker.es

DB_FILES := \
	db/Database.es \
	db/DatabaseConnector.es \
	db/Record.es

MYSQL_FILES := 

SQLITE_FILES := \
	db/Sqlite.es

ifeq	($(BLD_FEATURE_EJS_WEB),1)
	WEB_FILES	:= \
		web/Cache.es	\
		web/Controller.es	\
		web/Cookie.es	\
		web/Host.es	\
		web/Request.es	\
		web/Response.es	\
		web/Session.es	\
		web/UploadFile.es	\
		web/View.es \
		web/connectors/HtmlConnector.es \
		web/connectors/GoogleConnector.es
endif

CORE_TARGETS	+= $(BLD_MOD_DIR)/ejs.mod
CORE_HEADERS 	+= $(BLD_INC_DIR)/ejs.slots.h
CORE_HEADERS 	+= $(BLD_INC_DIR)/ejs.events.slots.h
CORE_HEADERS 	+= $(BLD_INC_DIR)/ejs.io.slots.h
CORE_HEADERS 	+= $(BLD_INC_DIR)/ejs.sys.slots.h

ifeq	($(BLD_FEATURE_STATIC),1)
	#
	#	Static builds put ejs.db.* and ejs.web into the core
	#
	CORE_FILES	+= $(DB_FILES) $(MYSQL_FILES) $(SQLITE_FILES) $(WEB_FILES)
	DB_FILES	=
	MYSQL_FILES	=
	SQLITE_FILES=
	WEB_FILES	=
	_ESFLAGS	+= --bind
	ifeq	($(BLD_FEATURE_MYSQL),1)
		CORE_HEADERS	+= $(BLD_INC_DIR)/ejs.db.mysql.slots.h
	endif
	ifeq	($(BLD_FEATURE_SQLITE),1)
		CORE_HEADERS	+= $(BLD_INC_DIR)/ejs.db.sqlite.slots.h
	endif
	ifeq	($(BLD_FEATURE_EJS_WEB),1)
		CORE_HEADERS	+= $(BLD_INC_DIR)/ejs.web.slots.h
	endif
else
	ifeq	($(BLD_FEATURE_EJS_DB),1)
		EXTRA_TARGETS	+= $(BLD_MOD_DIR)/ejs.db.mod
	endif
	ifeq	($(BLD_FEATURE_MYSQL),1)
		EXTRA_TARGETS	+= $(BLD_MOD_DIR)/ejs.db.mysql.mod
		EXTRA_HEADERS	+= $(BLD_INC_DIR)/ejs.db.mysql.slots.h
	endif
	ifeq	($(BLD_FEATURE_SQLITE),1)
		EXTRA_TARGETS	+= $(BLD_MOD_DIR)/ejs.db.sqlite.mod
		EXTRA_HEADERS	+= $(BLD_INC_DIR)/ejs.db.sqlite.slots.h
	endif
	ifeq	($(BLD_FEATURE_EJS_WEB),1)
		EXTRA_TARGETS	+= $(BLD_MOD_DIR)/ejs.web.mod
		EXTRA_HEADERS	+= $(BLD_INC_DIR)/ejs.web.slots.h
		EJS_WEB_MOD_DEP	+= $(BLD_MOD_DIR)/ejs.db.mod
		EJSWEB_DEP		+= $(BLD_MOD_DIR)/ejs.web.mod
	endif
endif
ifeq	($(BLD_FEATURE_EJS_WEB),1)
    EXTRA_TARGETS	    += $(BLD_BIN_DIR)/ejsweb
	ifeq ($(BUILD_NATIVE_OR_COMPLETE_CROSS),1)
		EXTRA_TARGETS	+= $(BLD_MOD_DIR)/ejsweb.mod
	endif
	ifeq ($(BLD_WIN_LIKE),1)
        EXTRA_TARGETS	+= $(BLD_BIN_DIR)/ejsweb.cmd
    endif
endif

ifeq	($(BLD_FEATURE_DOC),1)
	DOC_DIR				:= $(BLD_TOP)/doc/api/ejscript
	EXTRA_TARGETS		+= $(DOC_DIR)/index.html
endif

ifeq	($(BLD_DEBUG),0)
	#
	#	Currently all builds are debug.
	#
	_ESFLAGS	+= --debug
endif

compileExtra: $(CORE_TARGETS) $(CORE_HEADERS)

modules: $(EXTRA_TARGETS) $(EXTRA_HEADERS) dummy

#
#	Only copy the slot headers if they actually change. Minimizes build churn.
#
$(BLD_INC_DIR)/%.slots.h: %.slots.h
	@if ! diff $@ $< >/dev/null 2>&1 ; then \
		cp $< $@ ; \
		[ "$(TRACE)" = 1 ] && echo "cp $< $@" ; true ; \
	fi

$(BLD_MOD_DIR)/ejs.mod: ejs.mod
	cp ejs.mod $(BLD_MOD_DIR)/ejs.mod

ejs.mod: $(CORE_FILES) $(BLD_TOP)/buildConfig.make
	@$(call log) "[Generate]" ejs.mod
	$(call setlibpath) ; ejsc $(_ESFLAGS) --bind --empty --out ejs.mod $(CORE_FILES)

ejs.slots.h: ejs.mod
	@$(call log) "[Generate]" ejs.slots.h
	$(call setlibpath) ; ejsmod --showDebug --empty --showBuiltin --listing --cslots ejs.mod

#
#	DB
#
ejs.db.mod: $(DB_FILES) $(CORE_FILES) $(BLD_TOP)/buildConfig.make ejs.mod
	@$(call log) "[Generate]" ejs.db.mod
	$(call setlibpath) ; ejsc $(_ESFLAGS) --out ejs.db.mod $(DB_FILES)
	$(call setlibpath) ; ejsmod --listing --cslots ejs.db.mod

$(BLD_MOD_DIR)/ejs.db.mod: ejs.db.mod
	cp ejs.db.mod $(BLD_MOD_DIR)/ejs.db.mod

#
#	MySQL
#
ejs.db.mysql.mod: $(MYSQL_FILES) $(CORE_FILES) $(BLD_TOP)/buildConfig.make ejs.mod $(EJS_WEB_MOD_DEP)
	@$(call log) "[Generate]" ejs.db.mysql.mod
	$(call setlibpath) ; ejsc $(_ESFLAGS) --out ejs.db.mysql.mod $(MYSQL_FILES)

$(BLD_MOD_DIR)/ejs.db.mysql.mod: ejs.db.mysql.mod
	cp ejs.db.mysql.mod $(BLD_MOD_DIR)/ejs.db.mysql.mod

ifeq	($(BLD_FEATURE_STATIC),0)
ejs.db.mysql.slots.h: ejs.db.mysql.mod
	@$(call log) "[Generate]" ejs.db.mysql.slots.h
	$(call setlibpath) ; ejsmod --listing --cslots ejs.db.mysql.mod
endif

#
#	SQLite
#
ejs.db.sqlite.mod: $(SQLITE_FILES) $(CORE_FILES) $(BLD_TOP)/buildConfig.make ejs.mod $(EJS_WEB_MOD_DEP)
	@$(call log) "[Generate]" ejs.db.sqlite.mod
	$(call setlibpath) ; ejsc $(_ESFLAGS) --out ejs.db.sqlite.mod $(SQLITE_FILES)

$(BLD_MOD_DIR)/ejs.db.sqlite.mod: ejs.db.sqlite.mod
	cp ejs.db.sqlite.mod $(BLD_MOD_DIR)/ejs.db.sqlite.mod

ifeq	($(BLD_FEATURE_STATIC),0)
ejs.db.sqlite.slots.h: ejs.db.sqlite.mod
	@$(call log) "[Generate]" ejs.db.sqlite.slots.h
	$(call setlibpath) ; ejsmod --listing --cslots ejs.db.sqlite.mod
endif

#
#	Web
#
ejs.web.mod: $(WEB_FILES) $(CORE_FILES) $(BLD_TOP)/buildConfig.make ejs.mod $(EJS_WEB_MOD_DEP)
	@$(call log) "[Generate]" ejs.web.mod
	$(call setlibpath) ; ejsc $(_ESFLAGS) --out ejs.web.mod $(WEB_FILES)

$(BLD_MOD_DIR)/ejs.web.mod: ejs.web.mod
	cp ejs.web.mod $(BLD_MOD_DIR)/ejs.web.mod

ifeq	($(BLD_FEATURE_STATIC),0)
ejs.web.slots.h: ejs.web.mod
	@$(call log) "[Generate]" ejs.web.slots.h
	$(call setlibpath) ; ejsmod --listing --cslots ejs.web.mod
endif

#
#	Generate documentation for all es files
#	
doc: $(BLD_TOP)/doc/api/ejscript/index.html

$(DOC_DIR)/index.html: $(CORE_FILES) $(DB_FILES) $(MYSQL_FILES) $(SQLITE_FILES) $(WEB_FILES) Makefile
	@$(call log) "[Generate]" documentation
	$(call setlibpath) ; \
	ejsc --doc --bind $(_ESFLAGS) --out doc.mod --empty $(CORE_FILES) $(DB_FILES) $(MYSQL_FILES) $(SQLITE_FILES) $(WEB_FILES)
	rm -rf $(DOC_DIR)/*.html $(DOC_DIR)/*.css $(DOC_DIR)/images/*
	$(call setlibpath) ; ejsmod --html $(DOC_DIR) --empty --showBuiltin doc.mod
	rm -f doc.mod

#
#	Create the ejsweb module
#
$(BLD_MOD_DIR)/ejsweb.mod: web/ejsweb.es ejs.mod $(EJSWEB_DEP) $(EJS_WEB_MOD_DEP)
	@$(call log) "[Generate]" ejsweb.mod
	$(call setlibpath) ; ejsc --debug --out $(BLD_MOD_DIR)/ejsweb.mod web/ejsweb.es
	mkdir -p $(BLD_LIB_DIR)/default-web/images
	mkdir -p $(BLD_LIB_DIR)/default-web/themes
	cp -r web/default-web/* $(BLD_LIB_DIR)/default-web

$(BLD_BIN_DIR)/ejsweb: web/ejsweb.es
	cp web/ejsweb.es $(BLD_BIN_DIR)/ejsweb
	chmod 755 $(BLD_BIN_DIR)/ejsweb

$(BLD_BIN_DIR)/ejsweb.cmd: web/ejsweb.cmd
	cp web/ejsweb.cmd $(BLD_BIN_DIR)/ejsweb.cmd
	chmod 755 $(BLD_BIN_DIR)/ejsweb.cmd

cleanExtra:
	rm -f *.h *.mod *.lst $(BLD_MOD_DIR)/ejsweb.mod *.h

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4
#
