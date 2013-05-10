TEMPLATE = lib

CONFIG += staticlib

TARGET = sundown

include(../../liteideutils.pri)

DEPENDPATH += html src
INCLUDEPATH += . ./src ./html

HEADERS += src/autolink.h \
           src/buffer.h \
           src/html_blocks.h \
           src/markdown.h \
           src/stack.h \
           html/houdini.h \
           html/html.h \
           mdtohtml.h \

SOURCES += html/houdini_href_e.c \
           html/houdini_html_e.c \
           html/html.c \
           html/html_smartypants.c \
           src/autolink.c \
           src/buffer.c \
           src/markdown.c \
           src/stack.c \
           mdtohtml.cpp \

QMAKE_CFLAGS_WARN_ON -= -W -Wextra
QMAKE_CXXFLAGS_WARN_ON -= -W -Wextra
