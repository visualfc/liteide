DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)

TARGET = tst_qtjson
QT = core testlib
CONFIG -= app_bundle
CONFIG += testcase

TESTDATA += test.json test.bjson test3.json test2.json
SOURCES += tst_qtjson.cpp
RESOURCES += tst_qtjson.qrc
