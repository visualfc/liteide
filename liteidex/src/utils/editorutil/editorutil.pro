TARGET = editorutil
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../../3rdparty/diff_match_patch/diff_match_patch.pri)

SOURCES += editorutil.cpp

HEADERS += editorutil.h \
    difflib.h
