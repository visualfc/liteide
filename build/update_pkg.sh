#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

echo GOROOT=$GOROOT
echo BUILD_ROOT=$BUILD_ROOT
echo LITEIDE_ROOT=$LITEIDE_ROOT
echo .

go version
if [ $? -ge 1 ]; then
	echo 'error, not find go in PATH'
	exit 1
fi

echo update liteide tools ...
cd $LITEIDE_ROOT

export GOBIN=$PWD/bin

echo install gocode ...
go install -v github.com/visualfc/gocode@latest
echo install gotools ...
go install -v github.com/visualfc/gotools@latest
echo install gomodifytags ...
go install -v github.com/fatih/gomodifytags@latest

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

cd $BUILD_ROOT
