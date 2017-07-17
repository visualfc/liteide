INCLUDEPATH += $${PWD}

QJSON_INSTALL_HEADERS += \
    $${PWD}/qjson_export.h

PRIVATE_HEADERS += \
    $${PWD}/qjson_p.h \
    $${PWD}/qjsonwriter_p.h \
    $${PWD}/qjsonparser_p.h \
    $${PWD}/qjsondocument.h \
    $${PWD}/qjsonobject.h \
    $${PWD}/qjsonvalue.h \
    $${PWD}/qjsonarray.h

SOURCES += \
    $${PWD}/qjson.cpp \
    $${PWD}/qjsondocument.cpp \
    $${PWD}/qjsonobject.cpp \
    $${PWD}/qjsonarray.cpp \
    $${PWD}/qjsonvalue.cpp \
    $${PWD}/qjsonwriter.cpp \
    $${PWD}/qjsonparser.cpp


json.files = \
    $${PWD}/qjsondocument.h \
    $${PWD}/qjsonobject.h \
    $${PWD}/qjsonvalue.h \
    $${PWD}/qjsonarray.h

json.path = $${PREFIX}/include/qjsonrpc/json
INSTALLS += json
