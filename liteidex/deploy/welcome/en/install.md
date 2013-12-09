<!-- Install -->

# Installing LiteIDE
## Prerequisites
Use LiteIDE development golang need to install the Go language development environment.

### Go
Begin by installing the Go language runtime.  Detailed instructions can be found at <http://golang.org/doc/install.html>.  Testing your installation before continuing is highly recommended.

## Install from binary
<http://code.google.com/p/golangide>

### Windows
Download the archive and extract it into c:\

### Linux and MacOSX
Download the archive and extract it into /usr/local or $HOME

## Building from Source
Source code for LiteIDE can be found at <https://github.com/visualfc/liteide>.  You will need to install Qt4/Qt5 in order to compile the source.  Qt can be obtained from <https://qt-project.org/downloads> or from your distribution's package manager. 

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

Be sure to set `QTDIR` (and `MINGWDIR` for Windows users) accordingly based on your local environment.