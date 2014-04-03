TARGET = litefind
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../api/litefindapi/litefindapi.pri)
include (../../utils/textoutput/textoutput.pri)
include (../../3rdparty/qtc_searchresult/qtc_searchresult.pri)

DEFINES += LITEFIND_LIBRARY

SOURCES += litefindplugin.cpp \
    filesearch.cpp \
    findeditor.cpp \
    filesearchmanager.cpp

HEADERS += litefindplugin.h\
        litefind_global.h \
    filesearch.h \
    findeditor.h \
    filesearchmanager.h
