#!/bin/bash
# Example (see variable exports below):
#   QMAKE=qmake-qt4 PYTHON=python2 ./python/build.sh
#
# Note: FakeVim library must be built with same version of Qt.

set -e -x

# Executable paths
export QMAKE=${QMAKE:-qmake}
export SIP=${SIP:-sip}
export PYTHON=${PYTHON:-python}

# Project and library paths
export PROJECT_PATH=${PROJECT_PATH:-"$PWD"}
export LIBRARY_PATH=${LIBRARY_PATH:-"$PROJECT_PATH/fakevim"}
export SIP_FILE_PATH=${SIP_FILE_PATH:-"$PROJECT_PATH/python/fakevim.sip"}

# PyQt path to sip files (e.g. "QtCore/QtCoremod.sip"). Defaults to
# "/usr/share/sip/PyQt4" or "/usr/share/sip/PyQt5" depending on Qt version.
export PYQT_INCLUDE_PATH=${PYQT_INCLUDE_PATH:-""}

# remove previous build
rm -rf build

# build in separate directory
mkdir -p build
cd build

# generate files
"$PYTHON" "$PROJECT_PATH/python/configure.py"

# build
"$QMAKE" .
make

# test
export PYTHONPATH=$PWD
"$PYTHON" "$PROJECT_PATH/python/test.py"

