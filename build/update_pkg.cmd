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
set GOPATH=%CD%;%GOPATH%

echo get goimports ...
go get -v -u "github.com/visualfc/goimports"
echo get gocode ...
go get -v -u "github.com/nsf/gocode"


cd %BUILD_ROOT%

goto end

:go_fail
echo error, go fail
goto end

:end
