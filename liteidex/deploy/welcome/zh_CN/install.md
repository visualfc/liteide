<!-- Install -->

# 安装 LiteIDE
## 先决条件
使用LiteIDE开发golang需要安装Go语言开发环境。

### Go
先要安装Go语言，详细情况请参看<http://golang.org/doc/install.html>. 
在安装LiteIDE之前一定要先安装Go语言。

## 安装二进制文件
<https://sourceforge.net/projects/liteide/>

### Windows
下载压缩包并解压至c:\盘

### Linux and MacOSX
下载压缩包并解压到/usr/local或者是$HOME

## 从源代码编译
LiteIDE源码位于<https://github.com/visualfc/liteide>上。需要使用Qt4/Qt5来编译源代码，Qt库可以从<https://qt-project.org/downloads>上获取。Mac OS X用户可以不从源代码编译Qt，直接在终端中运行`brew update && brew install qt`，节省大量时间。下面的编译以Qt4为例。

### Windows
**Qt4**

	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=c:\Qt\Qt4.8.5
	> set MINGWDIR=c:\Qt\MinGW
	> cd liteide/build
	> install_pkg.cmd
	> build_windows_mingw.cmd
	> deploy_windows_qt4.8_webkit.cmd

**Qt5.6**

	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=C:\Qt\Qt5.6.2\5.6\mingw49_32
	> set MINGWDIR=C:\Qt\Qt5.6.2\Tools\mingw492_32
	> cd liteide/build
	> install_pkg.cmd
	> build_windows_mingw.cmd
	> deploy_windows_qt5.6.cmd
	
### Ubuntu 16.04 Qt4
	## Install Qt4 ##
	$ sudo apt-get update
	$ sudo apt-get install libqt4-dev

	## Git clone and build liteide ##
	$ git clone https://github.com/visualfc/liteide.git
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_linux_qt4.sh
	
	## Run it: ##
	$ cd liteide/build/liteide/bin
	$ ./liteide
	
	## Deploy it: ##
	$ cd liteide/build
	$ ./deploy_linux_x64_qt4.sh
	## 32 bit
	$ ./deploy_linux_x32_qt4.sh
	
### Ubuntu 16.04 Qt5
	## Install Qt5 ##
	$ sudo apt-get update
	$ sudo apt-get install qt5-default
	
	## Git clone and build liteide ##
	$ git clone https://github.com/visualfc/liteide.git
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_linux_qt5.sh
	
	## Run it: ##
	$ cd liteide/build/liteide/bin
	$ ./liteide
	
	## Deploy it: ##
	$ cd liteide/build
	$ ./deploy_linux_x64_qt5.sh
	## 32 bit
	$ ./deploy_linux_x32_qt5.sh
	
### Ubuntu 14.04
	$ git clone https://github.com/visualfc/liteide.git
	$ sudo apt-get update
	$ sudo apt-get install qt4-dev-tools libqt4-dev libqt4-core libqt4-gui libqtwebkit-dev g++
	$ cd liteide/build
	$ ./install_pkg.sh
	$ QTDIR=/usr ./build_linux.sh
	
	## Run it: ##
	$ cd liteide/build/liteide/bin
	$ ./liteide

### Linux
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_linux.sh
	$ ./deploy_linux_qt4.8_webkit.sh

### Mac OS X / macOS

**Qt 4**

	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc #如果Qt是通过brew安装，输入: export QTDIR=/usr/local/Cellar/qt/4.8.6
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_maoos_qt4.sh
	$ ./deploy_macos_qt4.sh
	$ open liteide/LiteIDE.app

**Qt 5 sdk install**

从 http://www.qt.io/download 下载并安装Qt. (Qt5.6.2/Qt5.7.1/Qt5.8)

	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/Qt5.6.2/Qt5.6/clang_64
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_macos_qt5.sh
	$ ./deploy_macos_qt5.sh
	$ open liteide/LiteIDE.app
	
**Qt 5 brew install**	

使用 brew 安装 Qt. (比如使用 brew install qt. 其他版本如 qt@5.5 qt@5.6 qt@5.7 ).

	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=/usr/local/Cellar/qt/5.8.0_2 # or modify accordingly for qt@5.5 and qt@5.7
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_macos_qt5.sh
	$ open liteide/LiteIDE.app

警告！ 使用 brew 安装的 Qt rpath 不正确，不要使用 deploy 脚本进行打包。

### OpenBSD
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=/usr/local/lib/qt4
	$ cd liteide/build
	$ ./install_pkg.sh
	$ ./build_openbsd.sh

	## Run it: ##
	$ export LD_LIBRARY_PATH=$HOME/liteide/build/liteide/bin:$LD_LIBRARY_PATH
	$ cd liteide/build/liteide/bin
	$ ./liteide

### Raspbian Jessie
	$ sudo apt-get update
	$ sudo apt-get install qt5-default xterm
	$ git clone https://github.com/visualfc/liteide.git
	$ cd liteide/build
	$ ./install_pkg.sh
	$ QTDIR=/usr ./build_linux.sh
	The build process from a µSD card takes approx. 105 minutes on a RPi 2.
	
	## Run it: ##
	$ cd liteide/build/liteide/bin
	$ ./liteide


**补充：** 根据自己的环境设置好 `QTDIR` (在Windows上还需要设置`MINGWDIR`)
