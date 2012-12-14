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
export GOPATH=$PWD:$GOPATH

go install -ldflags "-s" -v tools/goastview
go install -ldflags "-s" -v tools/godocview
go install -ldflags "-s" -v tools/goexec
go install -ldflags "-s" -v tools/goapi

if [ $? -ge 1 ]; then
	echo 'error, go install fail'
	exit 1
fi

echo deploy ...

cd $BUILD_ROOT

rm -r liteide
mkdir -p liteide

cp -r -v $LITEIDE_ROOT/liteide/bin/LiteIDE.app liteide
rm liteide/LiteIDE.app/Contents/PlugIns/*.a

cp -v $LITEIDE_ROOT/LICENSE.LGPL liteide
cp -v $LITEIDE_ROOT/LGPL_EXCEPTION.TXT liteide
cp -v $LITEIDE_ROOT/README.TXT liteide

cp $LITEIDE_ROOT/bin/* liteide/LiteIDE.app/Contents/MacOS
cp -r -v $LITEIDE_ROOT/deploy/* liteide/LiteIDE.app/Contents/Resources
cp -r -v $LITEIDE_ROOT/os_deploy/macosx/* liteide/LiteIDE.app/Contents/Resources

echo .
echo macdeployqt liteide/LiteIDE.app -no-plugins
# macdeployqt liteide/LiteIDE.app -no-plugins

export QTLIBPATH=$QTDIR/lib
echo QTLIBPATH=$QTLIBPATH

function deploy_p1()
{
 echo "install_name_tool" $1
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/PlugIns/$1  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/PlugIns/$1   
if [ $? -ge 1 ]; then
	echo 'error, not deploy'
	exit 1
fi
}

function deploy_p2()
{
  echo "install_name_tool" $1
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/PlugIns/$1  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/PlugIns/$1   
  
install_name_tool -change \
 $QTLIBPATH/QtXml.framework/Versions/4/QtXml \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
  liteide/LiteIDE.app/Contents/PlugIns/$1   
}

function deploy_m2()
{
  echo "install_name_tool" $1
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/MacOS/$1  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/MacOS/$1   
  
install_name_tool -change \
 $QTLIBPATH/QtXml.framework/Versions/4/QtXml \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
  liteide/LiteIDE.app/Contents/MacOS/$1   
}

function deploy_m3()
{
  echo "install_name_tool" $1
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/MacOS/$1  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/MacOS/$1   
  
install_name_tool -change \
 $QTLIBPATH/QtNetwork.framework/Versions/4/QtNetwork \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtNetwork \
  liteide/LiteIDE.app/Contents/MacOS/$1   
}

function deploy_m4()
{
  echo "install_name_tool" $1
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/LiteIDE.app/Contents/MacOS/$1  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/LiteIDE.app/Contents/MacOS/$1   
  
install_name_tool -change \
 $QTLIBPATH/QtNetwork.framework/Versions/4/QtNetwork \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtNetwork \
  liteide/LiteIDE.app/Contents/MacOS/$1   

install_name_tool -change \
 $QTLIBPATH/QttWebkit.framework/Versions/4/QttWebkit \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QttWebkit \
  liteide/LiteIDE.app/Contents/MacOS/$1 
}

deploy_m2 libliteapp.dylib
deploy_m2 LiteIDE

deploy_p1 libgolangast.dylib
deploy_p1 libgolangfmt.dylib
deploy_p1 liblitebuild.dylib
deploy_p2 libliteeditor.dylib
deploy_p2 libwelcome.dylib
deploy_p1 libgolangcode.dylib
deploy_p1 libfilebrowser.dylib
deploy_p1 liblitefind.dylib
deploy_p2 libgolangdoc.dylib
deploy_p1 libliteenv.dylib
deploy_p1 liblitedebug.dylib
deploy_p1 libgdbdebugger.dylib
deploy_p1 libgolangplay.dylib
deploy_p1 libgolangpackage.dylib
deploy_p1 litemarkdown.dylib
deploy_m4 libwebkithtmlwidget.dylib

