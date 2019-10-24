TARGET = liteeditor
TEMPLATE = lib

QT += xml

include (../../liteideplugin.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)
include (../../api/quickopenapi/quickopenapi.pri)
include (../../utils/extension/extension.pri)
include (../../utils/mimetype/mimetype.pri)
include (../../utils/colorstyle/colorstyle.pri)
include (../../utils/editorutil/editorutil.pri)
include (../../utils/folderview/folderview.pri)
include (../../utils/navigate/navigate.pri)
include (../../3rdparty/qtc_texteditor/qtc_texteditor.pri)
include (../../3rdparty/qtc_editutil/qtc_editutil.pri)
include (../../3rdparty/treemodelcompleter/treemodelcompleter.pri)
include (../../3rdparty/elidedlabel/elidedlabel.pri)
include (../../3rdparty/qjson/qjson.pri)

DEFINES += LITEEDITOR_LIBRARY

SOURCES += liteeditorplugin.cpp \
    liteeditor.cpp \
    liteeditorfile.cpp \
    liteeditorfilefactory.cpp \
    liteeditoroption.cpp \
    liteeditoroptionfactory.cpp \
    liteeditorwidgetbase.cpp \
    liteeditorwidget.cpp \
    litecompleter.cpp \
    litewordcompleter.cpp \
    liteeditormark.cpp \
    highlightermanager.cpp \
    katehighlighterfactory.cpp \
    codecompleter.cpp \
    functiontooltip.cpp \
    faketooltip.cpp \
    snippetapi.cpp \
    wordapi.cpp \
    editorapimanager.cpp \
    katetextlexer.cpp

HEADERS += liteeditorplugin.h\
        liteeditor_global.h \
    liteeditor.h \
    liteeditorfile.h \
    liteeditorfilefactory.h \
    liteeditoroption.h \
    liteeditoroptionfactory.h \
    liteeditorwidgetbase.h \
    liteeditorwidget.h \
    litecompleter.h \
    litewordcompleter.h \
    liteeditormark.h \
    highlightermanager.h \
    katehighlighterfactory.h \
    codecompleter.h \
    functiontooltip.h \
    faketooltip.h \
    snippetapi.h \
    wordapi.h \
    editorapimanager.h \
    katetextlexer.h

FORMS += \
    liteeditoroption.ui

RESOURCES += \
    liteeditor.qrc
