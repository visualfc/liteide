@echo off

setlocal

set BUILD_ROOT=%CD%
if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex

echo build liteide 
echo QTDIR=%QTDIR%
echo .

if x%QTDIR%==x goto qtdir_fail

windeployqt --dir liteide/bin liteide/lib/liteide/plugins/webkithtmlwidget.dll

windeployqt --dir liteide/bin liteide/lib/liteide/plugins/liteeditor.dll

windeployqt --dir liteide/bin liteide/lib/liteide/plugins/welcome.dll

windeployqt --dir liteide/bin liteide/lib/liteide/plugins/litebuild.dll

windeployqt --dir liteide/bin liteide/bin/liteapp.dll



goto end

:qtdir_fail
echo error, QTDIR is null
goto end

:end
