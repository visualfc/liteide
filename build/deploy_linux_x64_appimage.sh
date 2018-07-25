#!/bin/sh

mkdir liteide.AppDir
mkdir liteide.AppDir/usr
cp -r liteide/* liteide.AppDir/usr 
linuxdeployqt liteide.AppDir/usr/bin/liteide -bundle-non-qt-libs
cp liteide.desktop liteide.AppDir
cp liteide.png liteide.AppDir
cd liteide.AppDir
ln -s -f usr/bin/liteide AppRun
cd ..
appimagetool-x86_64.AppImage liteide.AppDir
rm -r liteide.AppDir