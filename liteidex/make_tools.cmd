@echo off

set GOPATH=%CD%

cd src\liteidex
windres -o liteide-res.syso liteide.rc
go install -ldflags "-s -H windowsgui" -v liteidex
del liteide-res.syso
cd ..\..

go install -v -ldflags -s tools/goastview
go install -v -ldflags -s tools/godocview
go install -v -ldflags -s tools/goexec
go install -v -ldflags -s tools/goapi
