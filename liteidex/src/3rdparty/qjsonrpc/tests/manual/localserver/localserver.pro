DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TEMPLATE = app
TARGET = localserver
HEADERS = \
    testservice.h
SOURCES = \
    testservice.cpp \
    localserver.cpp
