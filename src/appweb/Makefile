#
#	Makefile for Appweb
#
#	Copyright (c) Embedthis Software LLC, 2003-2008. All Rights Reserved.
#
#	This is an all-in-one build where all the Appweb source files are catenated into as few files as possible. 
#	See http://hg.embedthis.com/appweb if you require the full source for the Appweb.
#
#	NOTE: Makefile.all is ignored
#

include 	.makedep

MAKE_IFLAGS		+= $(BLD_OPENSSL_IFLAGS) $(BLD_MATRIXSSL_IFLAGS)

compileExtra: $(BLD_LIB_DIR)/ejswebserver.conf $(BLD_LIB_DIR)/mime.types

ifeq ($(BLD_FEATURE_ROMFS),1)
compileFirst: romFiles.c
endif

romFiles.c: mime.types ejswebserver.conf default-web/*.html
	$(call log) "[Generate]" "romFiles.c"
	echo -e ".\nejswebserver.conf\nmime.types" >rom.files
	find default-web -print | egrep -v 'CVS|\.svn|\.cvsignore|\.ignore|\.tmp|\.sav|\.php|\.ejs|php' >>rom.files
	$(call setlibpath) ; sort rom.files | uniq | xargs makerom --name romFiles >romFiles.c

$(BLD_LIB_DIR)/ejswebserver.conf: ejswebserver.conf
	cp ejswebserver.conf $(BLD_LIB_DIR)

$(BLD_LIB_DIR)/mime.types: mime.types
	cp mime.types $(BLD_LIB_DIR)


#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
