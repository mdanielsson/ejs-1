#
#	RPM spec file for !!BLD_NAME!!
#
Summary: !!BLD_NAME!! -- ECMAScript (Javascript) 4.X Language
Name: !!BLD_PRODUCT!!
Version: !!BLD_VERSION!!
Release: !!BLD_NUMBER_ONLY!!
License: Dual GPL/commercial
Group: Development/Other
URL: http://www.ejscript.com
Distribution: Embedthis
Vendor: Embedthis Software
BuildRoot: !!ROOT_DIR!!/RPM/BUILDROOT/!!BLD_PRODUCT!!-!!BLD_VERSION!!-!!BLD_NUMBER!!.!!BLD_CPU!!
AutoReqProv: no

%description
Embedthis Ejscript is an enhanced, embeddable implementation of JavaScript

%prep

%build

%install
    mkdir -p !!ROOT_DIR!!/RPM/BUILDROOT/!!BLD_PRODUCT!!-!!BLD_VERSION!!-!!BLD_NUMBER_ONLY!!.!!BLD_CPU!!
    for dir in BIN DEV SRC ; do
        cp -r !!ROOT_DIR!!/${dir}/*  !!ROOT_DIR!!/RPM/BUILDROOT/!!BLD_PRODUCT!!-!!BLD_VERSION!!-!!BLD_NUMBER_ONLY!!.!!BLD_CPU!!
    done

%clean

%files -f binFiles.txt

%post
if [ -x /usr/bin/chcon ] ; then 
	sestatus | grep enabled >/dev/null 2>&1
	if [ $? = 0 ] ; then
		for f in !!ORIG_BLD_LIB_PREFIX!!/*.so ; do
			chcon /usr/bin/chcon -t texrel_shlib_t $f
		done
	fi
fi
!!ORIG_BLD_BIN_PREFIX!!/linkup Install /
ldconfig -n !!ORIG_BLD_LIB_PREFIX!!

%preun
rm -f !!ORIG_BLD_PRD_PREFIX!!/latest
!!ORIG_BLD_PRD_PREFIX!!/linkup Remove /

%postun

#
#	Dev package
#
%package dev
Summary: !!BLD_NAME!!  -- Development headers and libraries for !!BLD_NAME!!
Group: Applications/Internet
Prefix: !!ORIG_BLD_INC_PREFIX!!

%description dev
Development headers for the !!BLD_NAME!!

%files dev -f devFiles.txt

#
#	Source package
#
%package src
Summary: !!BLD_NAME!!  -- Source code for !!BLD_NAME!! 
Group: Applications/Internet
Prefix: !!ORIG_BLD_SRC_PREFIX!!

%description src
Source code for the !!BLD_NAME!!

%files src -f srcFiles.txt
