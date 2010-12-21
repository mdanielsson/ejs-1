#
#	Makefile for the Ejscript Compiler library
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include 		.makedep

ifeq	($(BUILDING_CROSS),1)
	ifeq	($(BLD_FEATURE_EJS_CROSS_COMPILER),1)
		TARGETS	+= $(BLD_LIB_DIR)/libec$(BLD_LIB)
	endif
else
	TARGETS		+= $(BLD_LIB_DIR)/libec$(BLD_LIB)
endif

compileExtra: 	$(TARGETS)

$(BLD_LIB_DIR)/libec$(BLD_LIB): $(OBJECTS)
	bld --library $(BLD_LIB_DIR)/libec --search "$(BLD_EJS_LIBPATHS) $(BLD_EJS_WITHPATHS)" \
		--libs "$(BLD_EJS_LIBS) $(BLD_EJS_WITHLIBS)" $(OBJECTS)

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
