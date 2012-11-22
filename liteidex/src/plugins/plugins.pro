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
#    makefileproject \
#    profileproject  \
    filebrowser \
    golangast \
#    golangtool \
    golangfmt \
    golangcode \
    golangdoc \
    golangplay \
    golangpackage \
    gdbdebugger \
    markdown \
    webkithtmlwidget \
