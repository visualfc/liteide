TARGET = qtl_itemview
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += \
    opendocumentstreeview.h \
    itemviews.h

SOURCES += \
    opendocumentstreeview.cpp \ 
    itemviews.cpp

RESOURCES += \
    qtc_itemview.qrc
