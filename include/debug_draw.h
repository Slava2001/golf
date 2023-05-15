#ifndef INCLUDE_DEBUG_DRAWER_H
#define INCLUDE_DEBUG_DRAWER_H

struct point2f;

void draw_box(float x, float y, float h, float w);
void draw_obj(const struct point2f *center, const struct point2f *points, unsigned points_count);

#endif // INCLUDE_DEBUG_DRAWER_H
