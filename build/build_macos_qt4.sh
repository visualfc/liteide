#!/bin/sh

export BUILD_ROOT=$PWD

if [ -z $LITEIDE_ROOT ]; then
	export LITEIDE_ROOT=$PWD/../liteidex
fi

echo build liteide
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
qmake $LITEIDE_ROOT -spec macx-g++ "CONFIG+=release"

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

go install -ldflags "-s" -v github.com/visualfc/gocode

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

cp -R -v $LITEIDE_ROOT/liteide/bin/LiteIDE.app liteide
rm liteide/LiteIDE.app/Contents/PlugIns/*.a

cp -R -v $LITEIDE_ROOT/LICENSE.LGPL liteide
cp -R -v $LITEIDE_ROOT/LGPL_EXCEPTION.TXT liteide
cp -R -v $LITEIDE_ROOT/../README.md liteide
cp -R -v $LITEIDE_ROOT/../CONTRIBUTORS liteide

cp -R -v $LITEIDE_ROOT/bin/gotools liteide/LiteIDE.app/Contents/MacOS
cp -R -v $LITEIDE_ROOT/bin/gocode liteide/LiteIDE.app/Contents/MacOS
cp -R -v $LITEIDE_ROOT/bin/gomodifytags liteide/LiteIDE.app/Contents/MacOS
cp -R -v $LITEIDE_ROOT/deploy/* liteide/LiteIDE.app/Contents/Resources
cp -R -v $LITEIDE_ROOT/os_deploy/macosx/* liteide/LiteIDE.app/Contents/Resources

