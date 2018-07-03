#!/bin/sh

if [ -z $GOPATH ]; then
	export GOPATH=$PWD
else
	export GOPATH=$PWD:$GOPATH
fi


go install -ldflags "-s" -v github.com/visualfc/gotools
go install -ldflags "-s" -v github.com/mdempsky/gocode
go install -ldflags "-s" -v github.com/fatih/gomodifytags
