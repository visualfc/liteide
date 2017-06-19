# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)
include (../../liteidex.pri)

TEMPLATE  = subdirs

win32 {
SUBDIRS = liteide_ctrlc_stub
}
