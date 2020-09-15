QJSONRPC_VERSION = 1.1.0

QJSONRPC_LIBRARY_TYPE = staticlib

isEmpty(QJSONRPC_LIBRARY_TYPE) {
    QJSONRPC_LIBRARY_TYPE = shared
}

private-headers {
    DEFINES += USE_QT_PRIVATE_HEADERS
    QT += core-private
}

QT += network
QJSONRPC_INCLUDEPATH = $${PWD}/src

contains(QJSONRPC_LIBRARY_TYPE, staticlib) {
    DEFINES += QJSONRPC_STATIC
} else {
    DEFINES += QJSONRPC_SHARED
}

isEmpty(PREFIX) {
    unix {
        PREFIX = /usr
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}
isEmpty(LIBDIR) {
    LIBDIR = lib
}

 LIBS *= -l$$qtLibraryName(qjsonrpc)
