TARGET = filesystem
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../symboltreeview/symboltreeview.pri)

SOURCES += filesystemwidget.cpp \
    filesystemmodel.cpp \
    ../folderview/folderdialog.cpp

HEADERS += filesystemwidget.h \
    filesystemmodel.h \
    ../folderview/folderdialog.h
