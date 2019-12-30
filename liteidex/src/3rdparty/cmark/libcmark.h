#ifndef LIBCMARK_H
#define LIBCMARK_H

#include <QByteArray>

namespace cmark {

QByteArray markdown_to_html(const QByteArray &data);

}

inline QByteArray md2html(const QByteArray &data) {
    return  cmark::markdown_to_html(data);
}

#endif // LIBCMARK_H
