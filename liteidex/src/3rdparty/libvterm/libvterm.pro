TARGET = libvterm
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)

INCLUDEPATH += include

DISTFILES += \
    src/encoding/DECdrawing.inc \
    src/encoding/DECdrawing.tbl \
    src/encoding/uk.inc \
    src/encoding/uk.tbl \
    src/fullwidth.inc

HEADERS += \
    include/vterm.h \
    include/vterm_keycodes.h \
    src/rect.h \
    src/utf8.h \
    src/vterm_internal.h

SOURCES += \
    src/encoding.c \
    src/keyboard.c \
    src/mouse.c \
    src/parser.c \
    src/pen.c \
    src/screen.c \
    src/state.c \
    src/unicode.c \
    src/vterm.c
