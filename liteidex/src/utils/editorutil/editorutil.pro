TARGET = editorutil
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../../3rdparty/diff_match_patch/diff_match_patch.pri)
include (../../3rdparty/libucd/libucd.pri)

SOURCES += editorutil.cpp

HEADERS += editorutil.h \
    difflib.h \
    libucd.h \
    checkdata.h
