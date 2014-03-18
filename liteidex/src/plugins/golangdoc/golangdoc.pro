TARGET = golangdoc
TEMPLATE = lib
QT += xml

include(../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include(../../api/golangdocapi/golangdocapi.pri)
include(../../api/liteeditorapi/liteeditorapi.pri)
include(../../utils/documentbrowser/documentbrowser.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)
include(../../utils/htmlutil/htmlutil.pri)
include(../../utils/golangapi/golangapi.pri)
include(../../3rdparty/qtc_editutil/qtc_editutil.pri)
include(../../3rdparty/qjson/qjson.pri)
include(../../3rdparty/qt_browser/qt_browser.pri)

DEFINES += GOLANGDOC_LIBRARY

SOURCES += golangdocplugin.cpp \
    golangdoc.cpp \
    golangdocoptionfactory.cpp \
    golangdocoption.cpp \
    finddocwidget.cpp \
    findapiwidget.cpp

HEADERS += golangdocplugin.h\
        golangdoc_global.h \
    golangdoc.h \
    golangdocoptionfactory.h \
    golangdocoption.h \
    finddocwidget.h \
    findapiwidget.h

FORMS += \
    golangdocoption.ui
