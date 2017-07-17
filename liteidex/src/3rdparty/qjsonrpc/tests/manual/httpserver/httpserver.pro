DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TEMPLATE = app
TARGET = httpserver
HEADERS = \
    testservice.h
SOURCES = \
    testservice.cpp \
    httpserver.cpp
