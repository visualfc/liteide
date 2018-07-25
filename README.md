<!-- Welcome to LiteIDE X -->

LiteIDE X
=========

![liteide-logo](liteidex/liteide-logo/liteide.png)

### Introduction

_LiteIDE is a simple, open source, cross-platform Go IDE._

* Version: X34 (support go1.11 Go modules)
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
	* Support go1.11 Go modules
	* Support go1.5 Go vendor
	* Support go1 GOPATH
	* Golang build environment management
	* Compile and test using standard Golang tools and GOPATH
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

### Latest Release Supported Platform Details
* Windows
	* liteide-latest.windows-qt5.zip -> WindowsXP, Windows 7 8 10
	* liteide-latest.windows-qt4.zip -> WindowsXP, Windows 7
* macOS
	* liteide-latest.macosx-qt5.zip -> macOS 10.8 or higher
* Linux x64
	* liteide-latest.linux-64-qt4.tar.bz2 -> Linux (64 bit) build on ubuntu 16.04
	* liteide-latest.linux-64-qt5.tar.bz2 -> Linux (64 bit) build on ubuntu 16.04
* Linux x32
	* liteide-latest.linux-32-qt4.tar.bz2 -> Linux (32 bit) build on ubuntu 16.04
	* liteide-latest.linux-32-qt5.tar.bz2 -> Linux (32 bit) build on ubuntu 16.04
* ArchLinux
	* liteide-latest.archlinux-pkgbuild.zip -> ArchLinux (64 bit) PKGBUILD

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

### Documents
* How to Install
<https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/en/install.md>
* FAQ
<https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/en/guide.md>
* 安装 LiteIDE
<https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/zh_CN/install.md>
* FAQ 中文
<https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/zh_CN/guide.md>

### Links
* LiteIDE Home
<http://liteide.org>
* LiteIDE Source code
<https://github.com/visualfc/liteide>
* Gotools Source code
<https://github.com/visualfc/gotools>
* Release downloads
	* <http://sourceforge.net/projects/liteide/files>
	* <https://github.com/visualfc/liteide/releases/latest>
* Google group
<https://groups.google.com/group/liteide-dev>
* Changes
<https://github.com/visualfc/liteide/blob/master/liteidex/deploy/welcome/en/changes.md>


### Donate
* http://visualfc.github.com/support

### New Home Page
* [Home](http://liteide.org)
* [English Document](http://liteide.org/en/documents)
* [中文文档](http://liteide.org/cn/documents)
* More info at [liteide.org](http://liteide.org)