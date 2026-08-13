@echo off

setlocal

set "BUILD_ROOT=%CD%"
if not defined LITEIDE_ROOT set "LITEIDE_ROOT=%CD%\..\liteidex"
if defined QTDIR set "PATH=%QTDIR%\bin;%PATH%"

where qmake >nul 2>&1 || goto qmake_missing
for /f "delims=" %%V in ('qmake -query QT_VERSION') do set "QT_VERSION=%%V"
if not "%QT_VERSION:~0,2%"=="6." goto qt_version_fail

qmake "%LITEIDE_ROOT%" "CONFIG+=release" "CONFIG-=precompile_header"
if ERRORLEVEL 1 goto build_fail
nmake
if ERRORLEVEL 1 goto build_fail

cd /d "%LITEIDE_ROOT%"
call make_tools.cmd
if ERRORLEVEL 1 goto tools_fail
go run src/tools/exportqrc/main.go -root .
if ERRORLEVEL 1 goto tools_fail

cd /d "%BUILD_ROOT%"
if exist liteide rmdir /q /s liteide
mkdir liteide\bin
mkdir liteide\lib\liteide\plugins
mkdir liteide\share\liteide

copy "%LITEIDE_ROOT%\LICENSE.LGPL" liteide\
copy "%LITEIDE_ROOT%\LGPL_EXCEPTION.TXT" liteide\
copy "%LITEIDE_ROOT%\..\README.md" liteide\
copy "%LITEIDE_ROOT%\..\CONTRIBUTORS" liteide\
xcopy "%LITEIDE_ROOT%\liteide\bin\*" liteide\bin /y
copy "%LITEIDE_ROOT%\bin\gocode.exe" liteide\bin\
copy "%LITEIDE_ROOT%\bin\gotools.exe" liteide\bin\
copy "%LITEIDE_ROOT%\bin\gomodifytags.exe" liteide\bin\
copy "%LITEIDE_ROOT%\bin\gopls.exe" liteide\bin\
xcopy "%LITEIDE_ROOT%\liteide\lib\liteide\plugins\*.dll" liteide\lib\liteide\plugins /y
xcopy "%LITEIDE_ROOT%\deploy" liteide\share\liteide /e /y /i
xcopy "%LITEIDE_ROOT%\os_deploy\windows" liteide\share\liteide /e /y /i
exit /b 0

:qmake_missing
echo error, qmake not found in PATH
exit /b 1

:qt_version_fail
echo error, qmake is not Qt 6
exit /b 1

:build_fail
echo error, Qt 6 build failed
exit /b 1

:tools_fail
echo error, Go tools build failed
exit /b 1
