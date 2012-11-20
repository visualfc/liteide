#!/bin/sh

export GOPATH=$PWD

go install -v -ldflags "-s -r ." liteidex
go install -v -ldflags -s tools/goastview
go install -v -ldflags -s tools/godocview
go install -v -ldflags -s tools/goexec
go install -v -ldflags -s tools/goapi
