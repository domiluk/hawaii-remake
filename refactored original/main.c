#include <allegro.h>
#include <alfont.h>
#include <math.h>
#include <stdio.h>

#include "camera.h"
#include "player.h"

#define MODE_MULTIPLAYER 0
#define MODE_CAREER 1
#define MODE_PRACTICE 2

#define DEG_TO_RADS 0.01745329252 // just multiply your degrees by this constant
#define RADS_TO_DEG 57.2957795147 // just multiply your radians by this constant

#define ROTATE_AMOUNT 1.5

void rotate(BITMAP *bmp, BITMAP *tmp, float angle);

CAMERA camera, camera1, camera2;
BITMAP *ostrov, *vsetko;
BITMAP *alpha;
BITMAP *white_point_bmp, *rotated_white_point_bmp;
BITMAP *panel;
SAMPLE *lap_gong, *spring, *main_sample;
int game_mode;
int global_sec;
int global_min;

// TODO: these are set in options, never used anyway and never stored to a file or read from a file
int res = 0, depth = 2, vol = 2, nlaps = 3, player1_boat_color = 0, player2_boat_color = 1;
int winning_laps = 3;

enum Scene
{
  MAIN_MENU,
  PLAY_MENU,
  OPTIONS_MENU,
  CREDITS_MENU,
  GAME,
  GAME_OVER,
  EXIT
};

enum Scene main_menu_loop();
enum Scene play_menu_loop();
enum Scene options_menu_loop();
enum Scene credits_menu_loop();
enum Scene game_loop();
enum Scene game_over_loop();

BOAT player1, player2;
BITMAP *mb, *menu;

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

  init_cameras();
  init_players();

  white_point_bmp = create_bitmap(100, 100);
  clear_bitmap(white_point_bmp);
  putpixel(white_point_bmp, 97, 50, makecol(254, 255, 255));
  putpixel(white_point_bmp, 96, 50, makecol(254, 255, 255));

  rotated_white_point_bmp = create_bitmap(100, 100);
  clear_bitmap(rotated_white_point_bmp);

  ostrov = load_bitmap("ostrov1.bmp", NULL);
  alpha = load_bitmap("alpha1.bmp", NULL);
  menu = load_bitmap("menu.bmp", NULL);
  panel = load_bitmap("panel.bmp", NULL);
  lap_gong = load_sample("sounds/dray.wav");
  spring = load_sample("sounds/spring.wav");
  main_sample = load_sample("main.wav");

  alfont_set_font_size(pump, 50);

  play_sample(main_sample, 255, 128, 1000, 1);

  enum Scene next_scene = MAIN_MENU;
  while (1)
  {
    switch (next_scene)
    {
    case MAIN_MENU:
      next_scene = main_menu_loop();
      break;
    case PLAY_MENU:
      next_scene = play_menu_loop();
      break;
    case OPTIONS_MENU:
      next_scene = options_menu_loop();
      break;
    case CREDITS_MENU:
      next_scene = credits_menu_loop();
      break;
    case GAME:
      next_scene = game_loop();
      break;
    case GAME_OVER:
      next_scene = game_over_loop();
      break;
    case EXIT:
      printf("smrt blenderu");
      exit(0);
    }
  }
}
END_OF_MAIN()

int main_menu_button(const char *label, int x_min, int y_min, int x_max, int y_max, int label_x, int label_y)
{
  rect(mb, x_min, y_min, x_max, y_max, makecol(0, 0, 0));
  if (mouse_x > x_min && mouse_y > y_min && mouse_x < x_max && mouse_y < y_max)
  {
    alfont_textprintf_centre_aa(mb, pump, label_x, label_y, 0xFFFFFF, label);
    if (mouse_b & 1)
      return 1;
  }
  else
    alfont_textprintf_centre_aa(mb, pump, label_x, label_y, 0, label);
  return 0;
}

int play_button()
{
  return main_menu_button("Play", 162, 588, 258, 657, 211, 608);
}

int exit_button()
{
  return main_menu_button("Exit", 666, 601, 756, 658, 707, 608);
}

int options_button()
{
  return main_menu_button("Options", 312, 597, 404, 654, 357, 608);
}

int credits_button()
{
  return main_menu_button("Credits", 546, 597, 635, 656, 586, 608);
}

int career_button()
{
  int pressed = main_menu_button("Career", 100, 340, 306, 390, 206, 357);
  if (pressed)
    game_mode = MODE_CAREER;
  return pressed;
}

int practice_button()
{
  int pressed = main_menu_button("Practice", 100, 390, 306, 440, 206, 397);
  if (pressed)
    game_mode = MODE_PRACTICE;
  return pressed;
}

int multiplayer_button()
{
  int pressed = main_menu_button("Multiplayer", 100, 440, 306, 490, 206, 437);
  if (pressed)
    game_mode = MODE_MULTIPLAYER;
  return pressed;
}

int checkbox(const char *label, int x, int y, int checked)
{
  alfont_textout_aa(mb, pump, label, x + 30, y, 0);
  rect(mb, x, y, x + 20, y + 20, 0);
  if (checked)
    alfont_textout_aa(mb, pump, "X", x, y, 0);
  return mouse_x > x && mouse_x < x + 20 && mouse_y > y && mouse_y < y + 20 && mouse_b & 1;
}

enum Scene main_menu_loop()
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
  return MAIN_MENU;
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

enum Scene options_menu_loop()
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

    alfont_textprintf_centre_aa(mb, pump, 357, 608, 0xFFFFFF, "Options");

    if (credits_button())
      return CREDITS_MENU;

    alfont_set_font_size(pump, 32);
    alfont_textout_centre_aa(mb, pump, "IN GAME OPTIONS", 764, 20, 0xFFFFFF);
    alfont_textout_centre_aa(mb, pump, "SETTINGS", 764, 290, 0xFFFFFF);

    alfont_set_font_size(pump, 30);
    alfont_textout_centre_aa(mb, pump, "Number of laps", 700, 60, 0);
    if (checkbox("3", 800, 60, (nlaps == 3)))
      nlaps = 3;
    if (checkbox("5", 855, 60, (nlaps == 5)))
      nlaps = 5;
    if (checkbox("7", 910, 60, (nlaps == 7)))
      nlaps = 7;

    alfont_textout_centre_aa(mb, pump, "Color of Player's boat", 700, 90, 0);
    if (checkbox("Red", 800, 90, (player1_boat_color == 0)))
      player1_boat_color = 0;
    if (checkbox("Green", 800, 120, (player1_boat_color == 1)))
      player1_boat_color = 1;
    if (checkbox("Blue", 800, 150, (player1_boat_color == 2)))
      player1_boat_color = 2;

    alfont_textout_centre_aa(mb, pump, "Color of CPU's boat", 700, 190, 0);
    if (checkbox("Red", 800, 190, (player2_boat_color == 0)))
      player2_boat_color = 0;
    if (checkbox("Green", 800, 220, (player2_boat_color == 1)))
      player2_boat_color = 1;
    if (checkbox("Blue", 800, 250, (player2_boat_color == 2)))
      player2_boat_color = 2;

    alfont_textout_centre_aa(mb, pump, "Resolution", 700, 330, 0);
    if (checkbox("800x600", 800, 330, (res == 0)))
      res = 0;
    if (checkbox("1024x768", 800, 360, (res == 1)))
      res = 1;
    if (checkbox("1280x1024", 800, 390, (res == 2)))
      res = 2;

    alfont_textout_centre_aa(mb, pump, "Color Depth", 700, 420, 0);
    if (checkbox("16", 800, 420, (depth == 0)))
      depth = 0;
    if (checkbox("24", 800, 450, (depth == 1)))
      depth = 1;
    if (checkbox("32", 800, 480, (depth == 2)))
      depth = 2;

    alfont_textout_centre_aa(mb, pump, "Volume", 700, 510, 0);
    if (checkbox("Mute", 800, 510, (vol == 0)))
      vol = 0;
    if (checkbox("Normal", 800, 540, (vol == 1)))
      vol = 1;
    if (checkbox("Loud", 800, 570, (vol == 2)))
      vol = 2;

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

int get_boat_front_point_color_in_alpha(const BOAT *player)
{
  rotate(white_point_bmp, rotated_white_point_bmp, player->rot);
  int boat_front_x, boat_front_y;
  for (int rx = 0; rx < rotated_white_point_bmp->w; rx++)
    for (int ry = 0; ry < rotated_white_point_bmp->h; ry++)
      if (getr(getpixel(rotated_white_point_bmp, rx, ry)) == 254)
      {
        boat_front_x = rx;
        boat_front_y = ry;
      }

  return getr(getpixel(alpha, player->x + boat_front_x, player->y + boat_front_y));
}

void check_boat_collisions(BOAT *player)
{
  int boat_front_point_color_in_alpha = get_boat_front_point_color_in_alpha(player);
  int player_hit_the_land = boat_front_point_color_in_alpha == 0;
  if (player_hit_the_land)
  {
    player->v *= -0.75;
  }

  int player_hit_checkpoint_one = boat_front_point_color_in_alpha == 64;
  if (player_hit_checkpoint_one)
    player->checkpoint_one = 1;

  int player_hit_checkpoint_two = boat_front_point_color_in_alpha == 128;
  if (player_hit_checkpoint_two)
    player->checkpoint_two = 1;

  int player_hit_checkpoint_three = boat_front_point_color_in_alpha == 32;
  if (player_hit_checkpoint_three)
    player->checkpoint_three = 1;

  int player_hit_finish_line = boat_front_point_color_in_alpha == 192;
  if (player_hit_finish_line && player->checkpoint_one && player->checkpoint_two && player->checkpoint_three)
  {
    player->checkpoint_one = 0;
    player->checkpoint_two = 0;
    player->checkpoint_three = 0;
    player->last_lap_sec = global_sec - player->last_lap_sec;
    player->last_lap_min = global_min - player->last_lap_min;
    if (player->last_lap_sec < 0)
    {
      player->last_lap_min--;
      player->last_lap_sec = 60 - abs(player->last_lap_sec);
    }
    if (player->last_lap_sec + (player->last_lap_min) * 60 < player->best_lap_sec + (player->best_lap_min) * 60)
    {
      player->best_lap_sec = player->last_lap_sec;
      player->best_lap_min = player->last_lap_min;
    }
    player->laps++;
    play_sample(lap_gong, 255, 128, 1000, 0);
  }
}

void check_boat_to_boat_collision()
{
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

      temp = player1.v;
      player1.v = player2.v;
      player2.v = temp;
    }
    if (game_mode == MODE_CAREER)
    {
      player1.v = cos(getAI_rot(AI_pos) * DEG_TO_RADS) * 5; // TODO: probably wrong equation used (cos is just for x-vel)
      // player1.yv = sin(getAI_rot(AI_pos) * DEG_TO_RADS) * 5;
      player1.rot = getAI_rot(AI_pos);

      // player2.xv = cos(player1.rot * DEG_TO_RADS);
      // player2.yv = sin(player1.rot * DEG_TO_RADS);
    }
    // xpos[AI_pos] += cos(boat.rot* DEG_TO_RADS)*5;
    // ypos[AI_pos] += sin(boat.rot* DEG_TO_RADS)*5;
    // calc_AI();
  }
}

void slow_down(BOAT *player)
{
  // TODO: ???????? probably: add something like else: player.v = 0
  if (player->v > player->slowdown)
    player->v -= player->slowdown;
  if (player->v < -player->slowdown)
    player->v += player->slowdown;
}

void movement(BOAT *player)
{
  player->x += cos(player->rot * DEG_TO_RADS) * player->v;
  player->y += sin(player->rot * DEG_TO_RADS) * player->v;

  if (key[player->key_forwards])
  {
    if (player->v < player->maxspeed)
      player->v += player->speedup;
  }
  else
  {
    if (key[player->key_backwards])
      slow_down(player);
    slow_down(player);
  }
  if (key[player->key_turn_left])
    player->rot -= ROTATE_AMOUNT;
  if (key[player->key_turn_right])
    player->rot += ROTATE_AMOUNT;
}

void game_init()
{
  install_int(mooove_time, 1000);
}

void game_deinit()
{
  remove_int(mooove_time);
}

enum Scene game_loop()
{
  game_init();
  while (!key[KEY_ESC])
  {
    int somebody_won = player1.laps == winning_laps || player2.laps == winning_laps;
    if (somebody_won)
    {
      game_deinit();
      return GAME_OVER;
    }

    check_boat_collisions(&player1);
    movement(&player1);

    if (game_mode == MODE_MULTIPLAYER)
    {
      check_boat_collisions(&player2);
      movement(&player2);
    }

    if (game_mode == MODE_CAREER)
    {
      player2.x = getAI_x(AI_pos);
      player2.y = getAI_y(AI_pos);
      player2.rot = getAI_rot(AI_pos);
      if (player2.v > 0.2)
      {
        player2.x += player2.v;
        player2.y += player2.v;
        if (player2.v > player2.slowdown)
          player2.v -= player2.slowdown;
        if (player2.v < -player2.slowdown)
          player2.v += player2.slowdown;
      }

      AI_pos += 3;
      if (AI_pos == npts - 1)
      {
        AI_pos = 0;
        // REFACTOR: DELETE LINE: curspl++;
        player2.laps++;
      }
    }

    if (game_mode != MODE_PRACTICE)
      check_boat_to_boat_collision();

    if (game_mode == MODE_MULTIPLAYER)
    {
      center_camera_on_a_boat(&camera1, &player1);
      center_camera_on_a_boat(&camera2, &player2);

      blit(ostrov, camera1.mb, camera1.left, camera1.up, 0, 0, camera1.width, camera1.height);
      blit(ostrov, camera2.mb, camera2.left, camera2.up, 0, 0, camera2.width, camera2.height);

      rotate(player1.bmp, player1.bmp_rot, player1.rot + 90);
      draw_sprite(camera1.mb, player1.bmp_rot, player1.x - camera1.left, player1.y - camera1.up);
      draw_sprite(camera2.mb, player1.bmp_rot, player1.x - camera2.left, player1.y - camera2.up);

      rotate(player2.bmp, player2.bmp_rot, player2.rot + 90);
      draw_sprite(camera1.mb, player2.bmp_rot, player2.x - camera1.left, player2.y - camera1.up);
      draw_sprite(camera2.mb, player2.bmp_rot, player2.x - camera2.left, player2.y - camera2.up);

      blit(camera1.mb, mb, 0, 0, 0, 0, camera1.width, camera1.height);
      blit(camera2.mb, mb, 0, 0, 512, 0, camera2.width, camera2.height);
      vline(mb, 512, 0, 768, 0xFFFFFF);
    }
    else
    {
      center_camera_on_a_boat(&camera, &player1);

      blit(ostrov, mb, camera.left, camera.up, 0, 0, camera.left + 1024, camera.up + 768);

      rotate(player1.bmp, player1.bmp_rot, player1.rot + 90);
      draw_sprite(mb, player1.bmp_rot, player1.x - camera.left, player1.y - camera.up);
    }

    if (game_mode == MODE_CAREER)
    {
      rotate(player2.bmp, player2.bmp_rot, player2.rot); // TODO: preco tu nie je rot + 90??
      draw_sprite(mb, player2.bmp_rot, player2.x - camera.left, player2.y - camera.up);
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

    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }
  return EXIT;
}

enum Scene game_over_loop()
{
  while (!key[KEY_ESC])
  {
    if (game_mode != MODE_PRACTICE)
    {
      if (player1.laps == winning_laps || player2.laps == winning_laps)
      {
        clear_to_color(mb, 0);
        alfont_set_font_size(pump, 75);
        alfont_textprintf_centre_aa(mb, pump, 512, 384, 0xFFFFFF, "The winner is...!");
        alfont_set_font_size(pump, 80);
        alfont_textprintf_centre_aa(mb, pump, 512, 434, 0xFFFFFF, "Player no.%d", player1.laps == winning_laps ? 1 : 2);
      }
    }
    else
    {
      if (player1.laps == winning_laps)
      {
        clear_to_color(mb, 0);
        alfont_set_font_size(pump, 70);
        alfont_textprintf_centre_aa(mb, pump, 512, 384, 0xFFFFFF, "Your total time in %d laps is %d:%d", winning_laps, global_min, global_sec);
      }
    }

    blit(mb, screen, 0, 0, 0, 0, 1024, 768);
  }
  return MAIN_MENU;
}
