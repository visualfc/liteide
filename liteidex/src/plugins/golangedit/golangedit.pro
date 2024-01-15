TARGET = golangedit
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include (../../api/litefindapi/litefindapi.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)
include (../../api/litebuildapi/litebuildapi.pri)
include (../../api/quickopenapi/quickopenapi.pri)
include (../../utils/processex/processex.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/textoutput/textoutput.pri)
include (../../utils/colorstyle/colorstyle.pri)
include (../../3rdparty/qtc_texteditor/qtc_texteditor.pri)
include (../../3rdparty/cplusplus/cplusplus.pri)
include (../../3rdparty/qtc_editutil/qtc_editutil.pri)


DEFINES += GOLANGEDIT_LIBRARY

SOURCES += golangeditplugin.cpp \
    golangedit.cpp \
    golangfilesearch.cpp \
    golanghighlighter.cpp \
    golanghighlighterfactory.cpp \
    golangtextlexer.cpp \
    golangeditoption.cpp \
    golangeditoptionfactory.cpp \
    goaddtagsdialog.cpp \
    goplsfilesearch.cpp \
    goremovetagsdialog.cpp

HEADERS += golangeditplugin.h\
        golangedit_global.h \
    golangedit.h \
    golangfilesearch.h \
    golanghighlighter.h \
    golanghighlighterfactory.h \
    golangtextlexer.h \
    golangeditoption.h \
    golangeditoptionfactory.h \
    goaddtagsdialog.h \
    goplsfilesearch.h \
    goremovetagsdialog.h

FORMS += \
    golangeditoption.ui \
    goaddtagsdialog.ui \
    goremovetagsdialog.ui
