#
#	Makefile for Embedded Javascript (EJS) benchmarks
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include 		.makedep

benchExtra:
	$(call log) "[Prep]" "ejsc --optimize 9 --bind bench.es"
	$(call setlibpath) ; $(BLD_BIN_DIR)/ejsc --optimize 9 --bind bench.es 
ifeq	($(BLD_BUILD_OS),WIN)
	(sleep 25 ; pid=`ps -ef | grep ejsvm | awk '{print $$2}'` ; \
	size=`cat /proc/$$pid/status | grep RSS | awk '{print $$2}'` ; echo -e "\nMemory Size $${size}K\n" ) &
endif
ifeq	($(BLD_BUILD_OS),MACOSX)
	(sleep 20 ; size=`ps -eo rss,args | egrep -v 'grep|bash' | grep 'default.mod' | awk '{print $$1}'`; \
		echo -e '\nMemory Size' $$size K )&
endif
ifeq	($(BLD_BUILD_OS),LINUX)
	(sleep 25 ; size=`ps -eo rss,args | grep 'ejsvm default.mod' | grep -v grep`; \
		echo -e '\nMemory Size' $$size | sed 's/ \.\..*/ K\n/' )&
endif
	$(call log) "[Test]" "ejsvm default.mod"
	@$(call setlibpath) ; $(BLD_BIN_DIR)/ejsvm default.mod

run:
	$(call log) "[Prep]" "ejsc --optimize 9 --bind bench.es"
	$(call setlibpath) ; $(BLD_BIN_DIR)/ejsc --optimize 9 --bind bench.es 
	(sleep 20 ; size=`ps -eo rss,args | grep 'default.mod' | egrep -v 'grep|export'`; \
		echo -e '\nMemory Size' $$size | sed 's/ default.mod/ K/' )&
	$(call log) "[Test]" "ejsvm default.mod" 
	$(call setlibpath) ; $(BLD_BIN_DIR)/ejsvm default.mod | tee ~/a.csv
