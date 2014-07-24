TARGET = qtctexteditor
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)
include(../../utils/colorstyle/colorstyle.pri)

DEFINES += TEXTEDITOR_LIBRARY
INCLUDEPATH += .

HEADERS += \
    texteditor_global.h \
    tabsettings.h \
    syntaxhighlighter.h \
    itexteditor.h \
    basetextdocumentlayout.h \
    generichighlighter/specificrules.h \
    generichighlighter/rule.h \
    generichighlighter/reuse.h \
    generichighlighter/progressdata.h \
    generichighlighter/keywordlist.h \
    generichighlighter/itemdata.h \
    generichighlighter/includerulesinstruction.h \
    generichighlighter/highlighterexception.h \
    generichighlighter/highlighter.h \
    generichighlighter/highlightdefinitionmetadata.h \
    generichighlighter/highlightdefinitionhandler.h \
    generichighlighter/highlightdefinition.h \
    generichighlighter/dynamicrule.h \
    generichighlighter/context.h \
    katehighlighter.h \
    generichighlighter/manager2.h \
    colorscheme.h \
    autocompleter.h

SOURCES += \
    tabsettings.cpp \
    syntaxhighlighter.cpp \
    itexteditor.cpp \
    basetextdocumentlayout.cpp \
    generichighlighter/specificrules.cpp \
    generichighlighter/rule.cpp \
    generichighlighter/progressdata.cpp \
    generichighlighter/keywordlist.cpp \
    generichighlighter/itemdata.cpp \
    generichighlighter/includerulesinstruction.cpp \
    generichighlighter/highlighter.cpp \
    generichighlighter/highlightdefinitionmetadata.cpp \
    generichighlighter/highlightdefinitionhandler.cpp \
    generichighlighter/highlightdefinition.cpp \
    generichighlighter/dynamicrule.cpp \
    generichighlighter/context.cpp \
    katehighlighter.cpp \
    generichighlighter/manager2.cpp \
    colorscheme.cpp \
    autocompleter.cpp
