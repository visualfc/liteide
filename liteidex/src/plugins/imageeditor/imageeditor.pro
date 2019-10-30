TARGET = imageeditor
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../utils/mimetype/mimetype.pri)
include (../../utils/navigate/navigate.pri)

DEFINES += IMAGEEDITOR_LIBRARY

SOURCES += \
    imageeditor.cpp \
    imageeditorfactory.cpp \
    imageeditorfile.cpp \
    imageeditorplugin.cpp \
    imageeditorwidget.cpp

HEADERS +=\
        imageeditor.h \
        imageeditor_global.h \
        imageeditorfactory.h \
        imageeditorfile.h \
        imageeditorplugin.h \
        imageeditorwidget.h

RESOURCES += \
    imageeditor.qrc

!isEmpty(QT.svg.name): QT += svg
else: DEFINES += QT_NO_SVG

