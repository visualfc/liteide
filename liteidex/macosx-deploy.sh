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

function process_file()
{
 echo "install_name_tool" $deploy_file
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/bin/LiteIDE.app/Contents/PlugIns/$deploy_file  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/bin/LiteIDE.app/Contents/PlugIns/$deploy_file   
}

function process_file2()
{
  echo "install_name_tool" $deploy_file
install_name_tool -change \
 $QTLIBPATH/QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide/bin/LiteIDE.app/Contents/PlugIns/$deploy_file  

install_name_tool -change \
 $QTLIBPATH/QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide/bin/LiteIDE.app/Contents/PlugIns/$deploy_file   
  
install_name_tool -change \
 $QTLIBPATH/QtXml.framework/Versions/4/QtXml \
 @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
  liteide/bin/LiteIDE.app/Contents/PlugIns/$deploy_file   
}

export deploy_file=libgolangast.dylib
process_file

export deploy_file=libgolangfmt.dylib
process_file

export deploy_file=liblitebuild.dylib
process_file

export deploy_file=libliteeditor.dylib
process_file2

export deploy_file=libmakefileproject.dylib
process_file

export deploy_file=libprofileproject.dylib
process_file

export deploy_file=libwelcome.dylib
process_file2

export deploy_file=libgolangcode.dylib
process_file

export deploy_file=libfilebrowser.dylib
process_file

export deploy_file=liblitefind.dylib
process_file

export deploy_file=libgolangdoc.dylib
process_file2

export deploy_file=libliteenv.dylib
process_file

export deploy_file=liblitedebug.dylib
process_file

export deploy_file=libgdbdebugger.dylib
process_file

export deploy_file=libgolangplay.dylib
process_file

export deploy_file=libgolangpackage.dylib
process_file
