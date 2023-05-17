#define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"

#include <string.h>

struct physic_aabb get_primitive_aabb(const struct physic_primitive *p, struct point2f offset,
                                      float angle);
struct point2f rotate_axis(struct point2f p, float angle);
int update_collaider(struct physic_world *w, int od);

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
    w->objects[w->object_count].pos_x = d->pos_x;
    w->objects[w->object_count].pos_y = d->pos_y;
    w->objects[w->object_count].angle = d->angle;
    w->objects[w->object_count].speed_x = d->speed_x;
    w->objects[w->object_count].speed_y = d->speed_y;
    w->objects[w->object_count].speed_a = d->speed_a;
    w->object_count++;
    return w->object_count - 1;
}

struct physic_aabb get_primitive_aabb(const struct physic_primitive *p, struct point2f offset,
                                      float angle)
{
    switch (p->type) {
    case PPT_RECTANGLE: {
        float a = p->angle + angle;
        struct point2f p1 = rotate_axis((struct point2f) { p->w/2,  p->h/2 }, a);
        struct point2f p2 = rotate_axis((struct point2f) { p->w/2, -p->h/2 }, a);
        float w = MAX(fabs(p1.x), fabs(p2.x));
        float h = MAX(fabs(p1.y), fabs(p2.y));
        return (struct physic_aabb) { .min_x = offset.x - w,
                                      .max_x = offset.x + w,
                                      .min_y = offset.y - h,
                                      .max_y = offset.y + h,
                                      .delta_x = 2 * w,
                                      .delta_y = 2 * h };
    }
    case PPT_CIRCLE:
        return (struct physic_aabb) { .min_x = offset.x - p->r,
                                      .max_x = offset.x + p->r,
                                      .min_y = offset.y - p->r,
                                      .max_y = offset.y + p->r,
                                      .delta_x = p->r * 2,
                                      .delta_y = p->r * 2 };
    }
    return (struct physic_aabb) { 0 };
}

int update_collaider(struct physic_world *w, int od)
{
    struct physic_object *o = &w->objects[od];
    if (!o->primitives_count) {
        o->aabb = (struct physic_aabb) { 0 };
        return 0;
    }

    for (unsigned i = 0; i < o->primitives_count; i++) {
        const struct physic_primitive *p = &o->primitives[i];
        struct point2f offset = rotate_axis((struct point2f){ p->offset_x, p->offset_y }, o->angle);
        struct physic_aabb aabb = get_primitive_aabb(p, offset, o->angle);

        if (i) {
            o->aabb.min_x = MIN(o->aabb.min_x, aabb.min_x);
            o->aabb.max_x = MAX(o->aabb.max_x, aabb.max_x);
            o->aabb.min_y = MIN(o->aabb.min_y, aabb.min_y);
            o->aabb.max_y = MAX(o->aabb.max_y, aabb.max_y);
        } else {
            o->aabb = aabb;
        }
    }
    o->aabb.delta_x = o->aabb.max_x - o->aabb.min_x;
    o->aabb.delta_y = o->aabb.max_y - o->aabb.min_y;
    return 0;
}

int physic_add_primitive(struct physic_world *w, int od, struct physic_primitive_description *d)
{
    log_enter();
    check_true(d);
    check_true(w->object_count > (unsigned)od);
    check_true(w->objects[od].primitives_count < PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT);
    struct physic_primitive *p = &w->objects[od].primitives[w->objects[od].primitives_count];
    p->type = d->type;
    p->offset_x = d->offset_x;
    p->offset_y = d->offset_y;
    p->angle = d->angle;
    switch (d->type) {
    case PPT_RECTANGLE:
        p->h = d->h;
        p->w = d->w;
        break;
    case PPT_CIRCLE:
        p->r = d->r;
        break;
    default:
        err_if(true, "Unknown primitive type: %d", d->type);
    }
    try_do(update_collaider(w, od));
    w->objects[od].primitives_count++;
    return od * PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT + w->objects[od].primitives_count - 1;
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
        w->objects[i].angle += w->objects[i].speed_a * elapsed_time_s;
        try_do(update_collaider(w, i));
    }
    return 0;
}

int physic_set_debug_draw_api(struct physic_world *w, const struct physic_debug_drawer_api *a)
{
    check_true(a->box);
    check_true(a->circle);
    check_true(a->aabb);
    w->draw_api = a;
    return 0;
}

int physic_set_debug_draw_flag(struct physic_world *w, int flag)
{
    w->draw_flag = flag;
    return 0;
}

struct point2f rotate_axis(struct point2f p, float angle)
{
    // https://en.wikipedia.org/wiki/Rotation_of_axes
    float c = cos(angle);
    float s = sin(angle);
    return (struct point2f) {
        .x = p.x * c - p.y * s,
        .y = p.x * s + p.y * c
    };
}

int physic_debug_draw(const struct physic_world *w)
{
    check_true(w->draw_api);
    for (unsigned i = 0; i < w->object_count; i++) {
        const struct physic_object *o = &w->objects[i];

        if (w->draw_flag & PDDF_AABB) {
            float x = o->pos_x + ( o->aabb.min_x + o->aabb.max_x) / 2;
            float y = o->pos_y + ( o->aabb.min_y + o->aabb.max_y) / 2;
            w->draw_api->aabb(x, y, o->aabb.delta_y, o->aabb.delta_x, o->is_collided);
        }

        if (w->draw_flag & PDDF_OBJECTS) {
            for (unsigned q = 0; q < o->primitives_count; q++) {
                const struct physic_primitive *p = &o->primitives[q];
                struct point2f offset = rotate_axis((struct point2f) {p->offset_x, p->offset_y},
                                                    o->angle);
                offset.x += o->pos_x;
                offset.y += o->pos_y;

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
    }
    return 0;
}
