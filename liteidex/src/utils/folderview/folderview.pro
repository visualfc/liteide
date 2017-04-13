TARGET = folderview
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
    folderdialog.cpp \
    abstractmultiproxymodel.cpp \
    dirsortfilterproxymodel.cpp \
    multifoldermodel.cpp \
    multiindexmodel.cpp \
    multifolderview.cpp

HEADERS += \
    folderview.h \
    basefoldeview.h \
    folderlistview.h \
    folderlistmodel.h \
    filesystemmodelex.h \
    folderdialog.h \
    abstractmultiproxymodel_p.h \
    abstractmultiproxymodel.h \
    dirsortfilterproxymodel.h \
    multifoldermodel.h \
    multiindexmodel_p.h \
    multiindexmodel.h \
    multifolderview.h
