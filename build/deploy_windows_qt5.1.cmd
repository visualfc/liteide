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

xcopy %QTDIR%\bin\Qt5Core.dll liteide\bin /y
xcopy %QTDIR%\bin\Qt5Gui.dll liteide\bin /y
xcopy %QTDIR%\bin\Qt5Widgets.dll liteide\bin /y
xcopy %QTDIR%\bin\Qt5Xml.dll liteide\bin /y
xcopy %QTDIR%\bin\Qt5PrintSupport.dll liteide\bin /y
xcopy %QTDIR%\bin\icudt51.dll liteide\bin /y
xcopy %QTDIR%\bin\icuin51.dll liteide\bin /y
xcopy %QTDIR%\bin\icuuc51.dll liteide\bin /y
xcopy %QTDIR%\bin\libwinpthread-1.dll liteide\bin /y
xcopy %QTDIR%\bin\libgcc_s_dw2-1.dll liteide\bin /y
xcopy %QTDIR%\bin\libstdc++-6.dll liteide\bin /y
xcopy %QTDIR%\bin\Qt5Network.dll liteide\bin /y

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
