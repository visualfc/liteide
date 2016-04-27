Python Bindings for FakeVim
===========================

Few notes:
 * Bindings build process is handled using
   PyQt's [SIP](http://pyqt.sourceforge.net/Docs/sip4/index.html).
 * Debug build is enabled by default
 * Compile FakeVim library and binndings with same version of Qt.
 * Use same major version of python to build and run.

To build run bash script `build.sh` or just create `build` sub-directory in
this directory and from there run `configure.py`, `qmake` and `make`.

To install run `make install` from `build` sub-directory.

On Ubuntu 14.04, package `pyqt5-dev` must be installed. Additionally build
script needs to be invoked with following command.
```
PYTHON=python3 ./build.sh
```

