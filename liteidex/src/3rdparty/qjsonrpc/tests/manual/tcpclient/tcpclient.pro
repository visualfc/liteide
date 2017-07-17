DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TEMPLATE = app
TARGET = tcpclient
HEADERS = \
    tcpclient.h
SOURCES = \
    tcpclient.cpp \
    main.cpp

