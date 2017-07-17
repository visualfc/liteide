TEMPLATE = subdirs
SUBDIRS += \
    qjsonrpcmessage \
    qjsonrpcsocket \
    qjsonrpcserver \
    qjsonrpcservice \
    qjsonrpchttpclient \
    qjsonrpchttpserver \
    issue22

lessThan(QT_MAJOR_VERSION, 5) {
    SUBDIRS += json
}

greaterThan(QT_MAJOR_VERSION, 4) {
    greaterThan(QT_MINOR_VERSION, 1) {
        SUBDIRS += qjsonrpcmetatype
    }
}
