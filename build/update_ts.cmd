@echo off

@echo off

setlocal

set BUILD_ROOT=%CD%
if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex

echo QTDIR=%QTDIR%
echo GOROOT=%GOROOT%
echo BUILD_ROOT=%BUILD_ROOT%
echo LITEIDE_ROOT=%LITEIDE_ROOT%
echo .

if x%QTDIR%==x goto qtdir_fail

echo update translator files
lupdate %LITEIDE_ROOT%\liteidex.pro

goto end

:go_fail
echo error, go fail
goto end

:end
