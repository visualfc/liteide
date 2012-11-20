TARGET = golangfmt
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)

DEFINES += GOLANGFMT_LIBRARY

SOURCES += golangfmtplugin.cpp \
    golangfmt.cpp \
    golangfmtoptionfactory.cpp \
    golangfmtoption.cpp

HEADERS += golangfmtplugin.h\
        golangfmt_global.h \
    golangfmt.h \
    golangfmtoptionfactory.h \
    golangfmtoption.h

RESOURCES += \
    golangfmt.qrc

FORMS += \
    golangfmtoption.ui
