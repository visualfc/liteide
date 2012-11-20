CONFIG  += qtestlib
CONFIG += link_prl
DEFINES += QMAKE_BUILD

HEADERS = person.h

SOURCES = testqobjecthelper.cpp \
          person.cpp

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
