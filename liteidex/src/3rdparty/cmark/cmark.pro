TEMPLATE = lib

CONFIG += staticlib

TARGET = cmark

include(../../liteideutils.pri)


DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += .

SOURCES += \
        libcmark.cpp \
        src/blocks.c \
        src/buffer.c \
        src/cmark.c \
        src/cmark_ctype.c \
        src/commonmark.c \
        src/houdini_href_e.c \
        src/houdini_html_e.c \
        src/houdini_html_u.c \
        src/html.c \
        src/inlines.c \
        src/iterator.c \
        src/latex.c \
        src/man.c \
        src/node.c \
        src/references.c \
        src/render.c \
        src/scanners.c \
        src/utf8.c \
        src/xml.c

DISTFILES += \
    src/case_fold_switch.inc \
    src/entities.inc

HEADERS += \
    libcmark.h \
    src/buffer.h \
    src/chunk.h \
    src/cmark.h \
    src/cmark_ctype.h \
    src/houdini.h \
    src/inlines.h \
    src/iterator.h \
    src/node.h \
    src/parser.h \
    src/references.h \
    src/render.h \
    src/scanners.h \
    src/utf8.h
