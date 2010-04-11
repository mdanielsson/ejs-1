#
#	Makefile -- Top level Makefile for Ejscript
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#
#
#	Standard Make targets supported are:
#	
#		make 						# Does a "make compile"
#		make clean					# Removes generated objects
#		make compile				# Compiles the source
#		make depend					# Generates the make dependencies
#		make test 					# Runs unit tests
#		make leakTest 				# Runs memory leak tests
#		make loadTest 				# Runs load tests
#		make benchmark 				# Runs benchmarks
#		make package				# Creates an installable package
#
#	Installation targets. Use "make ROOT_DIR=myDir" to do a custom local install:
#
#		make install				# Call install-binary + install-dev
#		make install-binary			# Install binary files
#		make install-dev			# Install development libraries and headers
#
#	To remove, use make uninstall-ITEM, where ITEM is a component above.

include		build/make/Makefile.top

dependExtra:
	@[ ! -L extensions ] && ln -s ../packages extensions ; true

diff import sync:
	@if [ ! -x $(BLD_TOOLS_DIR)/edep$(BLD_BUILD_EXE) -a "$(BUILDING_CROSS)" != 1 ] ; then \
		$(MAKE) -S --no-print-directory _RECURSIVE_=1 -C $(BLD_TOP)/build/src compile ; \
	fi
	@import.ksh --$@ --src ../tools.stable --dir . ../tools.stable/build/export/export.gen
	@import.ksh --$@ --src ../tools.stable --dir . ../tools.stable/build/export/export.configure
	@import.ksh --$@ --src ../mpr.stable 	--dir . ../mpr.stable/build/export/export.gen
	@import.ksh --$@ --src ../mpr.stable 	--dir ./src/include --strip ./all/ ../mpr.stable/build/export/export.h
	@import.ksh --$@ --src ../mpr.stable 	--dir ./src/mpr --strip ./all/ ../mpr.stable/build/export/export.c
	@import.ksh --$@ --src ../appweb.stable --dir ./src/include --strip ./all/ ../appweb.stable/build/export/export.h
	@import.ksh --$@ --src ../appweb.stable --dir ./src/appweb --strip ./all/ ../appweb.stable/build/export/export.c
	@echo

#
#	Convenient configure targets
#
config:
	$(call log) "[Config]" "configure"
	./configure

release:
	$(call log) "[Config]" "configure --defaults=release --tune=size"
	./configure --defaults=release --tune=size

cross64:
	./configure --host=x86_64-apple-darwin --without-ssl --enable-complete-cross

cross-ppc:
	./configure --host=ppc-apple-darwin --without-ssl

config32:
	./configure --host=i386-apple-darwin --build=i386-apple-darwin --without-ssl 

config64:
	./configure --host=x86_64-apple-darwin --build=x86_64-apple-darwin

rom:
	./configure --host=i686-apple-darwin --build=x86_64-apple-darwin --rom --static --without-ssl 

external:
	./configure --with-mpr=../mpr --with-sqlite=/usr --with-openssl --with-matrixssl

small:
	./configure --disable-all --tune=size --type=release --enable-shared --number=int 

speed:
	./configure --type=release --disable-assert --tune=size

vx:
	unset WIND_HOME WIND_BASE ; \
	SEARCH_PATH=/tornado ./configure --host=i386-wrs-vxworks --enable-all --without-ssl

vx5:
	unset WIND_HOME WIND_BASE ; \
	SEARCH_PATH=/tornado ./configure --host=i386-wrs-vxworks --enable-all --without-ssl

vx6:
	unset WIND_HOME WIND_BASE ; \
	./configure --host=pentium-wrs-vxworks --enable-all --without-ssl

#
#	Sample for cross compilation
#
vx5env:
	ARCH=386 ; \
	WIND_HOME=c:/tornado ; \
	WIND_BASE=$$WIND_HOME ; \
	WIND_GNU_PATH=$$WIND_BASE/host ; \
	AR=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/ar$${ARCH}.exe \
	CC=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/cc$${ARCH}.exe \
	LD=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/ld$${ARCH}.exe \
	NM=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/nm$${ARCH}.exe \
	RANLIB=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/ranlib$${ARCH}.exe \
	STRIP=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/strip$${ARCH}.exe \
	IFLAGS="-I$$WIND_BASE/target/h -I$$WIND_BASE/target/h/wrn/coreip" \
	SEARCH_PATH=/tornado ./configure --host=i386-wrs-vxworks --enable-all --without-ssl

vx6env:
	ARCH=pentium ; \
	WIND_HOME=c:/WindRiver ; \
	VXWORKS=vxworks-6.3 ; \
	WIND_BASE=$$WIND_HOME/$$VXWORKS ; \
	PLATFORM=i586-wrs-vxworks ; \
	WIND_GNU_PATH=$$WIND_HOME/gnu/3.4.4-vxworks-6.3 ; \
	AR=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/ar$${ARCH}.exe \
	CC=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/cc$${ARCH}.exe \
	LD=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/bin/cc$${ARCH}.exe \
	NM=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/$${PLATFORM}/bin/nm.exe \
	RANLIB=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/$${PLATFORM}/bin/ranlib.exe \
	STRIP=$$WIND_GNU_PATH/$$WIND_HOST_TYPE/$${PLATFORM}/bin/strip.exe \
	CFLAGS="-I$$WIND_BASE/target/h -I$$WIND_BASE/target/h/wrn/coreip" \
	./configure --host=i386-wrs-vxworks --enable-all --without-ssl

cygwin:
	./configure --cygwin

wince:
	./configure --host=arm-ms-wince --without-ssl

vxenv:
	wrenv -p vxworks-6.3 -f sh -o print_env

#
#	Build with the uclibc small C library.
#	If using ubuntu, then install the  packages: uclibc-toolchain, libuclibc-dev
#	Use dpkg -L package to see installed files. Typically installed under /usr/i386-uclibc-linux
#
uclibc:
	CFLAGS="-nostdlibs -fno-stack-protector" \
	IFLAGS="-I/usr/i386-uclibc-linux/include" \
	LDFLAGS="-nodefaultlibs -nostartfiles /usr/i386-uclibc-linux/lib/crt1.o /usr/i386-uclibc-linux/lib/crti.o /usr/i386-uclibc-linux/lib/crtn.o /usr/i386-uclibc-linux/lib/libc.a /usr/lib/gcc/i486-linux-gnu/4.1.2/libgcc.a" \
	./configure --type=debug --tune=size

uclibc-detailed:
	PREFIX=i386-uclibc-linux; \
	DIR=/usr/i386-uclibc-linux/bin ; \
	AR=$${DIR}/$${PREFIX}-ar \
	CC=$${DIR}/$${PREFIX}-gcc \
	LD=$${DIR}/$${PREFIX}-gcc \
	NM=$${DIR}/$${PREFIX}-nm \
	RANLIB=$${DIR}/$${PREFIX}-ranlib \
	STRIP=$${DIR}/$${PREFIX}-strip \
	CFLAGS="-fno-stack-protector" \
	CXXFLAGS="-fno-rtti -fno-exceptions" \
	BUILD_CC=/usr/bin/cc \
	BUILD_LD=/usr/bin/cc \
	./configure --host=i386-pc-linux --type=RELEASE --tune=size --disable-assert

#
#	Compute lines of code
#
lines:
	@echo -n "Lines of C: "
	@find . -name '*.c' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "Lines of C Headers: "
	@find . -name '*.h' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "Lines of Java: "
	@find . -name '*.java' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "Lines of Ejscript: "
	@find . -name '*.es' -o -name '*.as' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "MPR: "
	@find src/mpr -name '*.c' -o -name '*.h' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "Tools: "
	@find build -name '*.c' -o -name '*.h' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "C VM: "
	@find src/vm -name '*.c' -o -name '*.h' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "C Types: "
	@find src/types -name '*.c' -o -name '*.h' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "Compiler: "
	@find src/compiler -name '*.c' -o -name '*.h' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l
	@echo -n "Total: "
	@find . -name '*.c' -o -name '*.h' -o -name '*.java' -o -name '*.es' \
		-o -name '*.as' | egrep -v '\.hg|OLD|sav' | xargs cat | wc -l

#
#	Profile guided optimization build
#
pgo:
	rm -f obj/*.gcda obj/*.gcno
	$(MAKE) clean
	$(MAKE) -C build/bin compile 
	$(MAKE) depend
	$(MAKE) PGO=-fprofile-generate clean compile
	$(MAKE) -C src/test/bench src/test
	$(MAKE) PGO=-fprofile-use compile
	$(MAKE) -C src/test/bench src/test
	rm -f obj/*.gcda obj/*.gcno

redo:
	hg pull ; hg update -C
	make TRACE=0 release clean depend compile install-binary test

update:
	hg pull ; hg update -C
	make TRACE=0 clean depend compile install-binary test
