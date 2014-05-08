TARGET = qtc_searchresult
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += \ 
    searchresultcolor.h \
    searchresulttreeitemdelegate.h \
    searchresulttreeitemroles.h \
    searchresulttreeitems.h \
    searchresulttreemodel.h \
    searchresulttreeview.h \
    searchresultitem.h \
    searchresultitem.h \
    searchresultwidget.h

SOURCES += \ 
    searchresulttreeitemdelegate.cpp \
    searchresulttreeitems.cpp \
    searchresulttreemodel.cpp \
    searchresulttreeview.cpp \
    searchresultwidget.cpp

RESOURCES +=
