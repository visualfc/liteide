@echo off

setlocal

set BUILD_ROOT=%CD%
if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex

echo build liteide 
echo QTDIR=%QTDIR%
echo GOROOT=%GOROOT%
echo BUILD_ROOT=%BUILD_ROOT%
echo LITEIDE_ROOT=%LITEIDE_ROOT%
echo MINGWDIR=%MINGWDIR%
echo .

if x%QTDIR%==x goto qtdir_fail
if x%MINGWDIR%==x goto mwdir_fail

set PATH=%QTDIR%/bin;%MINGWDIR%/bin;%PATH%

echo qmake liteide ...
echo .
qmake %LITEIDE_ROOT% -spec win32-g++ "CONFIG+=release"

if ERRORLEVEL 1 goto qmake_fail

echo make liteide ...
echo .
mingw32-make

if ERRORLEVEL 1 goto make_fail

go version

if ERRORLEVEL 1 goto go_fail

echo build liteide tools
echo .

cd %LITEIDE_ROOT%
set GOPATH=%CD%;%GOPATH%

REM cd src\liteidex
REM windres -o liteide-res.syso liteide.rc
REM go install -ldflags "-s -H windowsgui" -v liteidex
REM del liteide-res.syso
REM cd ..\..

go install -ldflags "-s" -v tools/goastview
go install -ldflags "-s" -v tools/godocview
go install -ldflags "-s" -v tools/goexec
go install -ldflags "-s" -v tools/goapi
go install -ldflags "-s" -v tools/gopresent

cd %BUILD_ROOT%

echo deploy liteide ...
echo .

if exist liteide rmdir /q /s liteide

if not exist liteide mkdir liteide
if not exist liteide\bin mkdir liteide\bin
if not exist liteide\lib mkdir liteide\lib
if not exist liteide\lib\liteide mkdir liteide\lib\liteide
if not exist liteide\lib\liteide\plugins mkdir liteide\lib\liteide\plugins

xcopy %LITEIDE_ROOT%\LICENSE.LGPL liteide /y
xcopy %LITEIDE_ROOT%\LGPL_EXCEPTION.TXT liteide /y
xcopy %LITEIDE_ROOT%\..\README.MD liteide /y
xcopy %LITEIDE_ROOT%\..\CONTRIBUTORS liteide /y

xcopy %LITEIDE_ROOT%\liteide\bin\* liteide\bin /y
xcopy %LITEIDE_ROOT%\bin\* liteide\bin /y
xcopy %LITEIDE_ROOT%\liteide\lib\liteide\plugins\*.dll liteide\lib\liteide\plugins /y

xcopy %LITEIDE_ROOT%\deploy liteide\share\liteide /e /y /i 
xcopy %LITEIDE_ROOT%\os_deploy\windows liteide\share\liteide  /e /y /i 

goto end

:qtdir_fail
echo error, QTDIR is null
goto end

:mwdir_fail
echo error, MINGWDIR is null
goto end

:qmake_fail
echo error, qmake fail
goto end

:make_fail
echo error, make fail
goto end

:go_fail
echo error, go fail
goto end

:end
