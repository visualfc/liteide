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

if [ -z $GOPATH ]; then
	export GOPATH=$PWD
else
	export GOPATH=$PWD:$GOPATH
fi

echo get gocode ...
rm -f -r src/github.com/mdempsky/gocode
go get -v -u "github.com/mdempsky/gocode"
echo get gotools ...
rm -f -r src/github.com/visualfc/gotools
go get -v -u "github.com/visualfc/gotools"
rm -f -r src/github.com/fatih/gomodifytags
go get -v -u "github.com/fatih/gomodifytags"

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

cd $BUILD_ROOT
