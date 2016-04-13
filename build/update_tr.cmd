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

echo compress translator files
lrelease -compress %LITEIDE_ROOT%\liteide_zh.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_zh.qm
lrelease -compress %LITEIDE_ROOT%\liteide_zh_TW.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_zh_TW.qm
lrelease -compress %LITEIDE_ROOT%\liteide_de.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_de.qm
lrelease -compress %LITEIDE_ROOT%\liteide_fr.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_fr.qm
lrelease -compress %LITEIDE_ROOT%\liteide_ja.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_ja.qm
lrelease -compress %LITEIDE_ROOT%\liteide_ru.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_ru.qm
lrelease -compress %LITEIDE_ROOT%\liteide_uk.ts -qm %LITEIDE_ROOT%\deploy\translations\liteide_uk.qm

goto end

:go_fail
echo error, go fail
goto end

:end
