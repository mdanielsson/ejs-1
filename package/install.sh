#!/bin/bash
#
#   install: Installation script
#
#   Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#
#   Usage: install [configFile]
#
################################################################################
#
#   The configFile is of the format:
#       FMT=[rpm|deb|tar]               # Package format to use
#       srcDir=sourcePath               # Where to install the src
#       devDir=documentationPath        # Where to install the doc
#       installbin=[YN]                 # Install binary package
#       installdev=[YN]                 # Install dev headers package
#

HOME=`pwd`
FMT=

HOSTNAME=`hostname`
BLD_PRODUCT="!!BLD_PRODUCT!!"
BLD_NAME="!!BLD_NAME!!"
BLD_VERSION="!!BLD_VERSION!!"
BLD_NUMBER="!!BLD_NUMBER!!"
BLD_HOST_OS="!!BLD_HOST_OS!!"
BLD_HOST_CPU="!!BLD_HOST_CPU!!"
BLD_HOST_DIST="!!BLD_HOST_DIST!!"

BLD_PREFIX="!!ORIG_BLD_PREFIX!!"
BLD_BIN_PREFIX="!!ORIG_BLD_BIN_PREFIX!!"
BLD_CFG_PREFIX="!!ORIG_BLD_CFG_PREFIX!!"
BLD_DOC_PREFIX="!!ORIG_BLD_DOC_PREFIX!!"
BLD_INC_PREFIX="!!ORIG_BLD_INC_PREFIX!!"
BLD_LIB_PREFIX="!!ORIG_BLD_LIB_PREFIX!!"
BLD_MAN_PREFIX="!!ORIG_BLD_MAN_PREFIX!!"
BLD_MOD_PREFIX="!!ORIG_BLD_MOD_PREFIX!!"
BLD_PRD_PREFIX="!!ORIG_BLD_PRD_PREFIX!!"
BLD_SAM_PREFIX="!!ORIG_BLD_SAM_PREFIX!!"
BLD_SRC_PREFIX="!!ORIG_BLD_SRC_PREFIX!!"
BLD_VER_PREFIX="!!ORIG_BLD_VER_PREFIX!!"
BLD_WEB_PREFIX="!!ORIG_BLD_WEB_PREFIX!!"

installbin=Y
installdev=Y

PATH=$PATH:/sbin:/usr/sbin

###############################################################################

setup() {

    umask 022

    if [ $BLD_HOST_OS != WIN -a `id -u` != "0" ] ; then
        echo "You must be root to install this product."
        exit 255
    fi

    #
    #   Headless install
    #
    if [ $# -ge 1 ] ; then
        if [ ! -f $1 ] ; then
            echo "Could not find installation config file \"$1\"." 1>&2
            exit 255
        else
            . $1 
            installFiles $FMT
        fi
        exit 0
    fi
    sleuthPackageFormat
    echo -e "\n$BLD_NAME !!BLD_VERSION!!-!!BLD_NUMBER!! Installation\n"

}

#
#   Try to guess if we should default to using RPM
#
sleuthPackageFormat() {
    local name

    name=`createPackageName ${BLD_PRODUCT}-bin`
    FMT=
    for f in deb rpm tar.gz ; do
        if [ -f ${name}.${f} ] ; then
            FMT=${f%.gz}
            break
        fi
    done
    if [ "$FMT" = "" ] ; then
        echo -e "\nYou may be be missing a necessary package file. "
        echo "Check that you have the correct $BLD_NAME package".
        exit 255
    fi
}

askUser() {
    local finished

    #
    #   Confirm the configuration
    #
    finished=N
    while [ "$finished" = "N" ]
    do
        installbin=`yesno "Install binary package" "$installbin"`
        installdev=`yesno "Install development headers and samples package" "$installdev"`
    
        echo -e "\nInstalling with this configuration:" 
        echo -e "    Install binary package: $installbin"
        echo -e "    Install development doc, headers and samples package: $installdev"
        echo
        finished=`yesno "Accept this configuration" "Y"`
    done
    
    if [ "$installbin" = "N" -a "$installdev" = "N" ] ; then
        echo -e "\nNothing to install, exiting. "
        exit 0
    fi
    
    #
    #   Save the install settings. Remove.sh will need this
    #
    saveSetup
}

createPackageName() {
    echo ${1}-${BLD_VERSION}-${BLD_NUMBER}-${BLD_HOST_DIST}-${BLD_HOST_OS}-${BLD_HOST_CPU}
}

# 
#   Get a yes/no answer from the user. Usage: ans=`yesno "prompt" "default"`
#   Echos 1 for Y or 0 for N
#
yesno() {
    local ans

    if [ "$!!BLD_PRODUCT!!_HEADLESS" = 1 ] ; then
        echo "Y"
        return
    fi
    echo -n "$1 [$2] : " 1>&2
    while [ 1 ] 
    do
        read ans
        if [ "$ans" = "" ] ; then
            echo $2 ; break
        elif [ "$ans" = "Y" -o "$ans" = "y" ] ; then
            echo "Y" ; break
        elif [ "$ans" = "N" -o "$ans" = "n" ] ; then
            echo "N" ; break
        fi
        echo -e "\nMust enter a 'y' or 'n'\n " 1>&1
    done
}

# 
#   Get input from the user. Usage: ans=`ask "prompt" "default"`
#   Returns the answer or default if <ENTER> is pressed
#
ask() {
    local ans

    default=$2

    if [ "$!!BLD_PRODUCT!!_HEADLESS" = 1 ] ; then
        echo "$default"
        return
    fi

    echo -n "$1 [$default] : " 1>&2
    read ans
    if [ "$ans" = "" ] ; then
        echo $default
    fi
    echo $ans
}

saveSetup() {
    local firstChar

    mkdir -p "$BLD_VER_PREFIX"
    echo -e "FMT=$FMT\nbinDir=$BLD_VER_PREFIX\ninstallbin=$installbin\ninstalldev=$installdev" \
        >"${BLD_VER_PREFIX}/install.conf"
}

installFiles() {
    local dir pkg doins NAME upper

    echo -e "\nExtracting files ...\n"

    for pkg in bin dev ; do
        
        doins=`eval echo \\$install${pkg}`
        if [ "$doins" = Y ] ; then
            suffix="-${pkg}"
            #
            #   RPM doesn't give enough control on error codes. So best to keep going.  
            #
            NAME=`createPackageName ${BLD_PRODUCT}${suffix}`.$FMT
            if [ "$FMT" = "rpm" ] ; then
                echo -e "rpm -Uhv $NAME"
                rpm -Uhv $HOME/$NAME
            elif [ "$FMT" = "deb" ] ; then
                echo -e "dpkg -i $NAME"
                dpkg -i $HOME/$NAME >/dev/null
            else
                echo tar xfz "$HOME/${NAME}.gz" --strip-components 1 -P -C /
                tar xfz "$HOME/${NAME}.gz" --strip-components 1 -P -C /
            fi
        fi
    done

    if [ -f /etc/redhat-release -a -x /usr/bin/chcon ] ; then 
        sestatus | grep enabled >/dev/null 2>&1
        if [ $? = 0 ] ; then
            for f in $BLD_LIB_PREFIX/*.so ; do
                chcon /usr/bin/chcon -t texrel_shlib_t $f 2>&1 >/dev/null
            done
        fi
    fi

    if [ "$BLD_HOST_OS" = "FREEBSD" ] ; then
        LDCONFIG_OPT=-m
    else
        LDCONFIG_OPT=-n
    fi
    if which ldconfig >/dev/null 2>&1 ; then
        ldconfig $LDCONFIG_OPT $BLD_LIB_PREFIX
    fi
    "$BLD_BIN_PREFIX/linkup" Install

    if [ $BLD_HOST_OS = WIN ] ; then
        echo -e "\nSetting file permissions ..."
        find "$BLD_CFG_PREFIX" -type d -exec chmod 755 {} \;
        find "$BLD_CFG_PREFIX" -type f -exec chmod g+r,o+r {} \;
        chmod 755 "$BLD_BIN_PREFIX"/*.dll "$BLD_BIN_PREFIX"/*.exe
    fi
    echo
}

#
#   Cleanup for 0.9.6 and before
#
legacyPrep() {
    rm -f /usr/bin/ejs.mod
    rm -f /usr/bin/ejs.db.mod
    rm -f /usr/bin/ejs.web.mod
    rm -f /usr/bin/ejsweb.mod

    rm -f /usr/bin/egen
    rm -f /usr/bin/ec
    rm -f /usr/bin/ecgi
    rm -f /usr/bin/ejs
    rm -f /usr/bin/ejsc
    rm -f /usr/bin/ejscgi
    rm -f /usr/bin/ejsmod
    rm -f /usr/bin/ejssql
    rm -f /usr/bin/ejsvm
    rm -f /usr/bin/ejsweb

    rm -f /usr/lib/ejs/ejs.mod
    rm -f /usr/lib/ejs/ejs.db.mod
    rm -f /usr/lib/ejs/ejs.web.mod
    rm -f /usr/lib/ejs/ejsweb.mod
}

startBrowser() {
    local url

    if [ "$!!BLD_PRODUCT!!_HEADLESS" = 1 ] ; then
        return
    fi
    #
    #   Conservative delay to allow appweb to start and initialize
    #
    sleep 5
    echo -e "\nStarting browser to view the $BLD_NAME Home Page."
    url=$BLD_DOC_PREFIX/index.html
    if [ $BLD_HOST_OS = WIN ] ; then
        cygstart --shownormal "$url"
    elif [ $BLD_HOST_OS = MACOSX ] ; then
        open $url
    else
        for f in /usr/bin/htmlview /usr/bin/firefox /usr/bin/mozilla /usr/bin/konqueror
        do
            if [ -x ${f} ] ; then
                sudo -H -b ${f} "$url" >/dev/null 2>&1 &
                break
            fi
        done
    fi
}

###############################################################################
#
#   Main program for install script
#

setup $*
askUser
legacyPrep
installFiles $FMT
startBrowser

echo -e "\n$BLD_NAME installation successful.\n"
