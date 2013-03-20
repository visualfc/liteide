#ifndef MDTOHTML_H
#define MDTOHTML_H

#include <QByteArray>
#include "src/markdown.h"

const int MKDEXT_ALL =
        MKDEXT_NO_INTRA_EMPHASIS|
        MKDEXT_TABLES |
        MKDEXT_FENCED_CODE |
        MKDEXT_AUTOLINK |
        MKDEXT_STRIKETHROUGH |
        MKDEXT_SPACE_HEADERS |
        MKDEXT_SUPERSCRIPT |
        MKDEXT_LAX_SPACING ;

QByteArray md2html(const QByteArray &data, int ext = MKDEXT_ALL);

void fix_qt_textbrowser(bool b);

#endif // MDTOHTML_H
