@echo off

setlocal

set BUILD_ROOT=%CD%
if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex

echo build liteide 
echo QTDIR=%QTDIR%
echo .

if x%QTDIR%==x goto qtdir_fail

cd %LITEIDE_ROOT%

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/liteeditord.dll

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/welcomed.dll

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/litebuildd.dll

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/bin/liteappd.dll



goto end

:qtdir_fail
echo error, QTDIR is null
goto end

:end
