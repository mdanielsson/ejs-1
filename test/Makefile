#
#	Makefile for the Ejscript unit test program
#
#	Copyright (c) Embedthis LLC, 2003-2012. All Rights Reserved.
#

BUILD_DEPTH	?=	1
BUILD_CROSS	:= 0

include 	.makedep

testExtra: 
ifeq	($(BLD_FEATURE_TEST),1)
ifeq	($(BLD_FEATURE_MULTITHREAD),1)
	$(BLD_BIN_DIR)/ejs $(BLD_TOOLS_DIR)/utest -v -d $(BUILD_DEPTH)
else
	$(call log) "[Notice]" "Test requires multithread configuration"
endif
endif
	
cleanExtra:
	rm -f *.lst */*.lst */*.mod *.mod *.h *.h test.sdb
	rm -fr .doctmp 

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
