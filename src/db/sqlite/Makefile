#
#	Makefile to build Sqlite
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include 		.makedep

ifeq ($(BLD_HOST_WIN),1)
	MAKE_DFLAGS	+= -D_CRT_SECURE_NO_WARNINGS
endif
ifeq ($(BLD_HOST_OS),CYGWIN)
_CFLAGS 		:= $(shell echo $(_CFLAGS) | sed 's/-W3/-W1/;s/-Wall/-w/;s/-g3//')
else
_CFLAGS 		:= $(shell echo $(_CFLAGS) | sed 's/-W3/-W1/;s/-Wall/-w/')
endif

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
