#ifndef INCLUDE_PHYSIC_H
#define INCLUDE_PHYSIC_H

#include "physic-object.h"
#include "physic-debug-draw.h"

#define PHYSIC_OBJECT_MAX_COUNT 100

struct physic_world {
    int draw_flag;
    const struct physic_debug_drawer_api *draw_api;

    struct physic_object objects[PHYSIC_OBJECT_MAX_COUNT];
    unsigned object_count;
};

/// @brief init physic world
/// @param w world
/// @return 0 on ok, -1 on fail
int physic_init_world(struct physic_world *w);
/// @brief update phisic
/// @param w world
/// @param elapsed_time_s elapsed time in seconds
/// @return 0 on ok, -1 on fail
int physic_update(struct physic_world *w, float elapsed_time_s);

#endif // INCLUDE_PHYSIC_H
