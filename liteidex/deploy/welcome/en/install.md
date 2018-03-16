<!-- Install -->

# Installing LiteIDE
## Prerequisites
Use LiteIDE development golang need to install the Go language development environment.

### Go
Begin by installing the Go language runtime.  Detailed instructions can be found at <http://golang.org/doc/install.html>.  Testing your installation before continuing is highly recommended.

## Install from binary
<http://sourceforge.net/projects/liteide/files>

### Windows
Download the archive and extract it into c:\

### Linux and MacOSX
Download the archive and extract it into /usr/local or $HOME

## Building from Source
Source code for LiteIDE can be found at <https://github.com/visualfc/liteide>.  You will need to install Qt4/Qt5 in order to compile the source.  Qt can be obtained from <https://qt-project.org/downloads> or from your distribution's package manager. For Mac OS X, you don't need to install Qt from source code, but run `brew update && brew install qt` which will save you several hours.

### Windows

**Qt4**

	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=c:\Qt\Qt4.8.5
	> set MINGWDIR=c:\Qt\MinGW
	> cd liteide/build
	> update_pkg.cmd
	> build_windows_mingw.cmd
	> deploy_windows_qt4.8_webkit.cmd

**Qt5.6**

	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=C:\Qt\Qt5.6.2\5.6\mingw49_32
	> set MINGWDIR=C:\Qt\Qt5.6.2\Tools\mingw492_32
	> cd liteide/build
	> update_pkg.cmd
	> build_windows_mingw.cmd
	> deploy_windows_qt5.6.cmd
	
### Ubuntu 16.04 Qt4
	## Install Qt4 ##
	$ sudo apt-get update
	$ sudo apt-get install libqt4-dev

	## Git clone and build liteide ##
	$ git clone https://github.com/visualfc/liteide.git
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_linux_qt4.sh
	
	## Run it: ##
	$ cd ~/liteide/build/liteide/bin
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
	$ ./update_pkg.sh
	$ ./build_linux_qt5.sh
	
	## Run it: ##
	$ cd ~/liteide/build/liteide/bin
	$ ./liteide
	
	## Deploy it: ##
	$ cd liteide/build
	$ ./deploy_linux_x64_qt5.sh
	## 32 bit
	$ ./deploy_linux_x32_qt5.sh

### Ubuntu 14.04
	$ sudo apt-get update
	$ sudo apt-get install qt4-dev-tools libqt4-dev libqt4-core libqt4-gui libqtwebkit-dev g++

	$ git clone https://github.com/visualfc/liteide.git
	$ cd liteide/build
	$ ./update_pkg.sh
	$ QTDIR=/usr ./build_linux.sh
	
	## Run it: ##
	$ cd ~/liteide/build/liteide/bin
	$ ./liteide

### Linux
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_linux.sh
	$ ./deploy_linux_qt4.8_webkit.sh

### Mac OS X / macOS

**Qt 4**

	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc #If QT is installed with brew, input: export QTDIR=/usr/local/Cellar/qt/4.8.6
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_macos_qt4.sh
	$ ./deploy_macos_qt4.sh
	$ open liteide/LiteIDE.app

**Qt 5 sdk install**

Download Qt from http://www.qt.io/download and install. (Qt5.6.2/Qt5.7.1/Qt5.8)

	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/Qt5.6.2/Qt5.6/clang_64
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_macos_qt5.sh
	$ ./deploy_macos_qt5.sh
	$ open liteide/LiteIDE.app
	
**Qt 5 brew install**	

Use brew install qt (eg brew install qt. Other versions of qt@5.5 and qt@5.7 works too).

	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=/usr/local/Cellar/qt/5.8.0_2 # or modify accordingly for qt@5.5 and qt@5.7
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_macos_qt5.sh
	$ open liteide/LiteIDE.app

Warning! brew install qt rpath incorrect do not use deploy script. 

### OpenBSD
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=/usr/local/lib/qt4
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_openbsd.sh

	## Run it: ##
	$ export LD_LIBRARY_PATH=$HOME/liteide/build/liteide/bin:$LD_LIBRARY_PATH
	$ cd ~/liteide/build/liteide/bin
	$ ./liteide

### Raspbian Jessie
	$ sudo apt-get update
	$ sudo apt-get install qt5-default xterm
	$ git clone https://github.com/visualfc/liteide.git
	$ cd liteide/build
	$ ./update_pkg.sh
	$ QTDIR=/usr ./build_linux.sh
	The build process from a µSD card takes approx. 105 minutes on a RPi 2.
	
	## Run it: ##
	$ cd ~/liteide/build/liteide/bin
	$ ./liteide

**Addin** Be sure to set `QTDIR` (and `MINGWDIR` for Windows users) accordingly based on your local environment.

## Fedora 27
        $ sudo dnf update
        $ sudo dnf install qt-devel
        $ git clone https://github.com/visualfc/liteide.git

Following this step I had to clone the contents of the following repos in liteide/src/github.com
i.e. liteidex/src/github.com/visualfc/gotools/README.md

* https://github.com/visualfc/gotools
* https://github.com/nsf/gocode
* https://github.com/fatih/gomodifytags

        $ cd liteide/build
        $ ./update_pkg.sh
        $ export QTDIR=/usr/bin
        $ ./build_linux_fedora27_x64.sh

        ## Run it: ##
        $ cd ~/liteide/build/liteide/bin
        $ ./liteide

