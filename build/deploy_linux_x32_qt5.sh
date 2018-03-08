#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

QT5LIBS=/usr/lib/i386-linux-gnu
echo "Setting QT5LIBS=$QT5LIBS"

echo build liteide
echo QT5LIBS=$QT5LIBS
echo GOROOT=$GOROOT
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $QT5LIBS ]; then
	echo 'error, QT5LIBS is null'
	exit 1
fi

cp -a -v $QT5LIBS/libQt5Core.so* liteide/lib/liteide
cp -a -v $QT5LIBS/libQt5Gui.so* liteide/lib/liteide
cp -a -v $QT5LIBS/libQt5Widgets.so* liteide/lib/liteide
cp -a -v $QT5LIBS/libQt5Xml.so* liteide/lib/liteide
