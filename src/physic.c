// #define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"
#include "physic-object.h"

#include <string.h>

int physic_init_world(struct physic_world *w)
{
    log_enter();
    memset(w, 0, sizeof(struct physic_world));
    return 0;
}

int physic_update(struct physic_world *w, float elapsed_time_s)
{
    for (unsigned i = 0; i < w->object_count; i++) {
        struct vector2f offset = vector_mul_on_scalar(&w->objects[i].speed, elapsed_time_s);
        w->objects[i].mass_center = vector_add(&w->objects[i].mass_center, &offset);
        w->objects[i].angle += w->objects[i].speed_a * elapsed_time_s;
        try_do(update_collaider(w, i));
    }
    return 0;
}
