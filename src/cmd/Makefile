#
#	Makefile for the Ejscript command line programs: ejsc, ejs, ejsvm and ejsweb
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include 		.makedep

ifeq ($(BUILDING_CROSS),1)
	TARGETS		+= $(BLD_BIN_DIR)/ejsvm$(BLD_EXE)
	ifeq ($(BLD_FEATURE_EJS_CROSS_COMPILER),1)
		TARGETS		+= $(BLD_BIN_DIR)/ejs$(BLD_EXE) 
		TARGETS		+= $(BLD_BIN_DIR)/ejsc$(BLD_EXE)
		ifeq ($(BLD_UNIX_LIKE),0)
			ifeq ($(BLD_FEATURE_EJS_WEB),1)
				TARGETS	+= $(BLD_BIN_DIR)/ejsweb$(BLD_EXE)
			endif
		endif
	endif
else
	TARGETS		+= $(BLD_BIN_DIR)/ejs$(BLD_EXE) 
	TARGETS		+= $(BLD_BIN_DIR)/ejsvm$(BLD_EXE)
	TARGETS		+= $(BLD_BIN_DIR)/ejsc$(BLD_EXE)
	ifeq ($(BLD_FEATURE_EJS_WEB),1)
		ifeq ($(BLD_WIN_LIKE),1)
            TARGETS	+= $(BLD_BIN_DIR)/ejsweb$(BLD_EXE)
        endif
		ifeq ($(BLD_OS),CYGWIN)
            TARGETS	+= $(BLD_BIN_DIR)/ejsweb$(BLD_EXE)
        endif
    endif
endif

compileExtra: $(TARGETS)

EJS_OBJECTS		+= $(BLD_OBJ_DIR)/ejs$(BLD_OBJ)
ifeq ($(BLD_FEATURE_ROMFS),1)
	EJS_OBJECTS	+= $(BLD_OBJ_DIR)/romFiles$(BLD_OBJ)
compileFirst: romFiles.c
endif

#
#	This is just a sample for how to build a ROMable version of ejs
#
romFiles.c:
	$(call log) "[Generate]" "romFiles.c"
	echo "print('Hello World')" >hello.es
	echo "hello.es" >rom.files 
	$(call setlibpath) ; sort rom.files | uniq | xargs $(BLD_BIN_DIR)/makerom --name romFiles >romFiles.c
	rm hello.es

#
#	Make the compiler
#
$(BLD_BIN_DIR)/ejsc$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libejs$(BLD_LIB) $(BLD_LIB_DIR)/libec$(BLD_LIB) \
		$(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejsc$(BLD_EXE) --search "$(BLD_EJS_LIBPATHS)" --libs "ec $(BLD_EJS_LIBS)" \
		"$(BLD_OBJ_DIR)/ejsc$(BLD_OBJ)"

#
#	Make the C vm interpreter
#
$(BLD_BIN_DIR)/ejsvm$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libejs$(BLD_LIB) $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejsvm$(BLD_EXE) --search "$(BLD_EJS_LIBPATHS)" --libs "$(BLD_EJS_LIBS)" \
		"$(BLD_OBJ_DIR)/ejsvm$(BLD_OBJ)"

#
#	Make the Ejscript shell
#
$(BLD_BIN_DIR)/ejs$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libejs$(BLD_LIB) $(BLD_LIB_DIR)/libec$(BLD_LIB) \
		$(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejs$(BLD_EXE) --search "$(BLD_EJS_LIBPATHS)" --libs "ec $(BLD_EJS_LIBS)" $(EJS_OBJECTS)

#
#	Make the ejsweb framework generator
#
$(BLD_BIN_DIR)/ejsweb$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejsweb$(BLD_EXE) --search "$(BLD_EJS_LIBPATHS)" --libs "ec $(BLD_EJS_LIBS)" \
		"$(BLD_OBJ_DIR)/ejsweb$(BLD_OBJ)"
