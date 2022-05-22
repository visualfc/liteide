@echo off

setlocal

set BUILD_ROOT=%CD%
if x%LITEIDE_ROOT%==x set LITEIDE_ROOT=%CD%\..\liteidex

echo GOROOT=%GOROOT%
echo BUILD_ROOT=%BUILD_ROOT%
echo LITEIDE_ROOT=%LITEIDE_ROOT%
echo .

go version

if ERRORLEVEL 1 goto go_fail

echo update liteide tools ...

cd %LITEIDE_ROOT%
if defined %GOPATH (
	set GOPATH=%CD%;%GOPATH%
) else (
	set GOPATH=%CD%
)
echo GOPATH=%GOPATH%

echo get gocode ...
go install -v github.com/visualfc/gocode@latest
echo get gotools ...
go install -v github.com/visualfc/gotools@latest
echo get gomodifytags ...
go install -v github.com/fatih/gomodifytags@latest

cd %BUILD_ROOT%

goto end

:go_fail
echo error, go fail
goto end

:end
