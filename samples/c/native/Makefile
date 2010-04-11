#
#	Makefile to build the composite sample
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

EXPORT_OBJECTS	:= 0
BUILD_CROSS		:= 0

include 		.makedep

ifeq			($(BLD_FEATURE_STATIC),0)
compileFirst: 	Sample.mod

Sample.mod: Shape.es
	$(call setlibpath) ; ejsc Shape.es
	$(call setlibpath) ; ejsmod --cslots Sample.mod

compileExtra: Sample.mod Sample$(BLD_SHOBJ)

Sample$(BLD_SHOBJ) : $(OBJECTS) $(BLD_EJS_MAKEDEP) Sample.slots.h
	@bld --shared --library Sample --shared --search "$(BLD_LIB_DIR) $(BLD_LIB_PREFIX)" \
		--libs "$(BLD_EJS_LIBS)" $(OBJECTS)

else
compileExtra:
	@$(call log) "[Notice]" "Skip building composite sample. Requires \"configure --enable-shared\"."
endif

cleanExtra:
	@rm -f *.lst *.mod *$(BLD_LIB)
