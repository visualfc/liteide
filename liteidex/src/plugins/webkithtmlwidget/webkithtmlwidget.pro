TARGET = webkithtmlwidget
TEMPLATE = lib

include(../../liteideplugin.pri)
#include (../../3rdparty/sundown/sundown.pri)
include (../../3rdparty/cmark/cmark.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
QT += webkitwidgets
} else {
QT += network
QT += webkit
}

DEFINES += WEBKITHTMLWIDGET_LIBRARY

SOURCES += webkithtmlwidgetplugin.cpp \
    webviewhtmlwidget.cpp \
    webkitbrowser.cpp

HEADERS += webkithtmlwidgetplugin.h\
        webkithtmlwidget_global.h \
    webviewhtmlwidget.h \
    webkitbrowser.h
