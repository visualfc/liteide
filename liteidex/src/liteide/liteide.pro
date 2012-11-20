#-------------------------------------------------
#
# Project created by QtCreator 2012-11-19T17:09:35
#
#-------------------------------------------------

include (../../liteidex.pri)
include (../rpath.pri)
include (../liteapp/liteapp.pri)

QT -= gui

TARGET = $$IDE_APP_TARGET
DESTDIR = $$IDE_APP_PATH
TEMPLATE = app

LIBS += -L$$IDE_APP_PATH

SOURCES += main.cpp

win32 {
    target.path = /bin
    INSTALLS += target
    RC_FILE += liteide.rc
} else:macx {
    ICON = liteide.icns
    QMAKE_INFO_PLIST = Info.plist
} else {
    target.path  = /bin
    INSTALLS    += target
}
