TEMPLATE = app
TARGET =
CONFIG += link_prl
DEPENDPATH += .
win32 {
  LIBS += -L../../lib -lqjson0
} macx {
  QMAKE_LFLAGS += -F../../lib
  LIBS += -framework qjson
} unix:!macx {
  LIBS += -L../../lib -lqjson
}
INCLUDEPATH +=  . \
                ../../include

# Input
SOURCES += cmdline_tester.cpp cmdlineparser.cpp
