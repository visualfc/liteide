@echo off

setlocal

set "BUILD_ROOT=%CD%"
if not defined LITEIDE_ROOT set "LITEIDE_ROOT=%CD%\..\liteidex"

echo build liteide
echo QTDIR=%QTDIR%
echo .

if defined QTDIR set "PATH=%QTDIR%\bin;%PATH%"
where windeployqt >nul 2>&1
if ERRORLEVEL 1 goto deploy_missing

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/liteeditor.dll
if ERRORLEVEL 1 goto deploy_fail

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/welcome.dll
if ERRORLEVEL 1 goto deploy_fail

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/litebuild.dll
if ERRORLEVEL 1 goto deploy_fail

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/lib/liteide/plugins/dlvdebugger.dll
if ERRORLEVEL 1 goto deploy_fail

windeployqt --no-angle --no-opengl-sw --dir liteide/bin liteide/bin/liteapp.dll
if ERRORLEVEL 1 goto deploy_fail



goto end

:qtdir_fail
goto deploy_missing

:deploy_missing
echo error, windeployqt not found in PATH
exit /b 1

:deploy_fail
echo error, windeployqt fail
exit /b 1

:end
exit /b 0
