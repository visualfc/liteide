#!/bin/sh

export GOBIN=$PWD/bin

echo install gocode ...
go install -v github.com/visualfc/gocode@latest
echo install gotools ...
go install -v github.com/visualfc/gotools@latest
echo install gomodifytags ...
go install -v github.com/fatih/gomodifytags@latest
