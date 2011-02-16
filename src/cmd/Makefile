#
#	Makefile for the Ejscript command line programs: ejsc, ejs, ejsvm and ejsweb
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include 		.makedep

ifeq ($(BUILDING_CROSS),1)
	TARGETS		+= $(BLD_BIN_DIR)/ejsvm$(BLD_EXE)
	ifeq ($(BLD_FEATURE_EJS_CROSS_COMPILER),1)
		TARGETS		+= $(BLD_BIN_DIR)/ejs$(BLD_EXE) 
		TARGETS		+= $(BLD_BIN_DIR)/ejsc$(BLD_EXE)
	endif
else
	TARGETS		+= $(BLD_BIN_DIR)/ejs$(BLD_EXE) 
	TARGETS		+= $(BLD_BIN_DIR)/ejsvm$(BLD_EXE)
	TARGETS		+= $(BLD_BIN_DIR)/ejsc$(BLD_EXE)
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
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
