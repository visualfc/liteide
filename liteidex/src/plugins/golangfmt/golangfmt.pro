TARGET = golangfmt
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)
include (../../utils/gotools/gotools.pri)
include (../../3rdparty/diff_match_patch/diff_match_patch.pri)

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
