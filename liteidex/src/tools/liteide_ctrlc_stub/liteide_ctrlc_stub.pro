!win32: error("process_ctrlc_stub is Windows only")

CONFIG    -= qt
CONFIG    += console warn_on

include(../../../liteidex.pri)

TEMPLATE  = app
TARGET    = liteide_ctrlc_stub
DESTDIR   = $$IDE_APP_PATH

SOURCES   += liteide_ctrlc_stub.cpp
LIBS      += -luser32 -lshell32


target.path  = $$IDE_APP_PATH
INSTALLS    += target
