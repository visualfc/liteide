TARGET = webkithtmlwidget
TEMPLATE = lib

include(../../liteideplugin.pri)

QT += webkit

DEFINES += WEBKITHTMLWIDGET_LIBRARY

SOURCES += webkithtmlwidgetplugin.cpp \
    webviewhtmlwidget.cpp

HEADERS += webkithtmlwidgetplugin.h\
        webkithtmlwidget_global.h \
    webviewhtmlwidget.h
