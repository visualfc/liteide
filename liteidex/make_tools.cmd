@echo off

set PATH=C:\go\bin;%PATH%
set GOPATH=%CD%

go install -ldflags "-s" -v liteide_stub
go install -ldflags "-s" -v github.com/nsf/gocode
