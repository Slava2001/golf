#ifndef INCLUDE_DEBUG_DRAWER_H
#define INCLUDE_DEBUG_DRAWER_H

#include "stdbool.h"

void draw_circle(float x, float y, float r, float a);
void draw_box(float x, float y, float h, float w, float a);
void draw_aabb(float x, float y, float h, float w, bool is_collided);

#endif // INCLUDE_DEBUG_DRAWER_H
