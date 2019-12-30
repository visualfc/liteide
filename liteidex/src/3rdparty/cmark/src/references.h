#ifndef CMARK_REFERENCES_H
#define CMARK_REFERENCES_H

#include "chunk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REFMAP_SIZE 16

struct cmark_reference {
  struct cmark_reference *next;
  unsigned char *label;
  cmark_chunk url;
  cmark_chunk title;
  unsigned int hash;
};

typedef struct cmark_reference cmark_reference;

struct cmark_reference_map {
  cmark_mem *mem;
  cmark_reference *table[REFMAP_SIZE];
};

typedef struct cmark_reference_map cmark_reference_map;

cmark_reference_map *cmark_reference_map_new(cmark_mem *mem);
void cmark_reference_map_free(cmark_reference_map *map);
cmark_reference *cmark_reference_lookup(cmark_reference_map *map,
                                        cmark_chunk *label);
extern void cmark_reference_create(cmark_reference_map *map, cmark_chunk *label,
                                   cmark_chunk *url, cmark_chunk *title);

#ifdef __cplusplus
}
#endif

#endif
