#
#	Makefile for the packages
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include 		.makedep 

PRE_DIRS		= $(BLD_HOST_OS)
BUILD_DEPTH		?= 2
BASE_NAME		:= "$(BLD_PRODUCT)-$(BLD_VERSION)-$(BLD_NUMBER)-$(BLD_HOST_DIST)-$(BLD_HOST_OS)-$(BLD_HOST_CPU)"
PACKS			:= binary dev source
PKG_DIR			:= `pwd`/../staging
INS_DIR			:= c:/tmp/testInsEjs

ifeq	($(BLD_OS),1)
	PRIV		:= fakepriv
else
	PRIV		:= priv
endif

install install-%: prep
	packs=$* ; [ "$$packs" = "" ] && packs="binary dev" ; \
	[ "$$packs" = "binary dev" ] && BROWSER=--browser ; \
	priv OPTIONS=$(OPTIONS) QUIET=$(QUIET) TRACE=$(TRACE) \
		$(BLD_TOP)/build/bin/makeInstall $$BROWSER --root=$(ROOT_DIR) --install $$packs post

uninstall uninstall-%: prep
	packs=$* ; [ "$$packs" = "" ] && packs="binary dev" ; \
	priv OPTIONS=$(OPTIONS) QUIET=$(QUIET) TRACE=$(TRACE) \
		$(BLD_TOP)/build/bin/makeInstall --root=$(ROOT_DIR) --remove $$packs post
	
packageExtra:
	if [ "$(BUILD_DEPTH)" -ge 2 ] ; then \
		fakePriv "OPTIONS=$(OPTIONS) TRACE=$(TRACE) \
			$(BLD_TOP)/build/bin/makeInstall --root=$(PKG_DIR) --package $(PACKS) && \
			$(BLD_TOOLS_DIR)/makePackage --root=$(PKG_DIR)" ; \
	else \
		echo "Packaging skipped at build depth $(BUILD_DEPTH)" ; \
	fi

packageOnly:
	fakePriv $(BLD_TOOLS_DIR)/makePackage --root=$(PKG_DIR) $(PACKS)

test-install-package:
test-uninstall-package: 
test-install-native-package:
test-uninstall-native-package:
test-installed:
test-removed:

prep:
	if [ ! -x $(BLD_TOOLS_DIR)/edep$(BLD_BUILD_EXE) -a "$(BUILDING_CROSS)" != 1 ] ; then \
		$(MAKE) -S --no-print-directory _RECURSIVE_=1 -C $(BLD_TOP)/build/src compile ; \
	fi

checkInstalled: installed-files

installed-files:
	( \
		for d in /etc /usr/src /usr/share /usr/share/doc /usr/lib /usr/lib/$(BLD_PRODUCT) /usr/include \
			/usr/local/bin /usr/local/bin/$(BLD_PRODUCT) /usr/bin /usr/bin/$(BLD_PRODUCT) \
			/usr/include/$(BLD_PRODUCT) /var/log /var/www ; \
		do \
			if [ "`echo $$d/$(BLD_PRODUCT)*`" != $$d/$(BLD_PRODUCT)'*' ] ; then \
				eval echo "$$d/$(BLD_PRODUCT)*" ; \
				continue ; \
			fi ; \
			if [ -x "$$d/$(BLD_PRODUCT)" ] ; then \
				echo "$$d/$(BLD_PRODUCT)" ; \
				continue ; \
			fi ; \
		done ; \
		for d in "$(BLD_CFG_PREFIX)" "$(BLD_DOC_PREFIX)" "$(BLD_INC_PREFIX)" "$(BLD_LIB_PREFIX)" "$(BLD_LOG_PREFIX)" \
			"$(BLD_SAM_PREFIX)" "$(BLD_SRC_PREFIX)" ; \
		do \
			[ -x "$$d" ] && echo "$$d" || true; \
		done ; \
	) | sort | uniq

dependExtra:
	[ "$(shell echo *.sh)" != "*.sh" ] && chmod +x *.sh ; true
	mkdir -p ../releases
