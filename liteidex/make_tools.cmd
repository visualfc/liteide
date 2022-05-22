@echo off

if defined %GOPATH (
	set GOPATH=%CD%;%GOPATH%
) else (
	set GOPATH=%CD%
)

echo get gocode ...
go install -v github.com/visualfc/gocode@latest
echo get gotools ...
go install -v github.com/visualfc/gotools@latest
echo get gomodifytags ...
go install -v github.com/fatih/gomodifytags@latest
