include(../qjsonrpc.pri)
include(http-parser/http-parser.pri)

INCLUDEPATH += .
TEMPLATE = lib
TARGET = qjsonrpc
QT += core gui network

DEFINES += QJSONRPC_BUILD
CONFIG += $${QJSONRPC_LIBRARY_TYPE}
VERSION = $${QJSONRPC_VERSION}

CONFIG += staticlib

include(../../../liteideutils.pri)

#win32:DESTDIR = $$OUT_PWD
#macx:QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/

# check if we need to build qjson
lessThan(QT_MAJOR_VERSION, 5) {
    include(json/json.pri)
}

PRIVATE_HEADERS += \
    qjsonrpcservice_p.h \
    qjsonrpcsocket_p.h \
    qjsonrpcabstractserver_p.h \
    qjsonrpcservicereply_p.h \
    qjsonrpchttpserver_p.h

INSTALL_HEADERS += \
    qjsonrpcmessage.h \
    qjsonrpcservice.h \
    qjsonrpcsocket.h \
    qjsonrpcserviceprovider.h \
    qjsonrpcabstractserver.h \
    qjsonrpclocalserver.h \
    qjsonrpctcpserver.h \
    qjsonrpcglobal.h \
    qjsonrpcservicereply.h \
    qjsonrpchttpclient.h \
    qjsonrpchttpserver.h

greaterThan(QT_MAJOR_VERSION, 4) {
    greaterThan(QT_MINOR_VERSION, 1) {
        INSTALL_HEADERS += qjsonrpcmetatype.h
    }
}

HEADERS += \
    $${INSTALL_HEADERS} \
    $${PRIVATE_HEADERS}

SOURCES += \
    qjsonrpcmessage.cpp \
    qjsonrpcservice.cpp \
    qjsonrpcsocket.cpp \
    qjsonrpcserviceprovider.cpp \
    qjsonrpcabstractserver.cpp \
    qjsonrpclocalserver.cpp \
    qjsonrpctcpserver.cpp \
    qjsonrpcservicereply.cpp \
    qjsonrpchttpclient.cpp \
    qjsonrpchttpserver.cpp

# install
#headers.files = $${INSTALL_HEADERS}
#headers.path = $${PREFIX}/include/qjsonrpc
#qjson_headers.files = $${QJSON_INSTALL_HEADERS}
#qjson_headers.path = $${PREFIX}/include/qjsonrpc/json
#private_headers.files = $${PRIVATE_HEADERS}
#private_headers.path = $${PREFIX}/include/qjsonrpc/private
#target.path = $${PREFIX}/$${LIBDIR}
#INSTALLS += headers qjson_headers private_headers target

# pkg-config support
#CONFIG += create_pc create_prl no_install_prl
#QMAKE_PKGCONFIG_DESTDIR = pkgconfig
#QMAKE_PKGCONFIG_LIBDIR = $$target.path
#QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(QJSONRPC_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DQJSONRPC_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DQJSONRPC_SHARED
}
#unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl

