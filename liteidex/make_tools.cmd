@echo off

set PATH=C:\go\bin;%PATH%
set GOPATH=%CD%

go install -ldflags "-s" -v tools/goastview
go install -ldflags "-s" -v tools/godocview
go install -ldflags "-s" -v tools/goexec
go install -ldflags "-s" -v tools/goapi
go install -ldflags "-s" -v tools/gopresent
go install -ldflags "-s" -v github.com/visualfc/goimports
go install -ldflags "-s" -v github.com/nsf/gocode
go install -ldflags "-s" -v github.com/gpmgo/gopm
