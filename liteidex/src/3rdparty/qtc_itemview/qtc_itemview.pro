TARGET = qtc_itemview
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += \
    opendocumentstreeview.h \
    itemviews.h \
    proxymodel.h

SOURCES += \
    opendocumentstreeview.cpp \ 
    itemviews.cpp \
    proxymodel.cpp

RESOURCES += \
    qtc_itemview.qrc
