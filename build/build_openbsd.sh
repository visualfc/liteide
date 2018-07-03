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

echo qmake liteide ...
echo .
qmake $LITEIDE_ROOT -spec openbsd-g++ "CONFIG+=release"

if [ $? -ge 1 ]; then
	echo 'error, qmake fail'
	exit 1
fi

echo make liteide ...
echo .
make

if [ $? -ge 1 ]; then
	echo 'error, make fail'
	exit 1
fi

go version
if [ $? -ge 1 ]; then
	echo 'error, not find go in PATH'
	exit 1
fi

echo build liteide tools ...
cd $LITEIDE_ROOT


if [ -z $GOPATH ]; then
	export GOPATH=$PWD
else
	export GOPATH=$PWD:$GOPATH
fi


go install -ldflags "-s" -v github.com/visualfc/gotools

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

go install -ldflags "-s" -v github.com/mdempsky/gocode

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

go install -ldflags "-s" -v github.com/fatih/gomodifytags

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi


echo export qrc images
go run src/tools/exportqrc/main.go -root .

if [ $? -ge 1 ]; then
	echo 'error, go run fail'
	exit 1
fi

echo deploy ...

cd $BUILD_ROOT

rm -r liteide
mkdir -p liteide
mkdir -p liteide/local/bin
mkdir -p liteide/local/share/liteide
mkdir -p liteide/local/share/pixmaps
mkdir -p liteide/local/lib/liteide/plugins

cp -RpP $LITEIDE_ROOT/LICENSE.LGPL liteide
cp -RpP $LITEIDE_ROOT/LGPL_EXCEPTION.TXT liteide
cp -RpP $LITEIDE_ROOT/../README.md liteide
cp -RpP $LITEIDE_ROOT/../CONTRIBUTORS liteide

cp $LITEIDE_ROOT/liteide/bin/liteide liteide/local/bin
cp $LITEIDE_ROOT/liteide/bin/*.so.* liteide/local/lib
cp -RpP $LITEIDE_ROOT/bin/gotools liteide/local/bin
cp -RpP $LITEIDE_ROOT/bin/gocode liteide/local/bin
cp -RpP $LITEIDE_ROOT/bin/gomodifytags liteide/local/bin
cp -RpP $LITEIDE_ROOT/liteide/lib/liteide/plugins/*.so liteide/local/lib/liteide/plugins

cp -r $LITEIDE_ROOT/deploy/* liteide/local/share/liteide/
cp -r $LITEIDE_ROOT/os_deploy/openbsd/* liteide/local/share/liteide/
cp $LITEIDE_ROOT/deploy/welcome/images/liteide-logo*.png liteide/local/share/pixmaps

