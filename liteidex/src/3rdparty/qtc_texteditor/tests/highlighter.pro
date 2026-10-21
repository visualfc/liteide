TEMPLATE = app
TARGET = tst_highlighter
QT += widgets testlib printsupport
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
CONFIG += testcase console c++11
CONFIG -= app_bundle

INCLUDEPATH += .. ../../../utils ../../../api
SOURCES += tst_highlighter.cpp ../syntaxhighlighter.cpp ../basetextdocumentlayout.cpp ../tabsettings.cpp
HEADERS += ../syntaxhighlighter.h ../basetextdocumentlayout.h
HEADERS += ../generichighlighter/highlighter.h

for(source, $$list(highlighter context highlightdefinition rule specificrules progressdata dynamicrule keywordlist itemdata includerulesinstruction)) {
    SOURCES += ../generichighlighter/$${source}.cpp
}
