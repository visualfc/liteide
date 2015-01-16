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
    webkithtmlwidget \
    rustedit
