#define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"

#include <string.h>

int physic_init_world(struct physic_world *w)
{
    log_enter();
    memset(w, 0, sizeof(struct physic_world));
    return 0;
}

int physic_add_object(struct physic_world *w, struct physic_object_description *d)
{
    log_enter();
    check_true(d);
    check_true(w->object_count < PHYSIC_OBJECT_MAX_COUNT);
    memset(&w->objects[w->object_count], 0, sizeof(struct physic_object));
    w->objects[w->object_count].inv_mass = d->mass == PHYSIC_INFINITY_MASS? 0: 1/d->mass;
    w->objects[w->object_count].point_arr = d->point_arr;
    w->objects[w->object_count].points_count = d->points_count;
    w->objects[w->object_count].pos_x = d->pos_x;
    w->objects[w->object_count].pos_y = d->pos_y;
    w->object_count++;
    return w->object_count - 1;
}

int physic_object_apply_impulse(struct physic_world *w, int od, const struct point2f *impulse)
{
    check_true(w);
    check_true((unsigned)od < w->object_count);
    // p = v*m => v = p/m => v = p*inv_m
    w->objects[od].speed_x += impulse->x * w->objects[od].inv_mass;
    w->objects[od].speed_y += impulse->y * w->objects[od].inv_mass;
    return 0;
}

int physic_update(struct physic_world *w, float elapsed_time_s)
{
    for (unsigned i = 0; i < w->object_count; i++) {
         w->objects[i].pos_x += w->objects[i].speed_x * elapsed_time_s;
         w->objects[i].pos_y += w->objects[i].speed_y * elapsed_time_s;
    }
    return 0;
}

int physic_set_debug_draw_api(struct physic_world *w, const struct physic_debug_drawer_api *a)
{
    check_true(a->box);
    check_true(a->obj);
    w->draw_api = a;
    return 0;
}

int physic_set_debug_draw_flag(struct physic_world *w, int flag)
{
    w->draw_flag = flag;
    return 0;
}

int physic_debug_draw(const struct physic_world *w)
{
    check_true(w->draw_api);
    if (w->draw_flag & PDDF_OBJECTS) {
        for (unsigned i = 0; i < w->object_count; i++) {
            w->draw_api->obj((&(struct point2f){ w->objects[i].pos_x, w->objects[i].pos_y }),
                             w->objects[i].point_arr, w->objects[i].points_count);
        }
    }
    return 0;
}
