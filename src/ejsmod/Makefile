#
#	Makefile for the Ejscript module manager program
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include 		.makedep

ifeq	($(BUILD_NATIVE_OR_COMPLETE_CROSS),1)
	TARGETS		+= $(BLD_BIN_DIR)/ejsmod$(BLD_EXE) 
endif

compileExtra: 	$(TARGETS) 

#
#	Make the module manager
#
$(BLD_BIN_DIR)/ejsmod$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libmpr$(BLD_LIB) $(BLD_LIB_DIR)/libejs$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/ejsmod$(BLD_EXE) --search "$(BLD_EJS_LIBPATHS)" --libs "$(BLD_EJS_LIBS)" \
		ejsmod doc docFiles listing slotGen

docFiles.c: images/banner.jpg doc.css images/inherit.gif
	$(call setlibpath) ; makerom images/banner.jpg doc.css images/inherit.gif | \
		sed 's/ROMFS/EJS_DOC/;s/mpr.h/ejsmod.h/;s/MprRomInode defaultRomFiles/DocFile docFiles/' >docFiles.c


#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
