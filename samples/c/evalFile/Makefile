#
#	Makefile to build the evalFile sample
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

EXPORT_OBJECTS	:= 0
BUILD_CROSS		:= 0

include 		.makedep

compileExtra: main$(BLD_EXE)

main$(BLD_EXE) : $(OBJECTS) $(BLD_EJS_MAKEDEP)
	@bld --exe main$(BLD_EXE) --shared --libs "ec $(BLD_EJS_LIBS)" $(OBJECTS)

cleanExtra:
	@rm -f *.mod main$(BLD_EXE)
