DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TEMPLATE = app
QT += script core
HEADERS = interface.h
SOURCES = interface.cpp \
          main.cpp
