set GOARCH=386
call build_windows_mingw32.cmd
call update_pkg.cmd

if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex
xcopy %LITEIDE_ROOT%\bin\windows_386\gotools.exe liteide\bin /y
xcopy %LITEIDE_ROOT%\bin\windows_386\gocode.exe liteide\bin /y
xcopy %LITEIDE_ROOT%\bin\windows_386\gomodifytags.exe liteide\bin /y
