#!/bin/sh

export GOPATH=$PWD

go install -ldflags "-s" -v liteide_stub
go install -ldflags "-s" -v github.com/visualfc/goimports
go install -ldflags "-s" -v github.com/nsf/gocode
