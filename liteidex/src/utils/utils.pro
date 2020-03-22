# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = \
    fileutil \
    mimetype \
    extension \
    textoutput \
    symboltreeview \
    modelproject \
    processex \
    colorstyle \
    documentbrowser \
    htmlutil \
    golangapi \
    filesystem \
    editorutil \
    folderview \
    iconutil \
    dlvclient \
    navigate \
    vterm
