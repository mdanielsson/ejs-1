# 
#	Makefile for the Ejscript Server-Side JavaScript Language and Web Framework
#
#	Copyright (C) 2009 Embedthis Software
#
#	This software is licensed under the GPLv2. Commercial and other licenses 
#   are available from http://embedthis.com.
#
#  	$Id$
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ejs
PKG_RELEASE:=1

#
#    Build 1.0.2-1
#
PKG_SOURCE_URL:=http://ejscript.org/software/
PKG_VERSION:=1.0.2-1
PKG_LIB_DIR:= /usr/lib/ejs/1.0.2
PKG_MD5SUM:=b30e2e541cf4d92e41aafd4f45696075
PKG_SOURCE:=$(PKG_NAME)-src-$(PKG_VERSION).tar.gz

#
#    Build the latest snapshot from the default trunk
#
#    PKG_SOURCE:=tip.tar.gz
#    PKG_SOURCE_URL:= http://hg.embedthis.com/ejs/archive/
#    PKG_VERSION:=default
#    PKG_MD5SUM:=

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_STAGE_DIR:=$(PKG_BUILD_DIR)/staging

include $(INCLUDE_DIR)/package.mk

EXTRA_CFG:=$(if $(CONFIG_PACKAGE_libopenssl),--with-openssl=$(wildcard $(BUILD_DIR)/openssl*))

CONFIGURE_ARGS += -q --defaults=release --shared --tune=size --disable-samples \
	--enable-complete-cross --enable-complete-native $(EXTRA_CFG)

define Package/ejs/Default
	SUBMENU:=Ejscript
	SECTION:=lang
	CATEGORY:=Languages
	TITLE:=Ejscript programming language
	URL:=http://ejscript.org/
endef

define Package/ejs/Default/description
	Ejscript is a compact and powerful server-side JavaScript language and web 
	framework. It can be also be used stand-alone as a general-purpose command 
	line scripting language.
endef

define Package/libejs
	$(call Package/ejs/Default)
	SUBMENU:=
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE+= (libraries)
endef

define Package/libejs/description
	$(call Package/ejs/Default/description)
	This package contains the Ejscript shared libraries that include the 
	Ejscript APIs and Virtual Machine interpreter.
endef

define Package/ejs
	$(call Package/ejs/Default)
	DEPENDS:=libejs
	TITLE+= (command shell)
endef

define Package/ejs/description
	$(call Package/ejs/Default/description)
	This package contains the Ejscript language command shell .
endef

define Package/ejsc
	$(call Package/ejs/Default)
	DEPENDS:=libejs
	TITLE+= (compiler)
endef

define Package/ejsc/description
	$(call Package/ejs/Default/description)
	This package contains the Ejscript language compiler (ejsc) and 
	stand-alone VM command interpreter (ejsvm).
endef

define Package/ejstools
	$(call Package/ejs/Default)
	DEPENDS:=libejs
	TITLE+= (tools)
endef

define Package/ejstools/description
	$(call Package/ejs/Default/description)
	This package contains the Ejscript language tools including module 
	disassembler and documentation generator program (ejsmod) and the 
	SQLite command line program (ejssql).
endef

define Package/ejsweb
	$(call Package/ejs/Default)
	DEPENDS:=libejs
	TITLE+= (web framework)
endef

define Package/ejsweb/description
	$(call Package/ejs/Default/description)
	This package contains the Ejscript MVC web framework. It also includes 
	the ejsweb generator program to create and manage Ejscript web applications.
endef

define Package/ejscgi
	$(call Package/ejs/Default)
	DEPENDS:=ejsweb
	TITLE+= (cgi)
endef

define Package/ejscgi/description
	$(call Package/ejs/Default/description)
	This package contains the Ejscript CGI program.
endef

define Package/ejswebserver
	$(call Package/ejs/Default)
	DEPENDS:=ejsweb
	TITLE+= (web server)
endef

define Package/ejswebserver/description
	$(call Package/ejs/Default/description)
	This package contains the a version of the Appweb embedded webserver 
	configured to support Ejscript applications. 
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	tar xzf "$(DL_DIR)/$(PKG_SOURCE)" --strip-components 1 -C "$(PKG_BUILD_DIR)"
endef

define Build/Configure
    $(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) 
	( cd $(PKG_BUILD_DIR) ; $(PKG_BUILD_DIR)/build/bin/makeInstall \
		--root=$(PKG_STAGE_DIR) --install --with-openwrt=1 binary )
endef

define Package/libejs/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/lib/
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/modules/
	$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libmpr*.so $(1)$(PKG_LIB_DIR)/lib/
	if [ -f $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libpcre.so ] ; then \
		$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libpcre.so $(1)$(PKG_LIB_DIR)/lib/ ; \
	fi
	if [ -f $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libsqlite3.so ] ; then \
		$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libsqlite3.so $(1)$(PKG_LIB_DIR)/lib/ ; \
	fi
	$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libec.so $(1)$(PKG_LIB_DIR)/lib/
	$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libejs*.so $(1)$(PKG_LIB_DIR)/lib/
	if [ -f $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libmprssl.so ] ; then \
		$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/libmprssl*.so $(1)$(PKG_LIB_DIR)/lib/ ; \
	fi
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/modules
	$(INSTALL_BIN) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/modules/*.mod $(1)$(PKG_LIB_DIR)/modules/
	if [ -f $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/modules/ejs.web.so ] ; then \
		$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/modules/ejs.web.so $(1)$(PKG_LIB_DIR)/modules/ ; \
	fi
	if [ -f $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/modules/ejs.db.so ] ; then \
		$(CP) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/modules/ejs.db.so $(1)$(PKG_LIB_DIR)/modules/ ; \
	fi
	$(INSTALL_BIN) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/bin/linkup $(1)$(PKG_LIB_DIR)/bin/
endef

define Package/ejs/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/lib/
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_BIN) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/bin/ejs $(1)$(PKG_LIB_DIR)/bin/
	$(CP) $(PKG_STAGE_DIR)/CONTROL/* $(1)/CONTROL
endef

define Package/ejsc/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_BIN) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/bin/{ejsc,ejsvm} $(1)$(PKG_LIB_DIR)/bin/
	$(CP) $(PKG_STAGE_DIR)/CONTROL/* $(1)/CONTROL
endef

define Package/ejstools/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_BIN) $(PKG_STAGE_DIR)/${PKG_LIB_DIR}/bin/{ejsmod,ejssql} $(1)$(PKG_LIB_DIR)/bin/
	$(CP) $(PKG_STAGE_DIR)/CONTROL/* $(1)/CONTROL
endef

define Package/ejsweb/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/lib/
	$(INSTALL_BIN) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/bin/ejsweb $(1)$(PKG_LIB_DIR)/bin/
	$(CP) -r $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/default-web $(1)$(PKG_LIB_DIR)/lib/
	$(CP) $(PKG_STAGE_DIR)/CONTROL/* $(1)/CONTROL
endef

define Package/ejscgi/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_BIN) $(PKG_STAGE_DIR)/usr/bin/ejscgi $(1)$(PKG_LIB_DIR)/bin/
	$(CP) $(PKG_STAGE_DIR)/CONTROL/* $(1)/CONTROL
endef

define Package/ejswebserver/install
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_DIR) $(1)$(PKG_LIB_DIR)/lib/
	$(INSTALL_BIN) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/bin/ejswebserver $(1)$(PKG_LIB_DIR)/bin/
	$(INSTALL_DATA) $(PKG_STAGE_DIR)$(PKG_LIB_DIR)/lib/{mime.types,ejswebserver.conf} $(1)$(PKG_LIB_DIR)/lib/
	$(CP) $(PKG_STAGE_DIR)/CONTROL/* $(1)/CONTROL
endef

$(eval $(call BuildPackage,libejs))
$(eval $(call BuildPackage,ejs))
$(eval $(call BuildPackage,ejsc))
$(eval $(call BuildPackage,ejsweb))
$(eval $(call BuildPackage,ejscgi))
$(eval $(call BuildPackage,ejswebserver))
$(eval $(call BuildPackage,ejstools))

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4
#
