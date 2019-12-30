#include "libcmark.h"
#include "src/cmark.h"

namespace cmark {

QByteArray markdown_to_html(const QByteArray &data)
{
    char *buf = cmark_markdown_to_html(data.data(),size_t(data.size()),CMARK_OPT_UNSAFE);
    QByteArray out(buf);
    free(buf);
    return  out;
}

} //cmark
