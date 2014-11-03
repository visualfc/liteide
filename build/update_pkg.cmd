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

echo get gocode ...
go get -v -u "github.com/nsf/gocode"
echo update liteide_stub ...
go get -v -u "code.google.com/p/go.tools/astutil"
go get -v -u "code.google.com/p/go.tools/go/exact"
go get -v -u "code.google.com/p/go.tools/go/types"
go get -v -u "code.google.com/p/go.tools/go/gcimporter"
go get -v -u "code.google.com/p/go.tools/present"

cd %BUILD_ROOT%

goto end

:go_fail
echo error, go fail
goto end

:end
