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

### Ubuntu 14.04
	$ git clone https://github.com/visualfc/liteide.git
	$ sudo apt-get update
	$ sudo apt-get install qt4-dev-tools libqt4-dev libqt4-core libqt4-gui libqtwebkit-dev g++
	$ cd liteide/build
	$ ./update_pkg.sh
	$ QTDIR=/usr ./build_linux.sh
	
	## Run it: ##
	$ cd ~/liteide/build/liteide/bin
	$ ./liteide

### Ubuntu 16.04
	$ git clone https://github.com/visualfc/liteide.git
	$ sudo apt-get update
	$ sudo apt-get install qt4-dev-tools libqt4-dev libqtcore4 libqtgui4 libqtwebkit-dev g++
	$ cd liteide/build
	$ ./update_pkg.sh
	$ QTDIR=/usr ./build_linux.sh
	
	## Run it: ##
	$ cd ~/liteide/build/liteide/bin
	$ ./liteide

### Mac OS X
	# Install Qt if not already installed. Tested to work on 5.5, MacOS 10.12.4
 	$ brew install qt@5.5
	$ brew link --force qt@5.5
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=/usr/local/Cellar/qt\@5.5/5.5.1_1
	$ cd liteide/build
	$ ./update_pkg.sh
	$ ./build_osx_clang.sh
	$ ./deploy_osx_qt5.sh

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

Be sure to set `QTDIR` (and `MINGWDIR` for Windows users) accordingly based on your local environment.
