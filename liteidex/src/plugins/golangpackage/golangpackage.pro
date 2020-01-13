TARGET = golangpackage
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include(../../api/golangdocapi/golangdocapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)
include (../../utils/symboltreeview/symboltreeview.pri)
include (../../3rdparty/qjson/qjson.pri)


DEFINES += GOLANGPACKAGE_LIBRARY

SOURCES += golangpackageplugin.cpp \
    filepathmodel.cpp \
    golangpackageoption.cpp \
    golangpackageoptionfactory.cpp \
    packagebrowser.cpp \
    gotool.cpp \
    packageproject.cpp \
    packagetree.cpp \
    packageprojectfactory.cpp

HEADERS += golangpackageplugin.h\
        golangpackage_global.h \
    filepathmodel.h \
    golangpackageoption.h \
    golangpackageoptionfactory.h \
    packagebrowser.h \
    gotool.h \
    packageproject.h \
    packagetree.h \
    packageprojectfactory.h

FORMS += \
    golangpackageoption.ui
