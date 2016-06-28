#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

echo GOROOT=$GOROOT
echo QTDIR=$QTDIR
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

if [ -z $QTDIR ]; then
	echo 'error, QTDIR is null'
	exit 1
fi

echo update translator files

lupdate -no-obsolete $LITEIDE_ROOT/liteidex.pro
