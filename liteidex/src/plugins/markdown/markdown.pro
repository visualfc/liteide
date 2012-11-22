TARGET = markdown
TEMPLATE = lib

DEFINES += LITEIDE_WEBKIT

contains(DEFINES, LITEIDE_WEBKIT) {
    QT += webkit
}

include (../../liteideplugin.pri)
include (../../3rdparty/sundown/sundown.pri)

DEFINES += MARKDOWN_LIBRARY

SOURCES += markdownplugin.cpp \
    htmlpreview.cpp

HEADERS += markdownplugin.h\
        markdown_global.h \
    htmlpreview.h
