TARGET = golangtool
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GOLANGTOOL_LIBRARY

SOURCES += golangtoolplugin.cpp \
    gopathbrowser.cpp \
    gopathmodel.cpp \
    gopathproject.cpp \
    gopathprojectfactor.cpp \
    importgopathdialog.cpp \
    ../filebrowser/createfiledialog.cpp \
    ../filebrowser/createdirdialog.cpp

HEADERS += golangtoolplugin.h\
        golangtool_global.h \
    gopathbrowser.h \
    gopathmodel.h \
    gopathproject.h \
    gopathprojectfactor.h \
    importgopathdialog.h \
    ../filebrowser/createfiledialog.h \
    ../filebrowser/createdirdialog.h

FORMS += \
    importgopathdialog.ui \
    ../filebrowser/createfiledialog.ui \
    ../filebrowser/createdirdialog.ui

RESOURCES += \
    golangtool.qrc
