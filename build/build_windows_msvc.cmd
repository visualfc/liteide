@echo off

setlocal

set "BUILD_ROOT=%CD%"
if not defined LITEIDE_ROOT set "LITEIDE_ROOT=%CD%\..\liteidex"

echo build liteide
echo QTDIR=%QTDIR%
echo GOROOT=%GOROOT%
echo BUILD_ROOT=%BUILD_ROOT%
echo LITEIDE_ROOT=%LITEIDE_ROOT%
echo .

if defined QTDIR set "PATH=%QTDIR%\bin;%PATH%"
where qmake >nul 2>&1
if ERRORLEVEL 1 goto qmake_missing

echo qmake liteide ...
echo .
qmake "%LITEIDE_ROOT%" "CONFIG+=release" "CONFIG-=precompile_header"

if ERRORLEVEL 1 goto qmake_fail

echo make liteide ...
echo .
nmake

if ERRORLEVEL 1 goto make_fail

go version

if ERRORLEVEL 1 goto go_fail

echo build liteide tools
echo .

cd /d "%LITEIDE_ROOT%"
if defined GOPATH (
	set "GOPATH=%CD%;%GOPATH%"
) else (
	set "GOPATH=%CD%"
)

:: (cd "%CD%/src/github.com/visualfc/gotools" & go install -ldflags "-s" -v & cd %CD%)
:: (cd "%CD%/src/github.com/visualfc/gocode" & go install -ldflags "-s" -v & cd %CD%)
:: (cd "%CD%/src/github.com/fatih/gomodifytags" & go install -ldflags "-s" -v & cd %CD%)

if ERRORLEVEL 1 goto go_fail

echo export qrc images
go run src/tools/exportqrc/main.go -root .
if ERRORLEVEL 1 goto go_fail

cd /d "%BUILD_ROOT%"

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
xcopy %LITEIDE_ROOT%\bin\gotools.exe liteide\bin /y
xcopy %LITEIDE_ROOT%\bin\gocode.exe liteide\bin /y
xcopy %LITEIDE_ROOT%\bin\gomodifytags.exe liteide\bin /y
xcopy %LITEIDE_ROOT%\bin\gopls.exe liteide\bin /y
xcopy %LITEIDE_ROOT%\liteide\lib\liteide\plugins\*.dll liteide\lib\liteide\plugins /y

xcopy %LITEIDE_ROOT%\deploy liteide\share\liteide /e /y /i
xcopy %LITEIDE_ROOT%\os_deploy\windows liteide\share\liteide  /e /y /i

goto end

:qtdir_fail
goto qmake_missing

:qmake_missing
echo error, qmake not found in PATH
exit /b 1

:qmake_fail
echo error, qmake fail
exit /b 1

:make_fail
echo error, make fail
exit /b 1

:go_fail
echo error, go fail
exit /b 1

:end
exit /b 0
