DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TEMPLATE = app
TARGET = localclient
HEADERS = \
    localclient.h
SOURCES = \
    localclient.cpp \
    main.cpp

