#!/bin/bash

ADL_REVISION=881fa21e177203a30eb498d43bcbbec3d9b18d37
ADL_SDK_DIR=$PWD/thirdparty/adl-sdk

IGCL_REVISION=e24a87d6fb7856f7dca1f099bff180f03a2ed097
IGCL_SDK_DIR=$PWD/thirdparty/igcl

[ -d thirdparty ] || mkdir thirdparty

pushd $ADL_SDK_DIR
if [ -n "$(find $ADL_SDK_DIR -prune -empty 2>/dev/null)" ]; then
    git fetch origin
else
    [ -d $ADL_SDK_DIR ] || mkdir $ADL_SDK_DIR
    git clone git@github.com:GPUOpen-LibrariesAndSDKs/display-library.git $ADL_SDK_DIR
fi
git checkout $ADL_REVISION
git submodule update --init
rm -rf Public-Documents 2> /dev/null
popd

pushd $IGCL_SDK_DIR
if [ -n "$(find $IGCL_SDK_DIR -prune -empty 2>/dev/null)" ]; then
    git fetch origin
else
    [ -d $IGCL_SDK_DIR ] || mkdir $IGCL_SDK_DIR
    git clone git@github.com:intel/drivers.gpu.control-library.git $IGCL_SDK_DIR
fi
git checkout $IGCL_REVISION
git submodule update --init
popd

if [ -x "$(command -v wslpath)" ]; then
    getpath=(wslpath)
elif [ -x "$(command -v cygpath)" ]; then
    getpath=(cygpath)
fi
getpath+=(-u)

RTSS_PATH="C:\Program Files (x86)\RivaTuner Statistics Server"
MSIAB_PATH="C:\Program Files (x86)\MSI Afterburner"
RTSS_PATH=$("${getpath[@]}" "$RTSS_PATH")

RTSS_SDK_DIR=$PWD/thirdparty/rtss-sdk
RTSS_OE_PLUGIN_DIR=$PWD/thirdparty/RTSSOverlayEditor

# If the user has RTSS SDK installed, copy over what we need in order to compile.
RTSS_PATH=$RTSS_PATH/SDK
if [ -d "$RTSS_PATH" ]; then
    [ -d $RTSS_SDK_DIR ] || mkdir $RTSS_SDK_DIR
    cp -r "$RTSS_PATH/Doc" "$RTSS_PATH/Include" "$RTSS_PATH/Samples" $RTSS_SDK_DIR 2>/dev/null
fi
