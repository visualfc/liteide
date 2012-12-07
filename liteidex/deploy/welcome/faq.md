<!-- FAQ -->

# LiteIDE FAQ

## Environment
LiteIDE environment setup plugin

### Select Environment
Select current use environment, default system

![envselect.png](images/envselect.png)

* Windows - system win64 win32
* Linux - system linux64 linux32
* MacOSX - system darwin64 darwin32

### Configure Environment
Options -> LiteEnv, double clicked *.env file to editor.

Example `win32.env`

	#win32 environment
	
	GOROOT=c:\go
	GOARCH=386
	GOOS=windows
	
	PATH=%GOROOT%\bin;%PATH%
	
	LITEIDE_GDB=gdb
	LITEIDE_MAKE=mingw32-make
	LITEIDE_TERM=%COMSPEC%
	LITEIDE_TERMARGS=
	LITEIDE_EXEC=%COMSPEC%
	LITEIDE_EXECOPT=/C


Example `linux32.env`

	#linux32 environment
	
	GOROOT=$HOME/go
	GOARCH=386
	GOOS=linux
	
	PATH=$GOROOT/bin:$PATH
	
	LITEIDE_GDB=gdb
	LITEIDE_MAKE=make
	LITEIDE_TERM=/usr/bin/gnome-terminal
	LITEIDE_TERMARGS=
	LITEIDE_EXEC=/usr/bin/xterm
	LITEIDE_EXECOPT=-e

## Build System
LiteIDE build system, build project or file, if file belong a project

### Configure Build
Options -> LiteBuild, double clicked *.xml file to editor.

Example gosrc.xml

	<?xml version="1.0" encoding="UTF-8"?>
	<mime-info xmlns='http://www.freedesktop.org/standards/shared-mime-info'>
		<mime-type type="text/x-gosrc" id="gosrc" work="$(EDITOR_DIR)" ver="1">
			<config id="Go" name="GO" value="$(GOROOT)/bin/go"/>
			<config id="GoExec" name="GOEXEC" value="$(LITEAPPDIR)/goexec"/>
			<config id="ErrRegex" name="ERRREGEX" value="([\w\d_\\/\.]+):(\d+):"/>
			<custom id="TargetArgs" name="TARGETARGS" value=""/>
			<custom id="BuildArgs" name="BUILDARGS" value=""/>
			<custom id="InstallArgs" name="INSTALLARGS" value=""/>
			<action id="Build" img="blue/build.png" key="Ctrl+B;F7" cmd="$(GO)" args="build $(BUILDARGS)" save="all" output="true" codec="" regex="$(ERRREGEX)" />
			<action id="Install" menu="Build" img="blue/install.png" key="Ctrl+F8" cmd="$(GO)" args="install $(INSTALLARGS)" save="all" output="true"/>
			<action id="Clean" menu="Build" img="blue/clean.png" cmd="$(GO)" args="clean" save="all" output="true"/>
			<action id="CleanAll" menu="Build" img="blue/cleanall.png" cmd="$(GO)" args="clean -i" save="all" output="true"/>
			<action id="BuildAndRun" img="blue/buildrun.png" key="Ctrl+R;Ctrl+F7" task="Build;Run"/>
			<action id="Run" menu="BuildAndRun" img="blue/run.png" key="Ctrl+F5" cmd="$(EDITOR_DIRNAME_GO)" args="$(TARGETARGS)" output="true" codec="utf-8" readline="true"/>
			<action id="RunTerm" menu="BuildAndRun" img="blue/runterm.png" key="Ctrl+Shift+F5" cmd="$(LITEIDE_EXEC)" args="$(LITEIDE_EXECOPT) $(GOEXEC) $(EDITOR_DIRNAME_GO) $(TARGETARGS)" output="false" readline="true"/>
			<action id="FileRun" menu="BuildAndRun" img="gray/filerun.png" key="Alt+F6" cmd="$(GO)" args="run $(EDITOR_NAME)" save="editor" output="true" codec="utf-8" readline="true"/>
			<action id="Test" img="blue/test.png" key = "Ctrl+T" cmd="$(GO)" args="test" save="all" output="true" codec="utf-8"/>
			<action id="Get" menu="Test" img="blue/get.png" cmd="$(GO)" args="get -v ." save="all" output="true" codec="utf-8"/>
			<action id="Fmt" menu="Test" img="blue/fmt.png" cmd="$(GO)" args="fmt" save="all" output="true" regex="$(ERRREGEX)"/>
			<action id="Vet" menu="Test" img="blue/vet.png" cmd="$(GO)" args="vet" save="all" output="true" regex="$(ERRREGEX)"/>
			<debug id="Debug" cmd="$(EDITOR_DIRNAME_GO)" args="$(TARGETARGS)" work="$(EDITOR_DIR)"/>
		</mime-type>	
	</mime-info>

## Gocode
Use gocode word complete, example input `fmt.`

GitHub:	<https://github.com/nsf/gocode>

### Install gocode
	$ export GOROOT=$HOME/go
	$ export PATH=$HOME/go/bin:$PATH
	$ go get -u github.com/nsf/gocode

## Golang document
Golang doc browser and api search.

### Find golang api
example input `zip` or `fmt.P`

## Search and replace
Editor code search and replace

### Regex

Replace regex:
`(Colo)(u)(r)` -> `\1\3`

## Golang Debug
LiteIDE debug plugin

### Gdb version >= 7.1
Golang debug request gdb version 7.1 or high

### Source path
The file or project source path cannot include outsite space or latin1 char

### Environment LITEIDE_GDB
example LiteEnv -> win64.env

	#win64 environment
	
	GOROOT=c:\go-w64
	GOARCH=amd64
	GOOS=windows
	
	PATH=%GOROOT%\bin;%PATH%
	
	LITEIDE_GDB=gdb64
	LITEIDE_MAKE=mingw32-make
	LITEIDE_TERM=%COMSPEC%
	LITEIDE_TERMARGS=
	LITEIDE_EXEC=%COMSPEC%
	LITEIDE_EXECOPT=/C
	
### WIN32 and WIN64
WIN32 request gdb.exe,WIN64 request gdb64.exe.