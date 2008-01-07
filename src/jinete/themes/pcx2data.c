/* Jinete - a GUI library
 * Copyright (C) 2003, 2004, 2005, 2007, 2008 David A. Capello.
 * All rights reserved.
 *
 * Read "LICENSE.txt" for more information.
 */

#ifdef PCX2DATA

#include <allegro.h>
#include <stdio.h>

void pcx2data(const char *filename)
{
  char buf[512];
  PALETTE pal;
  BITMAP *bmp;
  int x, y;

  bmp = load_bitmap(filename, pal);
  if (!bmp)
    return;

  ustrcpy(buf, get_filename(filename));
  if (get_extension(buf))
    *(get_extension(buf)-1) = 0;

  printf("static unsigned char default_theme_%s[%d] = {\n",
	 buf, 2 + bmp->w * bmp->h);

  printf("  %d, %d,\n", bmp->w, bmp->h);

  for (y=0; y<bmp->h; y++) {
    printf("  ");
    for (x=0; x<bmp->w; x++)
      printf("%d%s", getpixel(bmp, x, y), x == bmp->w-1 ? "": ", ");
    printf("%s\n", y == bmp->h-1 ? "": ",");
  }

  printf("};\n\n");
}

int main(int argc, char *argv[])
{
  int c;
  allegro_init();
  printf ("/* Generated by pcx2data */\n\n");
  for (c=1; c<argc; c++) {
    fprintf(stderr, "Converting %s...\n", argv[c]);
    pcx2data(argv[c]);
  }
  return 0;
}

END_OF_MAIN();

#else

static BITMAP *data2bmp(int color_depth, const unsigned char *data, int *cmap)
{
  int x, y, c = 2;
  BITMAP *bmp;

  bmp = create_bitmap_ex(color_depth, data[0], data[1]);
  if (!bmp)
    return NULL;

  for (y=0; y<bmp->h; y++)
    for (x=0; x<bmp->w; x++)
      putpixel(bmp, x, y, cmap[data[c++]]);

  return bmp;
}

#endif
