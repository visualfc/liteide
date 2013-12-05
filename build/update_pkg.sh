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
export GOPATH=$PWD

echo get goimports ...
go get -v -u "github.com/visualfc/goimports"
echo get gocode ...
go get -v -u "github.com/nsf/gocode"
echo get gopm ...
go get -v -u "github.com/gpmgo/gopm"

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

cd $BUILD_ROOT
