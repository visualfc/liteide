TARGET = qsqleditor
TEMPLATE = lib
QT += sql

include(../../liteideplugin.pri)

DEFINES += QSQLEDITOR_LIBRARY

SOURCES += qsqleditorplugin.cpp \
    qsqleditor.cpp \
    sqliterfile.cpp \
    qsqlfilefactory.cpp \
    qsqldbfile.cpp \
    tableviewwidget.cpp \
    sqltablemodel.cpp \
    sqlitemdelegate.cpp

HEADERS += qsqleditorplugin.h\
        qsqleditor_global.h \
    qsqleditor.h \
    sqlitefile.h \
    qsqlfilefactory.h \
    qsqldbfile.h \
    tableviewwidget.h \
    sqltablemodel.h \
    sqlitemdelegate.h

FORMS += \
    qsqleditor.ui \
    tableviewwidget.ui
