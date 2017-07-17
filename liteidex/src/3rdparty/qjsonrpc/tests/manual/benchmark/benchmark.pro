DEPTH = ../../..
include($${DEPTH}/qjsonrpc.pri)
include($${DEPTH}/tests/tests.pri)
CONFIG -= testcase

TARGET = benchmark
SOURCES = \
    tst_benchmark.cpp
