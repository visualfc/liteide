#ifndef CMARK_INLINES_H
#define CMARK_INLINES_H

#ifdef __cplusplus
extern "C" {
#endif

cmark_chunk cmark_clean_url(cmark_mem *mem, cmark_chunk *url);
cmark_chunk cmark_clean_title(cmark_mem *mem, cmark_chunk *title);

void cmark_parse_inlines(cmark_mem *mem, cmark_node *parent,
                         cmark_reference_map *refmap, int options);

bufsize_t cmark_parse_reference_inline(cmark_mem *mem, cmark_chunk *input,
                                       cmark_reference_map *refmap);

#ifdef __cplusplus
}
#endif

#endif
