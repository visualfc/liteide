# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

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
    golangpresent \
    golangfmt \
    golangcode \
    golangdoc \
    golangplay \
    golangpackage \
    gdbdebugger \
    markdown \
    webkithtmlwidget \
    golanglint \
