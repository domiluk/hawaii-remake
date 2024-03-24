#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "player.h"

typedef struct
{
  int left;
  int up;
  int width;
  int height;
  BITMAP *mb;
} CAMERA;

void init_cameras();
void center_camera_on_a_boat(CAMERA *camera, const BOAT *player);

#endif
