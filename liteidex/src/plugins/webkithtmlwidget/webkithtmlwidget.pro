TARGET = webkithtmlwidget
TEMPLATE = lib

include(../../liteideplugin.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
QT += webkitwidgets
} else {
QT += webkit
}

DEFINES += WEBKITHTMLWIDGET_LIBRARY

SOURCES += webkithtmlwidgetplugin.cpp \
    webviewhtmlwidget.cpp

HEADERS += webkithtmlwidgetplugin.h\
        webkithtmlwidget_global.h \
    webviewhtmlwidget.h
