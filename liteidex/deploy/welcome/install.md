<!-- Install -->

Install
=======

## Install golang
<http://golang.org/doc/install.html>

## Download LiteIDE
<https://github.com/visualfc/liteide/downloads>

## Install Gocode</h2>
<https://github.com/nsf/gocode>

	$ export GOROOT=$HOME/go
	$ export PATH=$HOME/go/bin:$PATH
	$ go get -u github.com/nsf/gocode 


## Build LiteIDE Source
<https://github.com/visualfc/liteide>

### Windows
	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=c:\Qt\Qt474
	> set MINGWDIR=c:\Qt\MinGW
	> cd liteide/build
	> build_mingw.cmd


### Linux
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/474/gcc
	$ cd liteide/build
	$ ./build_linux.sh

### MacOSX
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/474/gcc
	$ cd liteide/build
	$ ./build_osx.sh

