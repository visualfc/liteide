include (../liteidex.pri)

DESTDIR = $$IDE_LIBRARY_PATH
LIBS += -L$$IDE_LIBRARY_PATH

INCLUDEPATH += $$IDE_SOURCE_TREE/src/api

isEmpty(TARGET) {
    error("liteideapi.pri: You must provide a TARGET")
}

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

!macx {
    target.path = $$LIBPREFIX
    INSTALLS += target
}

TARGET = $$qtLibraryTarget($$TARGET)

