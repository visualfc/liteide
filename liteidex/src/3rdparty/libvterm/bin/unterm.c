#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

#include "vterm.h"

#include "../src/utf8.h" // fill_utf8

#define streq(a,b) (!strcmp(a,b))

static VTerm *vt;
static VTermScreen *vts;

static int cols;
static int rows;

static enum {
  FORMAT_PLAIN,
  FORMAT_SGR,
} format = FORMAT_PLAIN;

static int dump_cell_color(const VTermColor *col, int sgri, int sgr[], int fg)
{
    /* Reset the color if the given color is the default color */
    if (fg && VTERM_COLOR_IS_DEFAULT_FG(col)) {
        sgr[sgri++] = 39;
        return sgri;
    }
    if (!fg && VTERM_COLOR_IS_DEFAULT_BG(col)) {
        sgr[sgri++] = 49;
        return sgri;
    }

    /* Decide whether to send an indexed color or an RGB color */
    if (VTERM_COLOR_IS_INDEXED(col)) {
        const uint8_t idx = col->indexed.idx;
        if (idx < 8) {
            sgr[sgri++] = (idx + (fg ? 30 : 40));
        }
        else if (idx < 16) {
            sgr[sgri++] = (idx - 8 + (fg ? 90 : 100));
        }
        else {
            sgr[sgri++] = (fg ? 38 : 48);
            sgr[sgri++] = 5;
            sgr[sgri++] = idx;
        }
    }
    else if (VTERM_COLOR_IS_RGB(col)) {
        sgr[sgri++] = (fg ? 38 : 48);
        sgr[sgri++] = 2;
        sgr[sgri++] = col->rgb.red;
        sgr[sgri++] = col->rgb.green;
        sgr[sgri++] = col->rgb.blue;
    }
    return sgri;
}

static void dump_cell(const VTermScreenCell *cell, const VTermScreenCell *prevcell)
{
  switch(format) {
    case FORMAT_PLAIN:
      break;
    case FORMAT_SGR:
      {
        // If all 7 attributes change, that means 7 SGRs max
        // Each colour could consume up to 5 entries
        int sgr[7 + 2*5]; int sgri = 0;

        if(!prevcell->attrs.bold && cell->attrs.bold)
          sgr[sgri++] = 1;
        if(prevcell->attrs.bold && !cell->attrs.bold)
          sgr[sgri++] = 22;

        if(!prevcell->attrs.underline && cell->attrs.underline)
          sgr[sgri++] = 4;
        if(prevcell->attrs.underline && !cell->attrs.underline)
          sgr[sgri++] = 24;

        if(!prevcell->attrs.italic && cell->attrs.italic)
          sgr[sgri++] = 3;
        if(prevcell->attrs.italic && !cell->attrs.italic)
          sgr[sgri++] = 23;

        if(!prevcell->attrs.blink && cell->attrs.blink)
          sgr[sgri++] = 5;
        if(prevcell->attrs.blink && !cell->attrs.blink)
          sgr[sgri++] = 25;

        if(!prevcell->attrs.reverse && cell->attrs.reverse)
          sgr[sgri++] = 7;
        if(prevcell->attrs.reverse && !cell->attrs.reverse)
          sgr[sgri++] = 27;

        if(!prevcell->attrs.strike && cell->attrs.strike)
          sgr[sgri++] = 9;
        if(prevcell->attrs.strike && !cell->attrs.strike)
          sgr[sgri++] = 29;

        if(!prevcell->attrs.font && cell->attrs.font)
          sgr[sgri++] = 10 + cell->attrs.font;
        if(prevcell->attrs.font && !cell->attrs.font)
          sgr[sgri++] = 10;

        if(!vterm_color_is_equal(&prevcell->fg, &cell->fg)) {
          sgri = dump_cell_color(&cell->fg, sgri, sgr, 1);
        }

        if(!vterm_color_is_equal(&prevcell->bg, &cell->bg)) {
          sgri = dump_cell_color(&cell->bg, sgri, sgr, 0);
        }

        if(!sgri)
          break;

        printf("\x1b[");
        for(int i = 0; i < sgri; i++)
          printf(!i               ? "%d" :
              CSI_ARG_HAS_MORE(sgr[i]) ? ":%d" :
              ";%d",
              CSI_ARG(sgr[i]));
        printf("m");
      }
      break;
  }

  for(int i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
    char bytes[6];
    bytes[fill_utf8(cell->chars[i], bytes)] = 0;
    printf("%s", bytes);
  }
}

static void dump_eol(const VTermScreenCell *prevcell)
{
  switch(format) {
    case FORMAT_PLAIN:
      break;
    case FORMAT_SGR:
      if(prevcell->attrs.bold || prevcell->attrs.underline || prevcell->attrs.italic ||
         prevcell->attrs.blink || prevcell->attrs.reverse || prevcell->attrs.strike ||
         prevcell->attrs.font)
        printf("\x1b[m");
      break;
  }

  printf("\n");
}

void dump_row(int row)
{
  VTermPos pos = { .row = row, .col = 0 };
  VTermScreenCell prevcell = { 0 };
  vterm_state_get_default_colors(vterm_obtain_state(vt), &prevcell.fg, &prevcell.bg);

  while(pos.col < cols) {
    VTermScreenCell cell;
    vterm_screen_get_cell(vts, pos, &cell);

    dump_cell(&cell, &prevcell);

    pos.col += cell.width;
    prevcell = cell;
  }

  dump_eol(&prevcell);
}

static int screen_sb_pushline(int cols, const VTermScreenCell *cells, void *user)
{
  VTermScreenCell prevcell = { 0 };
  vterm_state_get_default_colors(vterm_obtain_state(vt), &prevcell.fg, &prevcell.bg);

  for(int col = 0; col < cols; col++) {
    dump_cell(cells + col, &prevcell);
    prevcell = cells[col];
  }

  dump_eol(&prevcell);

  return 1;
}

static int screen_resize(int new_rows, int new_cols, void *user)
{
  rows = new_rows;
  cols = new_cols;
  return 1;
}

static VTermScreenCallbacks cb_screen = {
  .sb_pushline = &screen_sb_pushline,
  .resize      = &screen_resize,
};

int main(int argc, char *argv[])
{
  rows = 25;
  cols = 80;

  int opt;
  while((opt = getopt(argc, argv, "f:l:c:")) != -1) {
    switch(opt) {
      case 'f':
        if(streq(optarg, "plain"))
          format = FORMAT_PLAIN;
        else if(streq(optarg, "sgr"))
          format = FORMAT_SGR;
        else {
          fprintf(stderr, "Unrecognised format '%s'\n", optarg);
          exit(1);
        }
        break;

      case 'l':
        rows = atoi(optarg);
        if(!rows)
          rows = 25;
        break;

      case 'c':
        cols = atoi(optarg);
        if(!cols)
          cols = 80;
        break;
    }
  }

  const char *file = argv[optind++];
  int fd = open(file, O_RDONLY);
  if(fd == -1) {
    fprintf(stderr, "Cannot open %s - %s\n", file, strerror(errno));
    exit(1);
  }

  vt = vterm_new(rows, cols);
  vterm_set_utf8(vt, true);

  vts = vterm_obtain_screen(vt);
  vterm_screen_set_callbacks(vts, &cb_screen, NULL);

  vterm_screen_reset(vts, 1);

  int len;
  char buffer[1024];
  while((len = read(fd, buffer, sizeof(buffer))) > 0) {
    vterm_input_write(vt, buffer, len);
  }

  for(int row = 0; row < rows; row++) {
    dump_row(row);
  }

  close(fd);

  vterm_free(vt);

  return 0;
}
