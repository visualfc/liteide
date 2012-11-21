include (../liteidex.pri)

isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$IDE_SOURCE_TREE/src/pch/liteide_gui_pch.h


DESTDIR = $$IDE_LIBRARY_PATH
LIBS += -L$$IDE_LIBRARY_PATH
LIBS += -L$$IDE_PLUGIN_PATH

INCLUDEPATH += $$IDE_SOURCE_TREE/src/api
INCLUDEPATH += $$IDE_SOURCE_TREE/src/api/liteapi
INCLUDEPATH += $$IDE_SOURCE_TREE/src/utils
INCLUDEPATH += $$IDE_SOURCE_TREE/src/3rdparty


isEmpty(TARGET) {
    error("liteideutils.pri: You must provide a TARGET")
}

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

!macx {
    target.path = /lib
    INSTALLS += target
}

TARGET = $$qtLibraryTarget($$TARGET)
