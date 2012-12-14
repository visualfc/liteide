#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

echo build liteide 
echo QTDIR=$QTDIR
echo GOROOT=$GOROOT
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $QTDIR ]; then
	echo 'error, QTDIR is null'
	exit 1
fi

export PATH=$QTDIR/bin:$PATH

cp -a -v $QTDIR/lib/libphonon.so* liteide/lib/liteide
cp -a -v $QTDIR/lib/QtNetwork.so* liteide/lib/liteide
cp -a -v $QTDIR/libQtWebkit.so* liteide/lib/liteide
