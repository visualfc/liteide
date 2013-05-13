@echo off

setlocal

set BUILD_ROOT=%CD%
if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex

echo build liteide 
echo QTDIR=%QTDIR%
echo .

if x%QTDIR%==x goto qtdir_fail

xcopy %QTDIR%\bin\Qt5Core.dll liteide\bin /y
xcopy %QTDIR%\bin\Qt5Gui.dll liteide\bin /y 
xcopy %QTDIR%\bin\Qt5Xml.dll liteide\bin /y 
xcopy %QTDIR%\bin\mingwm10.dll liteide\bin /y
xcopy %QTDIR%\bin\libgcc_s_dw2-1.dll liteide\bin /y

xcopy %QTDIR%\bin\phonon4.dll liteide\bin /y
xcopy %QTDIR%\bin\QtNetwork4.dll liteide\bin /y 
xcopy %QTDIR%\bin\QtWebkit4.dll liteide\bin /y 

goto end

:qtdir_fail
echo error, QTDIR is null
goto end

:end
