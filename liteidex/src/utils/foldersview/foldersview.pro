TARGET = foldersview
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../symboltreeview/symboltreeview.pri)

SOURCES += foldersview.cpp \
    foldersmodel.cpp

HEADERS += foldersview.h \
    foldersmodel.h
