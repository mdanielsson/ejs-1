#
#	Makefile to build the DB Classes
#
#	Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
#

include 			.makedep

ifeq ($(BLD_FEATURE_SQLITE),1)
	ifeq ($(BLD_SQLITE_DIR),)
		PRE_DIRS	+= sqlite
	endif
	MAKE_IFLAGS		+= $(BLD_SQLITE_IFLAGS)
endif

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
