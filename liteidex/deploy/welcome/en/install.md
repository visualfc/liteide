<!-- Install -->

# Installing LiteIDE
## Prerequisites
To install and use LiteIDE effectively, three main components are required: Go, gocode, and GDB.  While gocode does not need to be installed, it is required for auto-completion beyond the basic keywords.  GDB is required for debugging support.

### Go
Begin by installing the Go language runtime.  Detailed instructions can be found at <http://golang.org/doc/install.html>.  Testing your installation before continuing is highly recommended.

### Gocode
This optional step is required for auto-completion of code elements such as variable and function names.  Gocode operates independently from LiteIDE, so a persistent cache of auto-completion data is maintained between editor sessions.

Gocode can be found at <https://github.com/nsf/gocode>.

Be sure to read the setup instructions for gocode carefully.  You may need to modify your system path if you did not install the Go compiler suite in the standard way.

### GDB
This optional step is required for debugging support.  Install GDB 7.1 or higher using your distribution's package manager.  On Windows, you will need to install MinGW from <http://mingw.org/>.

Ensure that gdb can be found on your system PATH before launching LiteIDE.

## Install from binary
<http://code.google.com/p/golangide>

## Building from Source
Source code for LiteIDE can be found at <https://github.com/visualfc/liteide>.  You will need to install Qt4 in order to compile the source.  Qt4 can be obtained from <https://qt-project.org/downloads> or from your distribution's package manager.  On Windows, ensure that you install Qt4 for MinGW.

Windows users will also need to install a MinGW environment with gcc 4.4 for binary compatibility with Qt4.  One such environment is available at <http://nosymbolfound.blogspot.ca/2012/12/since-until-now-qt-under-windows-is.html>.

After installing these dependencies, compilation is simple:

### Windows
	> git clone https://github.com/visualfc/liteide.git
	> set QTDIR=c:\Qt\Qt484
	> set MINGWDIR=c:\Qt\MinGW
	> cd liteide/build
	> build_mingw.cmd

### Linux
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc
	$ cd liteide/build
	$ ./build_linux.sh

### Mac OS X
	$ git clone https://github.com/visualfc/liteide.git
	$ export QTDIR=$HOME/QtSDK/Desktop/Qt/484/gcc
	$ cd liteide/build
	$ ./build_osx.sh

Be sure to set `QTDIR` (and `MINGWDIR` for Windows users) accordingly based on your local environment.