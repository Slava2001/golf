// #define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"
#include "physic-debug-draw.h"
#include "physic-object.h"
#include "physic-util.h"
#include "vector.h"

#include <math.h>

int physic_set_debug_draw_api(struct physic_world *w, const struct physic_debug_drawer_api *a)
{
    check_true(a->box);
    check_true(a->circle);
    check_true(a->aabb);
    check_true(a->point);
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
    for (unsigned i = 0; i < w->object_count; i++) {
        const struct physic_object *o = &w->objects[i];

        if (w->draw_flag & PDDF_AABB) {
            struct vector2f sum = vector_add(&o->aabb.min, &o->aabb.max);
            struct vector2f offset = vector_mul_on_scalar(&sum, 0.5);
            struct vector2f pos = vector_add(&o->mass_center, &offset);
            w->draw_api->aabb(pos.x, pos.y, o->aabb.delta.y, o->aabb.delta.x, o->is_collided);
        }

        if (w->draw_flag & PDDF_OBJECTS) {
            for (unsigned q = 0; q < o->primitives_count; q++) {
                const struct physic_primitive *p = &o->primitives[q];
                struct vector2f offset = rotate_axis(&p->offset, o->angle);
                offset = vector_add(&offset, &o->mass_center);
                switch (p->type) {
                case PPT_RECTANGLE:
                    w->draw_api->box(offset.x, offset.y, p->h, p->w, p->angle + o->angle);
                    break;
                case PPT_CIRCLE:
                    w->draw_api->circle(offset.x, offset.y, p->r, p->angle + o->angle);
                    break;
                default:
                    err_if(true, "Unknown primitive type: %d", p->type);
                }
            }
        }

        if (w->draw_flag & PDDF_MASS_CENTER) {
            struct vector2f offset = rotate_axis(&o->zero_offset, o->angle);
            offset = vector_add(&offset, &o->mass_center);
            w->draw_api->point(offset.x, offset.y, o->angle);
            w->draw_api->point(o->mass_center.x, o->mass_center.y, o->angle);
            w->draw_api->point(o->mass_center.x, o->mass_center.y, o->angle + M_PI_4);
        }

    }
    return 0;
}
