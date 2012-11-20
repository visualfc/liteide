TARGET = litebuild
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/litebuildapi/litebuildapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)
include (../../utils/textoutput/textoutput.pri)

DEFINES += LITEBUILD_LIBRARY

SOURCES += litebuildplugin.cpp \
    litebuild.cpp \
    build.cpp \
    buildmanager.cpp \
    litebuildoptionfactory.cpp \
    litebuildoption.cpp \
    buildconfigdialog.cpp

HEADERS += litebuildplugin.h\
        litebuild_global.h \
    litebuild.h \
    build.h \
    buildmanager.h \
    litebuildoptionfactory.h \
    litebuildoption.h \
    buildconfigdialog.h

RESOURCES += \
    litebuild.qrc

FORMS += \
    litebuildoption.ui \
    buildconfigdialog.ui
