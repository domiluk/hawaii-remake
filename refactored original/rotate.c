#include <math.h>
#include <allegro.h>

#define scw (SCREEN_W / 2)
#define sch (SCREEN_H / 2)
#define DEG_TO_RADS 0.01745329252 // pi / 180

typedef struct trgb
{
  int r;
  int g;
  int b;
} trgb;

// i - interpolation parameter. has to be between 0 and 1
trgb gra(trgb c1, trgb c2, float i)
{
  trgb c;

  c.r = c1.r * (1 - i) + c2.r * (i);
  c.g = c1.g * (1 - i) + c2.g * (i);
  c.b = c1.b * (1 - i) + c2.b * (i);

  return c;
}

void rot(int x, int y, int center_x, int center_y, float angle_in_rads, float *rx, float *ry)
{
  float tx = x - center_x;
  float ty = y - center_y;

  float cosine = cos(angle_in_rads);
  float sine = sin(angle_in_rads);

  float mx = tx * cosine - ty * sine;
  float my = tx * sine + ty * cosine;

  *rx = mx + center_x;
  *ry = my + center_y;
}

void rotate(BITMAP *bmp, BITMAP *tmp, float angle)
{
  int w = bmp->w;
  int h = bmp->h;

  clear_to_color(tmp, makecol(255, 0, 255));

  float angle_in_rads = -angle * DEG_TO_RADS;

  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
    {
      float rx, ry;
      rot(i, j, w / 2, h / 2, angle_in_rads, &rx, &ry);

      if (rx < 0 || ry < 0 || rx > w - 1 || ry > h - 1)
        putpixel(tmp, i, j, makecol(255, 0, 255));
      else
        putpixel(tmp, i, j, getpixel(bmp, (int)rx, (int)ry));
    }
}

// NOTE: this was dead code, it's not yet ready for use
// Note that the getpixel/putpixel in this version has swapped bitmaps
void rotate_antialiased(BITMAP *bmp, BITMAP *tmp, float angle)
{
  int w = bmp->w;
  int h = bmp->h;

  clear_to_color(tmp, makecol(255, 0, 255));

  float angle_in_rads = -angle * DEG_TO_RADS;

  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
    {
      float rx, ry;
      rot(i, j, w / 2, h / 2, angle_in_rads, &rx, &ry);

      int x = (int)rx;
      int y = (int)ry;

      int p1_ = getpixel(tmp, x, y);
      int p2_ = getpixel(tmp, x + 1, y);
      int p3_ = getpixel(tmp, x + 1, y + 1);
      int p4_ = getpixel(tmp, x, y + 1);

      trgb p1 = {.r = getr(p1_), .g = getg(p1_), .b = getb(p1_)};
      trgb p2 = {.r = getr(p2_), .g = getg(p2_), .b = getb(p2_)};
      trgb p3 = {.r = getr(p3_), .g = getg(p3_), .b = getb(p3_)};
      trgb p4 = {.r = getr(p4_), .g = getg(p4_), .b = getb(p4_)};

      trgb hore = gra(p1, p2, rx - x);
      trgb dole = gra(p4, p3, rx - x);

      trgb p = gra(hore, dole, ry - y);

      putpixel(bmp, i, j, makecol(p.r, p.g, p.b));
    }
}
