#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

QT4LIBS=/usr/lib/x86_64-linux-gnu
echo "Setting QT4LIBS=$QT4LIBS"

echo build liteide
echo QT4LIBS=$QT4LIBS
echo GOROOT=$GOROOT
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $QT4LIBS ]; then
	echo 'error, QT4LIBS is null'
	exit 1
fi

cp -a -v $QT4LIBS/libQtCore.so* liteide/lib/liteide
cp -a -v $QT4LIBS/libQtGui.so* liteide/lib/liteide
cp -a -v $QT4LIBS/libQtXml.so* liteide/lib/liteide

cp -a -v $QT4LIBS/libQtNetwork.so* liteide/lib/liteide
cp -a -v $QT4LIBS/libQtWebKit.so* liteide/lib/liteide
