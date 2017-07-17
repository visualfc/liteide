DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)

TARGET = tst_qjsonrpchttpserver
SOURCES = tst_qjsonrpchttpserver.cpp

RESOURCES = certificates.qrc
