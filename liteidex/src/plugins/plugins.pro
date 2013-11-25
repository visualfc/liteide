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
#    makefileproject \
#    profileproject  \
    filebrowser \
    golangast \
#    golangtool \
    goslide \
    golangfmt \
    golangcode \
    golangdoc \
    golangplay \
    golangpackage \
    gdbdebugger \
    markdown \
    webkithtmlwidget \
