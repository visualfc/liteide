#include "mdtohtml.h"
#include "markdown.h"
#include "buffer.h"
#include "html.h"

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

QByteArray md2html(const QByteArray &data, int ext)
{
    if (data.isEmpty()) {
        return data;
    }

    struct buf *ob;

    struct sd_callbacks callbacks;
    struct html_renderopt options;
    struct sd_markdown *markdown;

    /* performing markdown parsing */
    ob = bufnew(OUTPUT_UNIT);

    sdhtml_renderer(&callbacks, &options, 0);
    markdown = sd_markdown_new(ext, 16, &callbacks, &options);

    sd_markdown_render(ob, (uint8_t*)data.constData(), data.size(), markdown);
    sd_markdown_free(markdown);

    QByteArray out((char*)ob->data,ob->size);
    /* cleanup */
    bufrelease(ob);

    return out;
}

void fix_qt_textbrowser(bool b)
{
    if (b) {
        set_fix_qt_textbrowser(1);
    } else {
        set_fix_qt_textbrowser(0);
    }
}
