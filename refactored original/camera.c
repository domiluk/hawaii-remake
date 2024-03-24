#include <allegro.h>

#include "camera.h"

extern BITMAP *ostrov;
extern CAMERA camera, camera1, camera2;

void init_cameras()
{
  camera.left = 0;
  camera.up = 0;
  camera.width = SCREEN_W;
  camera.height = SCREEN_H;
  camera.mb = create_bitmap(camera.width, camera.height);

  camera1.left = 0;
  camera1.up = 0;
  camera1.width = 512;
  camera1.height = 768;
  camera1.mb = create_bitmap(camera1.width, camera1.height);

  camera2.left = 0;
  camera2.up = 0;
  camera2.width = 512;
  camera2.height = 768;
  camera2.mb = create_bitmap(camera2.width, camera2.height);
}

void center_camera_on_a_boat(CAMERA *camera, const BOAT *player)
{
  camera->left = player->x - camera->width / 2;
  camera->up = player->y - camera->height / 2;

  if (camera->left < 0)
    camera->left = 0;
  if (camera->up < 0)
    camera->up = 0;
  if (camera->left > (ostrov->w - camera->width))
    camera->left = ostrov->w - camera->width;
  if (camera->up > (ostrov->h - camera->height))
    camera->up = ostrov->h - camera->height;
}
