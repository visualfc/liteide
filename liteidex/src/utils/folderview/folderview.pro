TARGET = foldersview
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../symboltreeview/symboltreeview.pri)

SOURCES += \
    folderview.cpp \
    basefolderview.cpp \
    folderlistview.cpp \
    folderlistmodel.cpp \
    filesystemmodelex.cpp \
    folderdialog.cpp

HEADERS += \
    folderview.h \
    basefoldeview.h \
    folderlistview.h \
    folderlistmodel.h \
    filesystemmodelex.h \
    folderdialog.h
