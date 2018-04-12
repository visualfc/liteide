TARGET = quickopen
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/quickopenapi/quickopenapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../3rdparty/qtc_editutil/qtc_editutil.pri)

DEFINES += QUICKOPEN_LIBRARY

SOURCES += quickopenplugin.cpp \
    quickopenwidget.cpp \
    quickopenmanager.cpp \
    quickopenfiles.cpp \
    quickopenlines.cpp \
    quickopenhelp.cpp \
    quickopeneditor.cpp \
    quickopenmimetype.cpp \
    quickopenoption.cpp \
    quickopenoptionfactory.cpp \
    quickopenaction.cpp \
    quickopenfolder.cpp

HEADERS += quickopenplugin.h\
        quickopen_global.h \
    quickopenwidget.h \
    quickopenmanager.h \
    quickopenfiles.h \
    quickopenlines.h \
    quickopenhelp.h \
    quickopeneditor.h \
    quickopenmimetype.h \
    quickopenoption.h \
    quickopenoptionfactory.h \
    quickopenaction.h \
    quickopenfolder.h

FORMS += \
    quickopenoption.ui
