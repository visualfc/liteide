TARGET = bookmarks
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)

DEFINES += BOOKMARKS_LIBRARY

SOURCES += bookmarksplugin.cpp \
    bookmarkmanager.cpp \
    bookmarkmodel.cpp

HEADERS += bookmarksplugin.h\
        bookmarks_global.h \
    bookmarkmanager.h \
    bookmarkmodel.h

DISTFILES +=

RESOURCES += \
    bookmarks.qrc
