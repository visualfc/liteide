#ifndef CMARK_NODE_H
#define CMARK_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "cmark.h"
#include "buffer.h"
#include "chunk.h"

typedef struct {
  cmark_list_type list_type;
  int marker_offset;
  int padding;
  int start;
  cmark_delim_type delimiter;
  unsigned char bullet_char;
  bool tight;
} cmark_list;

typedef struct {
  cmark_chunk info;
  cmark_chunk literal;
  uint8_t fence_length;
  uint8_t fence_offset;
  unsigned char fence_char;
  int8_t fenced;
} cmark_code;

typedef struct {
  int level;
  bool setext;
} cmark_heading;

typedef struct {
  cmark_chunk url;
  cmark_chunk title;
} cmark_link;

typedef struct {
  cmark_chunk on_enter;
  cmark_chunk on_exit;
} cmark_custom;

enum cmark_node__internal_flags {
  CMARK_NODE__OPEN = (1 << 0),
  CMARK_NODE__LAST_LINE_BLANK = (1 << 1),
  CMARK_NODE__LAST_LINE_CHECKED = (1 << 2),
};

struct cmark_node {
  cmark_strbuf content;

  struct cmark_node *next;
  struct cmark_node *prev;
  struct cmark_node *parent;
  struct cmark_node *first_child;
  struct cmark_node *last_child;

  void *user_data;

  int start_line;
  int start_column;
  int end_line;
  int end_column;
  int internal_offset;
  uint16_t type;
  uint16_t flags;

  union {
    cmark_chunk literal;
    cmark_list list;
    cmark_code code;
    cmark_heading heading;
    cmark_link link;
    cmark_custom custom;
    int html_block_type;
  } as;
};

static CMARK_INLINE cmark_mem *cmark_node_mem(cmark_node *node) {
  return node->content.mem;
}
CMARK_EXPORT int cmark_node_check(cmark_node *node, FILE *out);

#ifdef __cplusplus
}
#endif

#endif
