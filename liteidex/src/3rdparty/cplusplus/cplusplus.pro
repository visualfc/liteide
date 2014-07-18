TARGET = cplusplus
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)

HEADERS += \
    cppassert.h \
    Lexer.h \
    SimpleLexer.h \
    Token.h \
    BackwardsScanner.h

SOURCES += \
    gokeyword.cpp \
    gotypes.cpp \
    Keywords.cpp \
    Lexer.cpp \
    SimpleLexer.cpp \
    Token.cpp \
    BackwardsScanner.cpp
