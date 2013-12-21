#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

DEBIANQTLIBS=/usr/lib/x86_64-linux-gnu
echo "Setting DEBIANQTLIBS=$DEBIANQTLIBS"

echo build liteide 
echo DEBIANQTLIBS=$DEBIANQTLIBS
echo GOROOT=$GOROOT
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $DEBIANQTLIBS ]; then
	echo 'error, DEBIANQTLIBS is null'
	exit 1
fi

cp -a -v $DEBIANQTLIBS/libQtCore.so* liteide/lib/liteide
cp -a -v $DEBIANQTLIBS/libQtGui.so* liteide/lib/liteide
cp -a -v $DEBIANQTLIBS/libQtXml.so* liteide/lib/liteide

cp -a -v $DEBIANQTLIBS/libQtNetwork.so* liteide/lib/liteide
cp -a -v $DEBIANQTLIBS/libQtWebKit.so* liteide/lib/liteide
