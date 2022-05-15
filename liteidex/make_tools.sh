#!/bin/sh

if [ -z $GOPATH ]; then
	export GOPATH=$PWD
else
	export GOPATH=$PWD:$GOPATH
fi


echo get gocode ...
go install -v github.com/visualfc/gocode@latest
echo get gotools ...
go install -v github.com/visualfc/gotools@latest
echo get gomodifytags ...
go install -v github.com/fatih/gomodifytags@latest
