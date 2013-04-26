TARGET = welcome
TEMPLATE = lib

QT += xml

include (../../liteideplugin.pri)
include (../../3rdparty/sundown/sundown.pri)
include (../../3rdparty/elidedlabel/elidedlabel.pri)
include (../../utils/documentbrowser/documentbrowser.pri)
include (../../utils/htmlutil/htmlutil.pri)

DEFINES += WELCOME_LIBRARY

SOURCES += welcomeplugin.cpp \
    welcomebrowser.cpp \
    litedoc.cpp

HEADERS += welcomeplugin.h\
        welcome_global.h \
    welcomebrowser.h \
    litedoc.h

FORMS +=

OTHER_FILES +=

RESOURCES += \
    welcome.qrc
