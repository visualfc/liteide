@echo off

if defined %GOPATH (
	set GOPATH=%CD%;%GOPATH%
) else (
	set GOPATH=%CD%
)

go run src/tools/exportqrc/main.go -root .
