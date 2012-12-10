TARGET = markdown
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../3rdparty/sundown/sundown.pri)

DEFINES += MARKDOWN_LIBRARY

SOURCES += markdownplugin.cpp \
    htmlpreview.cpp \
    markdownedit.cpp

HEADERS += markdownplugin.h\
        markdown_global.h \
    htmlpreview.h \
    markdownedit.h

RESOURCES += \
    markdown.qrc

OTHER_FILES +=
