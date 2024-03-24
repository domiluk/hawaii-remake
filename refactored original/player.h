#ifndef __PLAYER_H_
#define __PLAYER_H_

typedef struct boat
{
  float x;
  float y;
  float v;
  float rot;
  float maxspeed; // TODO: should not be in boat
  float speedup;  // TODO: should not be in boat
  float slowdown; // TODO: should not be in boat
  int laps;
  int checkpoint_one;
  int checkpoint_two;
  int checkpoint_three;
  int last_lap_sec;
  int best_lap_sec;
  int last_lap_min;
  int best_lap_min;
  int AI;
  int key_forwards;
  int key_backwards;
  int key_turn_left;
  int key_turn_right;
  BITMAP *bmp;
  BITMAP *bmp_rot;
} BOAT;

void init_players();

#endif
