INCLUDEPATH += $${QJSONRPC_INCLUDEPATH} \
               $${QJSONRPC_INCLUDEPATH}/json \
               $${PWD}/common
               
LIBS += -L$${DEPTH}/src $${QJSONRPC_LIBS}
QT = core network testlib
QT -= gui
CONFIG -= app_bundle
CONFIG += testcase no_testcase_installs

HEADERS += \
    $${PWD}/common/signalspy.h 
    
unix:!macx:QMAKE_RPATHDIR += $${OUT_PWD}/$${DEPTH}/src
macx {
    QMAKE_RPATHDIR += @loader_path/$${DEPTH}/src
    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/$${DEPTH}/src
}
