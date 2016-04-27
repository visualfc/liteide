TARGET = fakevimedit
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/litefindapi/litefindapi.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)
include (../../3rdparty/fakevim/fakevim/fakevim.pri)

DEFINES += FAKEVIMEDIT_LIBRARY

SOURCES += fakevimeditplugin.cpp \
    fakevimedit.cpp \
    fakevimeditoption.cpp \
    fakevimeditoptionfactory.cpp

HEADERS += fakevimeditplugin.h\
        fakevimedit_global.h \
    fakevimedit.h \
    fakevimeditoption.h \
    fakevimeditoptionfactory.h

FORMS += \
    fakevimeditoption.ui
