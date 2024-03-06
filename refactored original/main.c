#include <allegro.h>
#include <alfont.h>
#include <math.h>
#include <stdio.h>

#define MODE_MULTIPLAYER 0
#define MODE_CAREER 1
#define MODE_PRACTICE 2

#define DEG_TO_RADS 0.01745329252 // just multiply your degrees by this constant
#define RADS_TO_DEG 57.2957795147 // just multiply your radians by this constant

void rotate(BITMAP *bmp, BITMAP *tmp, float angle);

int camup1 = 0, camup2 = 0, camleft2 = 0, camleft1 = 0;
BITMAP *ostrov, *vsetko;
BITMAP *alpha;
BITMAP *white_point_bmp, *rotated_white_point_bmp;
BITMAP *panel;
SAMPLE *lap_gong, *spring, *main_sample;
int game_mode;
int global_sec;
int global_min;

// these are set in options, never used anyway and never stored to a file or read from a file
int res = 0, depth = 16, vol = 255, nlaps = 3, player1_boat_color = 0, player2_boat_color = 1;
int winning_laps = 3;

enum Scene
{
  MAIN_MENU,
  PLAY_MENU,
  OPTIONS_MENU,
  CREDITS_MENU,
  GAME,
  EXIT
};

enum Scene main_menu_loop();
enum Scene play_menu_loop();
enum Scene credits_menu_loop();

typedef struct boat
{
  float x;
  float y;
  float xv;
  float yv;
  float rot;
  float rotate;
  float maxspeed;
  float speedup;
  float slowdown;
  int laps;
  int checkpoint_one;
  int checkpoint_two;
  int checkpoint_three;
  int last_lap_sec;
  int best_lap_sec;
  int last_lap_min;
  int best_lap_min;
  int AI;
  BITMAP *bmp;
  BITMAP *bmp_rot;
} BOAT;

BOAT player1, player2;
BITMAP *mb, *menu;

// BITMAP *boat, *ms, *boatr;
int AI_pos = 0;
int npts = 50;
int xpos[100 * 26];
int ypos[100 * 26];
int endofgame = 0;
int lastx = 288, lasty = 30;
int pp = 0;
int pots[26][8] = {{966, 1086, 997, 1011, 1026, 966, 1061, 928},
                   {1061, 928, 1108, 873, 1111, 832, 1100, 773},
                   {1100, 773, 1089, 707, 1072, 661, 1048, 617},
                   {1048, 617, 1028, 553, 1016, 522, 991, 473},
                   {991, 473, 967, 428, 942, 393, 898, 359},
                   {898, 359, 866, 324, 819, 295, 769, 287},
                   {769, 287, 709, 270, 643, 269, 582, 278},
                   {582, 278, 522, 282, 457, 297, 406, 321},
                   {406, 321, 340, 340, 280, 366, 246, 410},
                   {246, 410, 185, 468, 166, 527, 175, 596},
                   {175, 596, 162, 683, 145, 759, 158, 842},
                   {158, 842, 162, 936, 167, 1016, 194, 1115},
                   {194, 1115, 244, 1215, 304, 1265, 417, 1298},
                   {417, 1298, 538, 1321, 611, 1327, 726, 1304},
                   {726, 1304, 833, 1287, 905, 1265, 1013, 1217},
                   {1013, 1217, 1083, 1163, 1160, 1128, 1258, 1100},
                   {1258, 1100, 1342, 1059, 1411, 1053, 1520, 1079},
                   {1520, 1079, 1586, 1097, 1624, 1107, 1678, 1142},
                   {1678, 1142, 1736, 1183, 1750, 1208, 1755, 1269},
                   {1755, 1269, 1798, 1331, 1800, 1382, 1791, 1440},
                   {1791, 1440, 1785, 1551, 1760, 1550, 1712, 1578},
                   {1712, 1578, 1657, 1623, 1620, 1646, 1552, 1670},
                   {1552, 1670, 1479, 1708, 1414, 1707, 1334, 1698},
                   {1334, 1698, 1199, 1689, 1130, 1647, 1093, 1594},
                   {1093, 1594, 1018, 1518, 987, 1446, 977, 1350},
                   {977, 1350, 952, 1236, 936, 1175, 966, 1086}};

void calc_AI()
{
  int xs[100 + 1];
  int ys[100 + 1];

  for (int curspl = 0; curspl < 9; curspl++)
  {
    calc_spline(pots[curspl], npts + 1, xs, ys);

    for (int curpt = 0; curpt < npts; curpt++)
    {
      xpos[(curspl * npts) + curpt] = xs[curpt];
      ypos[(curspl * npts) + curpt] = ys[curpt];
    }
  }
}

int getAI_x(int pos)
{
  return (xpos[pos]);
}

int getAI_y(int pos)
{
  return (ypos[pos]);
}

float getAI_rot(int pos)
{
  float beta;
  float xres, yres;

  // REFACTOR: DELETE LINE: curspl = curpt / npts;
  // if (curpt == 0) // NOTE: curpt is never assigned anything else than 0
  // {
  xres = xpos[0] - xpos[(9 * npts) - 1];
  yres = ypos[0] - ypos[(9 * npts) - 1];
  // }
  // else
  // {
  //   xres = xpos[curpt] - xpos[curpt - 1];
  //   yres = ypos[curpt] - ypos[curpt - 1];
  // }
  if (xres != 0)
  {
    beta = atan(yres / xres);
    beta = beta * RADS_TO_DEG;
  }

  return beta;
}

float getAI_xres(int pos)
{
  float xres;
  // REFACTOR: DELETE LINE: curspl = curpt / npts;
  // if (curpt == 0) // NOTE: curpt is never assigned anything else than 0
  // {
  xres = xpos[0] - xpos[(9 * npts) - 1];
  // }
  // else
  // {
  //   xres = xpos[curpt] - xpos[curpt - 1];
  // }

  return xres;
}

float getAI_yres(int pos)
{
  float yres;
  // REFACTOR: DELETE LINE: curspl = curpt / npts;
  // if (curpt == 0) // NOTE: curpt is never assigned anything else than 0
  // {
  yres = ypos[0] - ypos[(9 * npts) - 1];
  // }
  // else
  // {
  //   yres = ypos[curpt] - ypos[curpt - 1];
  // }

  return yres;
}

void mooove_time()
{
  global_sec++;
  if (global_sec == 60)
  {
    global_min++;
    global_sec = 0;
  }
}

ALFONT_FONT *pump;
int main()
{
  srand(time(NULL));
  allegro_init();
  alfont_init();
  install_keyboard();
  install_mouse();
  install_timer();
  set_color_depth(desktop_color_depth());
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
  install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
  alfont_text_mode(-1);

  calc_AI();

  pump = alfont_load_font("airstream.ttf");

  mb = create_bitmap(SCREEN_W, SCREEN_H);
  vsetko = create_bitmap(2100, 1900);

  player1.x = 996 - 100;
  player1.y = 1025 - 100;
  player1.laps = 0;
  player1.checkpoint_one = 0;
  player1.checkpoint_two = 0;
  player1.checkpoint_three = 0;
  player1.xv = 0;
  player1.yv = 0;
  player1.rot = -50;
  player1.last_lap_sec = 0;
  player1.last_lap_min = 0;
  player1.best_lap_sec = 99;
  player1.best_lap_min = 99;
  player1.maxspeed = 10;
  player1.speedup = 0.05;
  player1.slowdown = 0.08;
  player1.rotate = 1.5;

  player2.x = 1105 - 100;
  player2.y = 1087 - 100;
  player2.laps = 0;
  player2.checkpoint_one = 0;
  player2.checkpoint_two = 0;
  player2.checkpoint_three = 0;
  player2.xv = 0;
  player2.yv = 0;
  player2.rot = -75;
  player2.last_lap_sec = 0;
  player2.last_lap_min = 0;
  player2.best_lap_sec = 99;
  player2.best_lap_min = 99;
  player2.maxspeed = 10;
  player2.speedup = 0.05;
  player2.slowdown = 0.08;
  player2.rotate = 1.5;

  white_point_bmp = create_bitmap(100, 100);
  clear_bitmap(white_point_bmp);
  putpixel(white_point_bmp, 97, 50, makecol(254, 255, 255));
  putpixel(white_point_bmp, 96, 50, makecol(254, 255, 255));

  rotated_white_point_bmp = create_bitmap(100, 100);
  clear_bitmap(rotated_white_point_bmp);

  player1.bmp = load_bitmap("lodcervena.bmp", NULL);
  player1.bmp_rot = load_bitmap("lodcervena.bmp", NULL);
  player2.bmp = load_bitmap("lodzelena.bmp", NULL);
  player2.bmp_rot = load_bitmap("lodzelena.bmp", NULL);
  ostrov = load_bitmap("ostrov1.bmp", NULL);
  alpha = load_bitmap("alpha1.bmp", NULL);
  menu = load_bitmap("menu.bmp", NULL);
  panel = load_bitmap("panel.bmp", NULL);
  lap_gong = load_sample("sounds/dray.wav");
  spring = load_sample("sounds/spring.wav");
  main_sample = load_sample("main.wav");

  alfont_set_font_size(pump, 50);

  enum Scene next_scene = MAIN_MENU;

scene_switch:
  switch (next_scene)
  {
  case MAIN_MENU:
    goto main_menu;
  case PLAY_MENU:
    goto play_menu;
  case OPTIONS_MENU:
    goto options_menu;
  case CREDITS_MENU:
    goto credits_menu;
  case GAME:
    goto game;
  case EXIT:
    goto exit;
  }

main_menu:
  next_scene = main_menu_loop();
  goto scene_switch;

play_menu:
  next_scene = play_menu_loop();
  goto scene_switch;

options_menu:
  while (1)
  {
    show_mouse(NULL);
    blit(menu, mb, 0, 0, 0, 0, 1024, 768);
    alfont_set_font_size(pump, 50);

    if (mouse_x > 162 && mouse_y > 588 && mouse_x < 258 && mouse_y < 657)
    {
      alfont_textprintf_centre_aa(mb, pump, 211, 608, 0xFFFFFF, "Play");
      if (mouse_b & 1)
        goto play_menu;
    }
    else
      alfont_textprintf_centre_aa(mb, pump, 211, 608, 0, "Play");

    if (mouse_x > 666 && mouse_y > 601 && mouse_x < 756 && mouse_y < 658)
    {
      alfont_textprintf_centre_aa(mb, pump, 707, 608, 0xFFFFFF, "Exit");
      if (mouse_b & 1)
        goto exit;
    }
    else
      alfont_textprintf_centre_aa(mb, pump, 707, 608, 0, "Exit");

    alfont_set_font_size(pump, 35);

    alfont_textprintf_centre_aa(mb, pump, 357, 608, 0xFFFFFF, "Options");

    if (mouse_x > 546 && mouse_y > 597 && mouse_x < 635 && mouse_y < 656)
    {
      alfont_textprintf_centre_aa(mb, pump, 586, 608, 0xFFFFFF, "Credits");
      if (mouse_b & 1)
        goto credits_menu;
    }
    else
      alfont_textprintf_centre_aa(mb, pump, 586, 608, 0, "Credits");

    alfont_set_font_size(pump, 32);
    alfont_textout_centre_aa(mb, pump, "IN GAME OPTIONS", 764, 20, 0xFFFFFF);
    alfont_textout_centre_aa(mb, pump, "SETTINGS", 764, 290, 0xFFFFFF);
    alfont_set_font_size(pump, 30);
    alfont_textout_centre_aa(mb, pump, "Number of laps", 700, 60, 0);
    rect(mb, 800, 60, 820, 80, 0);
    rect(mb, 840, 60, 860, 80, 0);
    rect(mb, 880, 60, 900, 80, 0);
    alfont_textout_centre_aa(mb, pump, "3", 830, 60, 0);
    alfont_textout_centre_aa(mb, pump, "5", 870, 60, 0);
    alfont_textout_centre_aa(mb, pump, "7", 910, 60, 0);
    alfont_textout_centre_aa(mb, pump, "Color of Player's boat", 700, 90, 0);
    alfont_textout_centre_aa(mb, pump, "Color of CPU's boat", 700, 190, 0);
    rect(mb, 800, 90, 820, 110, 0);
    rect(mb, 800, 190, 820, 210, 0);
    rect(mb, 800, 120, 820, 140, 0);
    rect(mb, 800, 220, 820, 240, 0);
    rect(mb, 800, 150, 820, 170, 0);
    rect(mb, 800, 250, 820, 270, 0);
    alfont_textout_aa(mb, pump, "Red", 830, 90, 0);
    alfont_textout_aa(mb, pump, "Green", 830, 120, 0);
    alfont_textout_aa(mb, pump, "Blue", 830, 150, 0);
    alfont_textout_aa(mb, pump, "Red", 830, 190, 0);
    alfont_textout_aa(mb, pump, "Green", 830, 220, 0);
    alfont_textout_aa(mb, pump, "Blue", 830, 250, 0);
    alfont_textout_centre_aa(mb, pump, "Resolution", 700, 330, 0);
    alfont_textout_centre_aa(mb, pump, "Color Depth", 700, 420, 0);
    alfont_textout_centre_aa(mb, pump, "Volume", 700, 510, 0);
    rect(mb, 800, 330, 820, 350, 0);
    rect(mb, 800, 360, 820, 380, 0);
    rect(mb, 800, 390, 820, 410, 0);
    alfont_textout_aa(mb, pump, "800x600", 830, 330, 0);
    alfont_textout_aa(mb, pump, "1024x768", 830, 360, 0);
    alfont_textout_aa(mb, pump, "1280x1024", 830, 390, 0);
    rect(mb, 800, 420, 820, 440, 0);
    rect(mb, 800, 450, 820, 470, 0);
    rect(mb, 800, 480, 820, 500, 0);
    alfont_textout_aa(mb, pump, "16", 830, 420, 0);
    alfont_textout_aa(mb, pump, "24", 830, 450, 0);
    alfont_textout_aa(mb, pump, "32", 830, 480, 0);
    rect(mb, 800, 510, 820, 530, 0);
    rect(mb, 800, 540, 820, 560, 0);
    rect(mb, 800, 570, 820, 590, 0);
    alfont_textout_aa(mb, pump, "Mute", 830, 510, 0);
    alfont_textout_aa(mb, pump, "Normal", 830, 540, 0);
    alfont_textout_aa(mb, pump, "Loud", 830, 570, 0);

    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 60 && mouse_y < 80 && mouse_b & 1)
      nlaps = 3;
    if (mouse_x > 840 && mouse_x < 860 && mouse_y > 60 && mouse_y < 80 && mouse_b & 1)
      nlaps = 5;
    if (mouse_x > 880 && mouse_x < 900 && mouse_y > 60 && mouse_y < 80 && mouse_b & 1)
      nlaps = 7;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 90 && mouse_y < 110 && mouse_b & 1)
      player1_boat_color = 0;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 120 && mouse_y < 140 && mouse_b & 1)
      player1_boat_color = 1;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 150 && mouse_y < 170 && mouse_b & 1)
      player1_boat_color = 2;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 190 && mouse_y < 210 && mouse_b & 1)
      player2_boat_color = 0;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 220 && mouse_y < 240 && mouse_b & 1)
      player2_boat_color = 1;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 250 && mouse_y < 270 && mouse_b & 1)
      player2_boat_color = 2;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 330 && mouse_y < 350 && mouse_b & 1)
      res = 0;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 360 && mouse_y < 380 && mouse_b & 1)
      res = 1;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 390 && mouse_y < 410 && mouse_b & 1)
      res = 2;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 420 && mouse_y < 440 && mouse_b & 1)
      depth = 0;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 450 && mouse_y < 470 && mouse_b & 1)
      depth = 1;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 480 && mouse_y < 490 && mouse_b & 1)
      depth = 2;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 510 && mouse_y < 530 && mouse_b & 1)
      vol = 0;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 540 && mouse_y < 560 && mouse_b & 1)
      vol = 180;
    if (mouse_x > 800 && mouse_x < 820 && mouse_y > 570 && mouse_y < 590 && mouse_b & 1)
      vol = 255;

    if (nlaps == 3)
      alfont_textout_aa(mb, pump, "X", 800, 60, 0);
    else if (nlaps == 5)
      alfont_textout_aa(mb, pump, "X", 840, 60, 0);
    else
      alfont_textout_aa(mb, pump, "X", 880, 60, 0);
    if (player1_boat_color == 0)
      alfont_textout_aa(mb, pump, "X", 800, 90, 0);
    else if (player1_boat_color == 1)
      alfont_textout_aa(mb, pump, "X", 800, 120, 0);
    else
      alfont_textout_aa(mb, pump, "X", 800, 150, 0);
    if (player2_boat_color == 0)
      alfont_textout_aa(mb, pump, "X", 800, 190, 0);
    else if (player2_boat_color == 1)
      alfont_textout_aa(mb, pump, "X", 800, 220, 0);
    else
      alfont_textout_aa(mb, pump, "X", 800, 250, 0);
    if (res == 0)
      alfont_textout_aa(mb, pump, "X", 800, 330, 0);
    else if (res == 1)
      alfont_textout_aa(mb, pump, "X", 800, 360, 0);
    else
      alfont_textout_aa(mb, pump, "X", 800, 390, 0);
    if (depth == 0)
      alfont_textout_aa(mb, pump, "X", 800, 420, 0);
    else if (depth == 1)
      alfont_textout_aa(mb, pump, "X", 800, 450, 0);
    else
      alfont_textout_aa(mb, pump, "X", 800, 480, 0);
    if (vol == 0)
      alfont_textout_aa(mb, pump, "X", 800, 510, 0);
    else if (vol == 180)
      alfont_textout_aa(mb, pump, "X", 800, 540, 0);
    else
      alfont_textout_aa(mb, pump, "X", 800, 570, 0);

    alfont_set_font_size(pump, 75);
    alfont_textprintf_centre_aa(mb, pump, 512, 100, 0, "Hawaii");

    show_mouse(mb);
    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }

credits_menu:
  next_scene = credits_menu_loop();
  goto scene_switch;

game:
  while (!key[KEY_ESC])
  {

    rotate(white_point_bmp, rotated_white_point_bmp, player1.rot);
    int boat_front_x, boat_front_y;
    for (int rx = 0; rx < rotated_white_point_bmp->w; rx++)
      for (int ry = 0; ry < rotated_white_point_bmp->h; ry++)
      {
        if (getr(getpixel(rotated_white_point_bmp, rx, ry)) == 254)
        {
          boat_front_x = rx;
          boat_front_y = ry;
        }
      }

    int boat_front_point_color = getr(getpixel(alpha, player1.x + boat_front_x, player1.y + boat_front_y));

    int player1_hit_the_land = boat_front_point_color == 0;
    if (player1_hit_the_land)
    {
      player1.xv *= -0.75;
      player1.yv *= -0.75;
    }

    int player1_hit_checkpoint_one = boat_front_point_color == 64;
    if (player1_hit_checkpoint_one)
      player1.checkpoint_one = 1;

    int player1_hit_checkpoint_two = boat_front_point_color == 128;
    if (player1_hit_checkpoint_two)
      player1.checkpoint_two = 1;

    int player1_hit_checkpoint_three = boat_front_point_color == 32;
    if (player1_hit_checkpoint_three)
      player1.checkpoint_three = 1;

    int player1_hit_finish_line = boat_front_point_color == 192;
    if (player1_hit_finish_line && player1.checkpoint_one && player1.checkpoint_two && player1.checkpoint_three)
    {
      player1.checkpoint_one = 0;
      player1.checkpoint_two = 0;
      player1.checkpoint_three = 0;
      player1.last_lap_sec = global_sec - player1.last_lap_sec;
      player1.last_lap_min = global_min - player1.last_lap_min;
      if (player1.last_lap_sec < 0)
      {
        player1.last_lap_min--;
        player1.last_lap_sec = 60 - abs(player1.last_lap_sec);
      }
      if (player1.last_lap_sec + (player1.last_lap_min) * 60 < player1.best_lap_sec + (player1.best_lap_min) * 60)
      {
        player1.best_lap_sec = player1.last_lap_sec;
        player1.best_lap_min = player1.last_lap_min;
      }
      player1.laps++;
      play_sample(lap_gong, 255, 128, 1000, 0);
    }

    if (key[KEY_UP]) // && getr(getpixel(alpha,boat.x + boat_front_x, boat.y + boat_front_y)) == 255)
    {
      player1.x += cos(player1.rot * DEG_TO_RADS) * player1.xv;
      player1.y += sin(player1.rot * DEG_TO_RADS) * player1.yv;
      if (player1.xv < player1.maxspeed && player1.yv < player1.maxspeed)
      {
        player1.xv += player1.speedup;
        player1.yv += player1.speedup;
      }

      if (key[KEY_LEFT])
      {
        player1.rot -= player1.rotate;
      }

      if (key[KEY_RIGHT])
      {
        player1.rot += player1.rotate;
      }
    }
    else
    {
      player1.x += cos(player1.rot * DEG_TO_RADS) * player1.xv;
      player1.y += sin(player1.rot * DEG_TO_RADS) * player1.yv;

      if (key[KEY_DOWN])
      {
        if (player1.xv > player1.slowdown)
          player1.xv -= player1.slowdown;
        if (player1.yv > player1.slowdown)
          player1.yv -= player1.slowdown;
        if (player1.xv < -player1.slowdown)
          player1.xv += player1.slowdown;
        if (player1.yv < -player1.slowdown)
          player1.yv += player1.slowdown;
      }

      if (player1.xv > player1.slowdown)
        player1.xv -= player1.slowdown;
      if (player1.yv > player1.slowdown)
        player1.yv -= player1.slowdown;
      if (player1.xv < -player1.slowdown)
        player1.xv += player1.slowdown;
      if (player1.yv < -player1.slowdown)
        player1.yv += player1.slowdown;

      if (key[KEY_LEFT])
      {
        player1.rot -= player1.rotate;
      }

      if (key[KEY_RIGHT])
      {
        player1.rot += player1.rotate;
      }
    }

    if (game_mode == MODE_MULTIPLAYER)
    {
      rotate(white_point_bmp, rotated_white_point_bmp, player2.rot);

      for (int rx = 0; rx < rotated_white_point_bmp->w; rx++)
        for (int ry = 0; ry < rotated_white_point_bmp->h; ry++)
        {
          if (getr(getpixel(rotated_white_point_bmp, rx, ry)) == 254)
          {
            boat_front_x = rx;
            boat_front_y = ry;
          }
        }

      boat_front_point_color = getr(getpixel(alpha, player2.x + boat_front_x, player2.y + boat_front_y));

      int player2_hit_the_land = boat_front_point_color == 0;
      if (player2_hit_the_land)
      {
        player2.xv *= -0.75;
        player2.yv *= -0.75;
      }

      int player2_hit_checkpoint_one = boat_front_point_color == 64;
      if (player2_hit_checkpoint_one)
        player2.checkpoint_one = 1;

      int player2_hit_checkpoint_two = boat_front_point_color == 128;
      if (player2_hit_checkpoint_two)
        player2.checkpoint_two = 1;

      int player2_hit_checkpoint_three = boat_front_point_color == 32;
      if (player2_hit_checkpoint_three)
        player2.checkpoint_three = 1;

      int player2_hit_finish_line = boat_front_point_color == 192;
      if (player2_hit_finish_line && player2.checkpoint_one && player2.checkpoint_two && player2.checkpoint_three)
      {
        player2.checkpoint_one = 0;
        player2.checkpoint_two = 0;
        player2.checkpoint_three = 0;
        player2.last_lap_sec = global_sec - player2.last_lap_sec;
        player2.last_lap_min = global_min - player2.last_lap_min;
        if (player2.last_lap_sec < 0)
        {
          player2.last_lap_min--;
          player2.last_lap_sec = 60 - abs(player2.last_lap_sec);
        }
        if (player2.last_lap_sec + (player2.last_lap_min) * 60 < player2.best_lap_sec + (player2.best_lap_min) * 60)
        {
          player2.best_lap_sec = player2.last_lap_sec;
          player2.best_lap_min = player2.last_lap_min;
        }
        player2.laps++;
        play_sample(lap_gong, 255, 128, 1000, 0);
      }

      if (key[KEY_W]) // && getr(getpixel(alpha,boat.x + boat_front_x, boat.y + boat_front_y)) == 255)
      {
        player2.x += cos(player2.rot * DEG_TO_RADS) * player2.xv;
        player2.y += sin(player2.rot * DEG_TO_RADS) * player2.yv;
        if (player2.xv < player2.maxspeed && player2.yv < player2.maxspeed)
        {
          player2.xv += player2.speedup;
          player2.yv += player2.speedup;
        }

        if (key[KEY_A])
        {
          player2.rot -= player2.rotate;
        }

        if (key[KEY_D])
        {
          player2.rot += player2.rotate;
        }
      }
      else
      {
        player2.x += cos(player2.rot * DEG_TO_RADS) * player2.xv;
        player2.y += sin(player2.rot * DEG_TO_RADS) * player2.yv;

        if (key[KEY_S])
        {
          if (player2.xv > player2.slowdown)
            player2.xv -= player2.slowdown;
          if (player2.yv > player2.slowdown)
            player2.yv -= player2.slowdown;
          if (player2.xv < -player2.slowdown)
            player2.xv += player2.slowdown;
          if (player2.yv < -player2.slowdown)
            player2.yv += player2.slowdown;
        }

        if (player2.xv > player2.slowdown)
          player2.xv -= player2.slowdown;
        if (player2.yv > player2.slowdown)
          player2.yv -= player2.slowdown;
        if (player2.xv < -player2.slowdown)
          player2.xv += player2.slowdown;
        if (player2.yv < -player2.slowdown)
          player2.yv += player2.slowdown;

        if (key[KEY_A])
        {
          player2.rot -= player2.rotate;
        }

        if (key[KEY_D])
        {
          player2.rot += player2.rotate;
        }
      }
    } // if mode = MODE_MULTIPLAYER

    if (game_mode == MODE_MULTIPLAYER)
    {
      // lava polka obrazovky
      camleft1 = player1.x - 256;
      camup1 = player1.y - 384;

      if (camleft1 < 0)
        camleft1 = 0;
      if (camup1 < 0)
        camup1 = 0;
      if (camup1 > (ostrov->h - 768))
        camup1 = ostrov->h - 768;
      if (camleft1 > (ostrov->w - 1024 + 512))
        camleft1 = ostrov->w - 1024 + 512;
      // 1536       1024
      blit(ostrov, vsetko, 0, 0, 0, 0, 2100, 1900);
      rotate(player1.bmp, player1.bmp_rot, player1.rot + 90);
      draw_sprite(vsetko, player1.bmp_rot, player1.x, player1.y);

      // prava polka obrazovky
      camleft2 = player2.x - 256;
      camup2 = player2.y - 384;

      if (camleft2 < 0)
        camleft2 = 0;
      if (camup2 < 0)
        camup2 = 0;
      if (camup2 > (ostrov->h - 768))
        camup2 = ostrov->h - 768;
      if (camleft2 > (ostrov->w - 1024 + 512))
        camleft2 = ostrov->w - 1024 + 512;

      rotate(player2.bmp, player2.bmp_rot, player2.rot + 90);
      draw_sprite(vsetko, player2.bmp_rot, player2.x, player2.y);

      // vykresli lavu a pravu polku z bitmapy vsetko
      blit(vsetko, mb, camleft1, camup1, 0, 0, 512, 768);
      blit(vsetko, mb, camleft2, camup2, 512, 0, 512, 768);
      vline(mb, 512, 0, 768, makecol(rand() % 255, 0, 0));
    }
    if (game_mode == MODE_PRACTICE || game_mode == MODE_CAREER)
    {
      camleft1 = player1.x - 512;
      camup1 = player1.y - 384;
      if (camleft1 < 0)
        camleft1 = 0;
      if (camup1 < 0)
        camup1 = 0;
      if (camup1 > (ostrov->h - 768))
        camup1 = ostrov->h - 768;
      if (camleft1 > (ostrov->w - 1024))
        camleft1 = ostrov->w - 1024;
      blit(ostrov, mb, camleft1, camup1, 0, 0, camleft1 + 1024, camup1 + 768);
      rotate(player1.bmp, player1.bmp_rot, player1.rot + 90);
      draw_sprite(mb, player1.bmp_rot, player1.x - camleft1, player1.y - camup1);
      // /* REFACTOR: ATTEMPT TO UNDERSTAND white_point_bmp and rotated_white_point_bmp */
      // blit(white_point_bmp, mb, 0, 0, 300, 300, 100, 100);
      // blit(rotated_white_point_bmp, mb, 0, 0, 500, 300, 100, 100);
      // /* /END */
    }

    if (game_mode == MODE_CAREER)
    {
      player2.x = getAI_x(AI_pos);
      player2.y = getAI_y(AI_pos);
      player2.rot = getAI_rot(AI_pos);
      if (player2.xv > 0.2 || player2.yv > 0.2)
      {
        player2.x += player2.xv;
        player2.y += player2.yv;
        if (player2.xv > player2.slowdown)
          player2.xv -= player2.slowdown;
        if (player2.yv > player2.slowdown)
          player2.yv -= player2.slowdown;
        if (player2.xv < -player2.slowdown)
          player2.xv += player2.slowdown;
        if (player2.yv < -player2.slowdown)
          player2.yv += player2.slowdown;
      }
      rotate(player2.bmp, player2.bmp_rot, player2.rot);
      draw_sprite(mb, player2.bmp_rot, player2.x - camleft1, player2.y - camup1);
    }
    // rest(40);

    int boats_distance_less_than_90 = ((player1.x - player2.x) * (player1.x - player2.x)) + ((player1.y - player2.y) * (player1.y - player2.y)) <= 90 * 90;
    if (boats_distance_less_than_90)
    {
      // player1.xv = -getAI_xres(AI_pos)/3;
      // player1.yv = getAI_yres(AI_pos)/3;
      stop_sample(spring);
      play_sample(spring, 255, 128, 1000, 0);
      if (game_mode == MODE_MULTIPLAYER)
      {
        float temp;
        temp = player1.rot;
        player1.rot = player2.rot;
        player2.rot = temp;

        temp = player1.xv;
        player1.xv = player2.xv;
        player2.xv = temp;

        temp = player1.yv;
        player1.yv = player2.yv;
        player2.yv = temp;
      }
      if (game_mode == MODE_CAREER)
      {
        player1.xv = cos(getAI_rot(AI_pos) * DEG_TO_RADS) * 5;
        player1.yv = sin(getAI_rot(AI_pos) * DEG_TO_RADS) * 5;
        player1.rot = getAI_rot(AI_pos);

        player2.xv = cos(player1.rot * DEG_TO_RADS);
        player2.yv = sin(player1.rot * DEG_TO_RADS);
      }
      // xpos[AI_pos] += cos(boat.rot* DEG_TO_RADS)*5;
      // ypos[AI_pos] += sin(boat.rot* DEG_TO_RADS)*5;
      // calc_AI();
    }

    if (game_mode == MODE_CAREER)
    {
      AI_pos += 3;
      if (AI_pos == npts - 1)
      {
        AI_pos = 0;
        // REFACTOR: DELETE LINE: curspl++;
        player2.laps++;
      }
    }

    draw_sprite(mb, panel, 512 - 100, 0);
    alfont_set_font_size(pump, 75);
    alfont_textprintf_centre_aa(mb, pump, 512, 0, 0xFFFFFF, ":");
    alfont_textprintf_centre_aa(mb, pump, 555, 0, 0xFFFFFF, "%d", global_sec);
    alfont_textprintf_centre_aa(mb, pump, 472, 0, 0xFFFFFF, "%d", global_min);
    alfont_set_font_size(pump, 20);
    alfont_textprintf_centre_aa(mb, pump, 512, 70, 0xFFFFFF, "powered by DL games");
    alfont_set_font_size(pump, 35);
    alfont_textprintf_aa(mb, pump, 20, 10, 0, "Laps %d", player1.laps);
    alfont_textprintf_aa(mb, pump, 20, 40, 0, "Last lap time %d:%d", player1.last_lap_min, player1.last_lap_sec);
    alfont_textprintf_aa(mb, pump, 20, 70, 0, "Best lap time %d:%d", player1.best_lap_min, player1.best_lap_sec);
    if (game_mode == MODE_MULTIPLAYER)
    {
      alfont_textprintf_aa(mb, pump, 810, 10, 0, "Laps %d", player2.laps);
      alfont_textprintf_aa(mb, pump, 810, 40, 0, "Last lap time %d:%d", player2.last_lap_min, player2.last_lap_sec);
      alfont_textprintf_aa(mb, pump, 810, 70, 0, "Best lap time %d:%d", player2.best_lap_min, player2.best_lap_sec);
    }

    if (game_mode != MODE_PRACTICE)
    {
      if (player1.laps == winning_laps)
      {
        clear_to_color(mb, 0);
        alfont_set_font_size(pump, 75);
        alfont_textprintf_centre_aa(mb, pump, 512, 384, 0xFFFFFF, "The winner is...!");
        alfont_set_font_size(pump, 80);
        alfont_textprintf_centre_aa(mb, pump, 512, 434, 0xFFFFFF, "Player no.1");
        blit(mb, screen, 0, 0, 0, 0, 1024, 768);
        //       rest(3000);
        if (key[KEY_ESC])
          goto main_menu;
      }
      if (player2.laps == winning_laps)
      {
        clear_to_color(mb, 0);
        alfont_set_font_size(pump, 75);
        alfont_textprintf_centre_aa(mb, pump, 512, 384, 0xFFFFFF, "The winner is...!");
        alfont_set_font_size(pump, 80);
        alfont_textprintf_centre_aa(mb, pump, 512, 434, 0xFFFFFF, "Player no.2");
        blit(mb, screen, 0, 0, 0, 0, 1024, 768);
        // rest(3000);
        if (key[KEY_ESC])
          goto main_menu;
      }
    }
    else
    {
      if (player1.laps == winning_laps)
      {
        clear_to_color(screen, 0);
        alfont_set_font_size(pump, 70);
        alfont_textprintf_centre_aa(screen, pump, 512, 384, 0xFFFFFF, "your total time in %d lasp is %d:%d", winning_laps, global_min, global_sec);
        rest(2500);
        goto main_menu;
      }
    }

    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }
exit:
  printf("smrt blenderu");
}
END_OF_MAIN()

int play_button()
{
  if (mouse_x > 162 && mouse_y > 588 && mouse_x < 258 && mouse_y < 657)
  {
    alfont_textprintf_centre_aa(mb, pump, 211, 608, 0xFFFFFF, "Play");
    if (mouse_b & 1)
      return 1;
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 211, 608, 0, "Play");
  return 0;
}

int exit_button()
{
  if (mouse_x > 666 && mouse_y > 601 && mouse_x < 756 && mouse_y < 658)
  {
    alfont_textprintf_centre_aa(mb, pump, 707, 608, 0xFFFFFF, "Exit");
    if (mouse_b & 1)
      return 1;
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 707, 608, 0, "Exit");
  return 0;
}

int options_button()
{
  if (mouse_x > 312 && mouse_y > 597 && mouse_x < 404 && mouse_y < 654)
  {
    alfont_textprintf_centre_aa(mb, pump, 357, 608, 0xFFFFFF, "Options");
    if (mouse_b & 1)
      return 1;
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 357, 608, 0, "Options");
  return 0;
}

int credits_button()
{
  if (mouse_x > 546 && mouse_y > 597 && mouse_x < 635 && mouse_y < 656)
  {
    alfont_textprintf_centre_aa(mb, pump, 586, 608, 0xFFFFFF, "Credits");
    if (mouse_b & 1)
      return 1;
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 586, 608, 0, "Credits");
  return 0;
}

int career_button()
{
  if (mouse_x > 100 && mouse_y > 340 && mouse_x < 306 && mouse_y < 390)
  {
    alfont_textprintf_centre_aa(mb, pump, 206, 357, 0xFFFFFF, "Career");
    if (mouse_b & 1)
    {
      game_mode = MODE_CAREER;
      install_int(mooove_time, 1000);
      return 1;
    }
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 206, 357, 0, "Career");
  return 0;
}

int practice_button()
{
  if (mouse_x > 100 && mouse_y > 390 && mouse_x < 306 && mouse_y < 440)
  {
    alfont_textprintf_centre_aa(mb, pump, 206, 397, 0xFFFFFF, "Practice");
    if (mouse_b & 1)
    {
      game_mode = MODE_PRACTICE;
      install_int(mooove_time, 1000);
      return 1;
    }
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 206, 397, 0, "Practice");
  return 0;
}

int multiplayer_button()
{
  if (mouse_x > 100 && mouse_y > 440 && mouse_x < 306 && mouse_y < 490)
  {
    alfont_textprintf_centre_aa(mb, pump, 206, 437, 0xFFFFFF, "Multiplayer");
    if (mouse_b & 1)
    {
      game_mode = MODE_MULTIPLAYER;
      install_int(mooove_time, 1000);
      return 1;
    }
  }
  else
    alfont_textprintf_centre_aa(mb, pump, 206, 437, 0, "Multiplayer");
  return 0;
}

enum Scene main_menu_loop()
{
  play_sample(main_sample, 255, 128, 1000, 1);
  while (1)
  {
    show_mouse(NULL);
    blit(menu, mb, 0, 0, 0, 0, 1024, 768);
    alfont_set_font_size(pump, 50);

    if (play_button())
      return PLAY_MENU;

    if (exit_button())
      return EXIT;

    alfont_set_font_size(pump, 35);

    if (options_button())
      return OPTIONS_MENU;

    if (credits_button())
      return CREDITS_MENU;

    alfont_set_font_size(pump, 75);
    alfont_textprintf_centre_aa(mb, pump, 512, 100, 0, "Hawaii");
    show_mouse(mb);
    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }
}

enum Scene play_menu_loop()
{
  while (1)
  {
    show_mouse(NULL);
    blit(menu, mb, 0, 0, 0, 0, 1024, 768);
    alfont_set_font_size(pump, 50);

    alfont_textprintf_centre_aa(mb, pump, 211, 608, 0xFFFFFF, "Play");

    if (exit_button())
      return EXIT;

    alfont_set_font_size(pump, 35);

    if (options_button())
      return OPTIONS_MENU;

    if (credits_button())
      return CREDITS_MENU;

    alfont_set_font_size(pump, 60);

    if (career_button())
      return GAME;

    if (practice_button())
      return GAME;

    if (multiplayer_button())
      return GAME;

    alfont_set_font_size(pump, 75);
    alfont_textprintf_centre_aa(mb, pump, 512, 100, 0, "Hawaii");
    show_mouse(mb);
    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }
}

enum Scene credits_menu_loop()
{
  while (1)
  {
    show_mouse(NULL);
    blit(menu, mb, 0, 0, 0, 0, 1024, 768);
    alfont_set_font_size(pump, 50);

    if (play_button())
      return PLAY_MENU;

    if (exit_button())
      return EXIT;

    alfont_set_font_size(pump, 35);

    if (options_button())
      return OPTIONS_MENU;

    alfont_textprintf_centre_aa(mb, pump, 586, 608, 0xFFFFFF, "Credits");

    alfont_textprintf_centre_aa(mb, pump, 206, 417, 0, "Programming");
    alfont_textprintf_centre_aa(mb, pump, 764, 417, 0, "Graphics");
    alfont_set_font_size(pump, 70);
    alfont_textprintf_centre_aa(mb, pump, 206, 437, 0, "Daniel Lovasko");
    alfont_textprintf_centre_aa(mb, pump, 764, 437, 0, "Dominik Lukac");
    alfont_set_font_size(pump, 75);
    alfont_textprintf_centre_aa(mb, pump, 512, 100, 0, "Hawaii");
    show_mouse(mb);
    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }
}