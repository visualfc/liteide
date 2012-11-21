#!/bin/sh
cp -v LICENSE.LGPL liteide/bin
cp -v LGPL_EXCEPTION.TXT liteide/bin
cp -v README.TXT liteide/bin
cp -r -v deploy/* liteide/bin/LiteIDE.app/Contents/Resources
cp -r -v os_deploy/macosx/* liteide/bin/LiteIDE.app/Contents/Resources

#cp -v liteide/bin/goastview liteide/bin/LiteIDE.app/Contents/MacOS
#cp -v liteide/bin/goexec liteide/bin/LiteIDE.app/Contents/MacOS
#cp -v liteide/bin/gopromake liteide/bin/LiteIDE.app/Contents/MacOS
#cp -v liteide/bin/godocview liteide/bin/LiteIDE.app/Contents/MacOS
cp -v bin/* liteide/bin/LiteIDE.app/Contents/MacOS
cp -v src/liteapp/Info.plist liteide/bin/LiteIDE.app/Contents

export QTLIBPATH=/Users/win32/QtSDK/Desktop/Qt/474/gcc/lib

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
