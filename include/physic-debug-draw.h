#ifndef INCLUDE_PHYSIC_DEBUG_DRAW_H
#define INCLUDE_PHYSIC_DEBUG_DRAW_H

#include "stdbool.h"

struct physic_world;

enum physic_debug_draw_flag {
    PDDF_OBJECTS     = 1 << 0,
    PDDF_AABB        = 1 << 1,
    PDDF_MASS_CENTER = 1 << 2
};

struct physic_debug_drawer_api {
    void (*circle)(float x, float y, float r, float a);
    void (*box)(float x, float y, float h, float w, float a);
    void (*aabb)(float x, float y, float h, float w, bool is_collided);
    void (*point)(float x, float y, float a);
};

/// @brief set debug draw api
/// @param w world
/// @param a draw api
/// @return 0 on ok, -1 on fail
int physic_set_debug_draw_api(struct physic_world *w, const struct physic_debug_drawer_api *a);
/// @brief set debug draw flag
/// @param w world
/// @param flag flag
/// @return 0 on ok, -1 on fail
int physic_set_debug_draw_flag(struct physic_world *w, int flag);
/// @brief debug draw
/// @param w world
/// @return 0 on ok, -1 on fail
int physic_debug_draw(const struct physic_world *w);

#endif // INCLUDE_PHYSIC_DEBUG_DRAW_H
