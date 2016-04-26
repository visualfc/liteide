# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)
include (../../liteidex.pri)

TEMPLATE  = subdirs

SUBDIRS = \
    welcome \
    liteenv \
    litebuild \
    liteeditor \
    litefind    \
    litedebug   \
    litetty \
    filebrowser \
    golangast \
    golangedit \
    golangfmt \
    golangcode \
    golangdoc \
    golangplay \
    golangpackage \
    golangpresent \
    gdbdebugger \
    markdown \
    jsonedit \
    rustedit \
    dlvdebugger 


contains(DEFINES, LITEIDE_QTWEBKIT) {
    SUBDIRS += webkithtmlwidget
}
