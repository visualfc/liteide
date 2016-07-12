include (../liteidex.pri)
include (api/liteapi/liteapi.pri)

isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$IDE_SOURCE_TREE/src/pch/liteide_gui_pch.h

DESTDIR = $$IDE_PLUGIN_PATH
LIBS += -L$$IDE_PLUGIN_PATH
LIBS += -L$$IDE_LIBRARY_PATH

INCLUDEPATH += $$IDE_SOURCE_TREE/src/api
INCLUDEPATH += $$IDE_SOURCE_TREE/src/utils
INCLUDEPATH += $$IDE_SOURCE_TREE/src/3rdparty

isEmpty(TARGET) {
    error("liteideplugin.pri: You must provide a TARGET")
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../PlugIns/
} else:linux-* {
    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    QMAKE_RPATHDIR += \$\$ORIGIN
    QMAKE_RPATHDIR += \$\$ORIGIN/..
    QMAKE_RPATHDIR += \$\$ORIGIN/../..
    IDE_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")
    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${IDE_PLUGIN_RPATH}\'
    QMAKE_RPATHDIR =
}


contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

CONFIG += plugin plugin_with_soname

!macx {
    target.path = $$PLUGINPREFIX
    INSTALLS += target
}

TARGET = $$qtLibraryTarget($$TARGET)

