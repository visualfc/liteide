DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
include($${DEPTH}/src/http-parser/http-parser.pri)

TARGET = tst_qjsonrpchttpclient
HEADERS += \
    testhttpserver.h
SOURCES += \
    testhttpserver.cpp \
    tst_qjsonrpchttpclient.cpp
