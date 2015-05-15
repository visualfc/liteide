<!-- Install -->

# 安装 LiteIDE
## 先决条件
使用LiteIDE开发golang需要安装Go语言开发环境。

### Go
先要安装Go语言，详细情况请参看<http://golang.org/doc/install.html>. 
在安装LiteIDE之前一定要先安装Go语言。

## 安装二进制文件
<http://sourceforge.net/projects/liteide/>

### Windows
下载压缩包并解压至c:\盘

### Linux and MacOSX
下载压缩包并解压到/usr/local或者是$HOME

## 从源代码编译
LiteIDE源码位于<https://github.com/visualfc/liteide>上。需要使用Qt4/Qt5来编译源代码，Qt库可以从<https://qt-project.org/downloads>上获取。下面的编译以Qt4为例。

### Windows
	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=c:\Qt\Qt484
	> set MINGWDIR=c:\Qt\MinGW
	> cd liteide/build
	> update_pkg.cmd
	> build_mingw.cmd
	> deploy_qt4.8_webkit.cmd

### Linux
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_linux.sh
	$ ./deploy_linux_qt4.8_webkit.sh

### Mac OS X
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_osx.sh
	$ ./deploy_osx_qt4.sh

根据自己的环境一定要设置`QTDIR`(在Windows上还需要设置`MINGWDIR`)
