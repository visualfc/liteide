DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TEMPLATE = app
TARGET = qjsonrpc
SOURCES = \
    qjsonrpc.cpp
