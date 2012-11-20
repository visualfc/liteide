#-------------------------------------------------
#
# Project created by QtCreator 2011-03-28T17:00:01
#
#-------------------------------------------------
TARGET = syntaxeditor
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/mimetype.pri)

DEFINES += SYNTAXEDITOR_LIBRARY

SOURCES += syntaxeditorplugin.cpp \
    syntaxtexteditor.cpp \
    syntaxeditor.cpp \
    golanghighlighter.cpp \
    syntaxfilefactory.cpp \
    projecthighlighter.cpp

HEADERS += syntaxeditorplugin.h\
        syntaxeditor_global.h \
    syntaxcompleter.h \
    syntaxtexteditor.h \
    syntaxeditor.h \
    golanghighlighter.h \
    syntaxfilefactory.h \
    projecthighlighter.h

OTHER_FILES += \
    syntaxeditor.xml
