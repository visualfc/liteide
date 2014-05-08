# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = \
    liteapi \
    liteenvapi  \
    litefindapi \
    litebuildapi \
    liteeditorapi \
    litedebugapi \
    litettyapi \
    docbrowserapi \
    golangdocapi \
    golangastapi \
