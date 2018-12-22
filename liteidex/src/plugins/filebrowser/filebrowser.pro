TARGET = filebrowser
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/folderview/folderview.pri)
include(../../utils/symboltreeview/symboltreeview.pri)
include(../../api/litebuildapi/litebuildapi.pri)
include(../../api/golangdocapi/golangdocapi.pri)
include(../../api/litefindapi/litefindapi.pri)


DEFINES += FILEBROWSER_LIBRARY

SOURCES += filebrowserplugin.cpp \
    filebrowser.cpp \
    filebrowseroptionfactory.cpp \
    filebrowseroption.cpp \

HEADERS += filebrowserplugin.h\
        filebrowser_global.h \
    filebrowser.h \
    filebrowseroptionfactory.h \
    filebrowseroption.h

RESOURCES += \
    filebrowser.qrc

FORMS += \
    filebrowseroption.ui
