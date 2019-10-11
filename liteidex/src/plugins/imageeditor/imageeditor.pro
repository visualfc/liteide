TARGET = imageeditor
TEMPLATE = lib

!isEmpty(QT.svg.name): QT += svg
else: DEFINES += QT_NO_SVG


include(../../liteideplugin.pri)
include (../../utils/mimetype/mimetype.pri)

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
