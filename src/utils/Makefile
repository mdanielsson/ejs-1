#
#	Makefile for the Ejscript command line programs: ejsc, ejs, ejsvm
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

EXPORT_OBJECTS	:= 0
BUILD_CROSS		:= 0

include 		.makedep

TARGETS			+= $(BLD_BIN_DIR)/genbyte$(BLD_EXE)

compileExtra: 	$(TARGETS) 

#
#	Make the compiler
#
$(BLD_BIN_DIR)/genbyte$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/genbyte$(BLD_EXE) --search "$(BLD_MPR_LIBPATHS)" --libs "$(BLD_MPR_LIBS)" \
		"genbyte$(BLD_OBJ)"

gen:
	genbyte ../include

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
