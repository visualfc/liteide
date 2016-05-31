#-------------------------------------------------
#
# Project created by QtCreator 2011-03-18T16:21:43
#
#-------------------------------------------------
include (../../liteidex.pri)
include (../rpath.pri)
include (../api/liteapi/liteapi.pri)
include (../utils/fileutil/fileutil.pri)
include (../utils/mimetype/mimetype.pri)
include (../utils/textoutput/textoutput.pri)
include (../utils/extension/extension.pri)
include (../utils/folderview/folderview.pri)
include (../utils/symboltreeview/symboltreeview.pri)
include (../utils/colorstyle/colorstyle.pri)
include (../3rdparty/elidedlabel/elidedlabel.pri)
include (../3rdparty/qtc_editutil/qtc_editutil.pri)

isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$IDE_SOURCE_TREE/src/pch/liteide_gui_pch.h

QT += core gui

macx {
    QT += xml
}

DESTDIR = $$IDE_APP_PATH
TEMPLATE = app
TARGET = $$qtLibraryName(liteapp)

contains(DEFINES, LITEAPP_LIBRARY) {
    TEMPLATE = lib
    win32 {
        DESTDIR = $$IDE_APP_PATH
    } else:macx {
        DESTDIR = $$IDE_BIN_PATH
    } else:linux-* {
            DESTDIR = $$IDE_LIBRARY_PATH
    }
}

LIBS += -L$$IDE_LIBRARY_PATH

macx {
    LIBS += -framework AppKit
}

INCLUDEPATH += $$IDE_SOURCE_TREE/src/api
INCLUDEPATH += $$IDE_SOURCE_TREE/src/utils
INCLUDEPATH += $$IDE_SOURCE_TREE/src/3rdparty

SOURCES += main.cpp\
        mainwindow.cpp \
    liteapp.cpp \
    filemanager.cpp \
    editormanager.cpp \
    projectmanager.cpp \
    pluginmanager.cpp \
    actionmanager.cpp \
    aboutdialog.cpp \
    pluginsdialog.cpp \
    mimetypemanager.cpp \
    litetabwidget.cpp \
    optionmanager.cpp \
    newfiledialog.cpp \
    optionsbrowser.cpp \
    liteappoption.cpp \
    liteappoptionfactory.cpp \
    toolmainwindow.cpp \
    tooldockwidget.cpp \
    rotationtoolbutton.cpp \
    toolwindowmanager.cpp \
    folderproject.cpp \
    folderprojectfactory.cpp \
    goproxy.cpp \
    htmlwidgetmanager.cpp \
    textbrowserhtmlwidget.cpp \
    splitwindowstyle.cpp \
    sidewindowstyle.cpp \
    outputoption.cpp \
    filtermanager.cpp \
    quickopenwidget.cpp

HEADERS  += mainwindow.h \
    liteapp.h \
    liteapp_global.h \
    filemanager.h \
    editormanager.h \
    projectmanager.h \
    pluginmanager.h \
    actionmanager.h \
    aboutdialog.h \
    pluginsdialog.h \
    mimetypemanager.h \
    litetabwidget.h \
    optionmanager.h \
    newfiledialog.h \
    optionsbrowser.h \
    liteappoption.h \
    liteappoptionfactory.h \
    toolmainwindow.h \
    tooldockwidget.h \
    rotationtoolbutton.h \
    toolwindowmanager.h \
    folderproject.h \
    folderprojectfactory.h \
    goproxy.h \
    cdrv.h \
    htmlwidgetmanager.h \
    textbrowserhtmlwidget.h \
    windowstyle.h \
    splitwindowstyle.h \
    sidewindowstyle.h \
    outputoption.h \
    filtermanager.h \
    quickopenwidget.h

FORMS += \
    aboutdialog.ui \
    pluginsdialog.ui \
    newfiledialog.ui \
    optionswidget.ui \
    liteappoption.ui \
    outputoption.ui

RESOURCES += \
    liteapp.qrc

macx {
    HEADERS  += macsupport.h
    OBJECTIVE_SOURCES += macsupport.mm
}

contains(DEFINES, LITEAPP_LIBRARY) {
    win32 {
        target.path = /bin
        INSTALLS += target
        RC_FILE += liteapp.rc
    } else:macx {
        ICON = images/liteide.icns
        QMAKE_INFO_PLIST = Info.plist
    } else {
        target.path  = /bin
        INSTALLS    += target
    }
}
