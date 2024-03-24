#include <allegro.h>

#include "player.h"

extern BOAT player1, player2;

void init_players()
{
  player1.x = 996 - 100;
  player1.y = 1025 - 100;
  player1.laps = 0;
  player1.checkpoint_one = 0;
  player1.checkpoint_two = 0;
  player1.checkpoint_three = 0;
  player1.v = 0;
  player1.rot = -50;
  player1.last_lap_sec = 0;
  player1.last_lap_min = 0;
  player1.best_lap_sec = 99;
  player1.best_lap_min = 99;
  player1.maxspeed = 10;
  player1.speedup = 0.05;
  player1.slowdown = 0.08;
  player1.key_forwards = KEY_UP;
  player1.key_backwards = KEY_DOWN;
  player1.key_turn_left = KEY_LEFT;
  player1.key_turn_right = KEY_RIGHT;
  player1.bmp = load_bitmap("lodcervena.bmp", NULL);
  player1.bmp_rot = load_bitmap("lodcervena.bmp", NULL);

  player2.x = 1105 - 100;
  player2.y = 1087 - 100;
  player2.laps = 0;
  player2.checkpoint_one = 0;
  player2.checkpoint_two = 0;
  player2.checkpoint_three = 0;
  player2.v = 0;
  player2.rot = -75;
  player2.last_lap_sec = 0;
  player2.last_lap_min = 0;
  player2.best_lap_sec = 99;
  player2.best_lap_min = 99;
  player2.maxspeed = 10;
  player2.speedup = 0.05;
  player2.slowdown = 0.08;
  player2.key_forwards = KEY_W;
  player2.key_backwards = KEY_S;
  player2.key_turn_left = KEY_A;
  player2.key_turn_right = KEY_D;

  player2.bmp = load_bitmap("lodzelena.bmp", NULL);
  player2.bmp_rot = load_bitmap("lodzelena.bmp", NULL);
}
