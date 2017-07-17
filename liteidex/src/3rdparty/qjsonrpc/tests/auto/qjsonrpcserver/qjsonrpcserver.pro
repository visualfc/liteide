DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)

TARGET = tst_qjsonrpcserver
HEADERS = \
    testservices.h
SOURCES = \
    testservices.cpp \
    tst_qjsonrpcserver.cpp
