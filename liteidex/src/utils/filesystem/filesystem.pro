TARGET = filesystem
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../symboltreeview/symboltreeview.pri)

SOURCES += filesystemwidget.cpp \
    filesystemmodel.cpp \
    ../../plugins/filebrowser/createfiledialog.cpp \
    ../../plugins/filebrowser/createdirdialog.cpp

HEADERS += filesystemwidget.h \
    filesystemmodel.h \
    ../../plugins/filebrowser/createfiledialog.h \
    ../../plugins/filebrowser/createdirdialog.h

FORMS += \
    ../../plugins/filebrowser/createfiledialog.ui \
    ../../plugins/filebrowser/createdirdialog.ui
