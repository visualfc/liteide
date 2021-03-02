@echo off

if defined %GOPATH (
	set GOPATH=%CD%;%GOPATH%
) else (
	set GOPATH=%CD%
)

(cd "%CD%/src/github.com/visualfc/gotools" & go install -ldflags "-s" -v & cd %CD%)
(cd "%CD%/src/github.com/visualfc/gocode" & go install -ldflags "-s" -v & cd %CD%)
(cd "%CD%/src/github.com/fatih/gomodifytags" & go install -ldflags "-s" -v & cd %CD%)
