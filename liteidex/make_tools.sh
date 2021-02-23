#!/bin/sh

if [ -z $GOPATH ]; then
	export GOPATH=$PWD
else
	export GOPATH=$PWD:$GOPATH
fi


(cd "$PWD/src/github.com/visualfc/gotools" && go install -ldflags "-s" -v)
(cd "$PWD/src/github.com/visualfc/gocode" && go install -ldflags "-s" -v)
(cd "$PWD/src/github.com/fatih/gomodifytags" && go install -ldflags "-s" -v)
