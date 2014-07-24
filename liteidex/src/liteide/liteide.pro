#-------------------------------------------------
#
# Project created by QtCreator 2012-11-19T17:09:35
#
#-------------------------------------------------

include (../../liteidex.pri)
include (../liteapp/liteapp.pri)

QT += core gui

macx {
    QT += xml
}

TARGET = $$IDE_APP_TARGET
DESTDIR = $$IDE_APP_PATH
TEMPLATE = app
LIBS += -L$$IDE_LIBRARY_PATH
macx {
    LIBS += -L$$IDE_BIN_PATH
} else {
    LIBS += -L$$IDE_APP_PATH
}

linux-* {
    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var
    QMAKE_RPATHDIR += \$\$ORIGIN
    QMAKE_RPATHDIR += \$\$ORIGIN/../$$IDE_LIBRARY_BASENAME/liteide
    IDE_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${IDE_PLUGIN_RPATH}\'
    QMAKE_RPATHDIR =
}


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
