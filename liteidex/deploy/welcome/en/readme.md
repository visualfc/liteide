<!-- Welcome to LiteIDE X -->

LiteIDE X
=========

### Introduction

_LiteIDE is a simple, open source, cross-platform Go IDE._

* Version: X38.4
* Author: [visualfc](mailto:visualfc@gmail.com)


### Features

* Core features
	* System environment management
	* MIME type management 
	* Configurable build commands
	* Support files search replace and revert
	* Quick open file, symbol and commands
	* Plug-in system

* Advanced code editor
	* Code editor supports Golang, Markdown and Golang Present
	* Rapid code navigation tools
	* Syntax highlighting and color scheme
	* Code completion
	* Code folding
	* Display save revision
	* Reload file by internal diff way

* Golang support
	* support Go1.18~Go1.20 generics
	* Support go1.11 Go modules
	* Support go1.5 Go vendor
	* Support go1 GOPATH
	* Golang build environment management
	* Compile and test using standard Golang tools
	* Custom GOPATH support system, IDE and project
	* Custom project build configuration
	* Golang package browser
	* Golang class view and outline
	* Golang doc search and api index
	* Source code navigation and information tips
	* Source code find usages
	* Source code refactoring and revert
	* Integrated  [gocode](https://github.com/visualfc/gocode) clone of [nsf/gocode](https://github.com/nsf/gocode)
	* Integrated [gomodifytags](https://github.com/fatih/gomodifytags)
	* Support source query tools guru
	* Debug with GDB and [Delve](https://github.com/derekparker/delve)

### Supported Systems
* Windows x86 (32-bit or 64-bit)
* Linux x86 (32-bit or 64-bit)
* MacOS X10.6 or higher (64-bit)
* FreeBSD 9.2 or higher (32-bit or 64-bit)
* OpenBSD 5.6 or higher (64-bit)

### LiteIDE Command Line
	liteide [files|folder] [--select-env id] [--local-setting] [--user-setting] [--reset-setting]
	 
	--select-env [system|win32|cross-linux64|...]     select init environment id
	--local-setting   force use local setting
	--user-setting    force use user setting
	--reset-setting   reset current setting ( clear setting file)
	
### Update liteide tools for support new Golang Version	

	go get -u github.com/visualfc/gotools
	go get -u github.com/visualfc/gocode
	
	Windows/Linux: copy GOPATH/bin gotools and gocode to liteide/bin
	MacOS: copy GOPATH/bin gotools and gocode to LiteIDE.app/Contents/MacOS	

### Website
* LiteIDE Home
	* <http://liteide.org>
* Source code
	* <https://github.com/visualfc/liteide>
* Binary downloads
	* <https://sourceforge.net/projects/liteide/files>
* Google group
	* <https://groups.google.com/group/liteide-dev>	
* FAQ
	* <https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/en/guide.md>	
* Support LiteIDE
	* <https://visualfc.github.com/support>