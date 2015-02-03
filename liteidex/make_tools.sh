#!/bin/sh

export GOPATH=$PWD

go install -ldflags "-s" -v github.com/visualfc/gotools
go install -ldflags "-s" -v github.com/nsf/gocode
