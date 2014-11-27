TARGET = foldersview
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../symboltreeview/symboltreeview.pri)

SOURCES += \
    folderview.cpp \
    basefolderview.cpp \
    folderlistview.cpp \
    folderlistmodel.cpp

HEADERS += \
    folderview.h \
    basefoldeview.h \
    folderlistview.h \
    folderlistmodel.h
