#define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"

#include <string.h>

struct physic_aabb get_primitive_aabb(const struct physic_primitive *p, struct vector2f offset,
                                      float angle);
struct vector2f rotate_axis(struct vector2f p, float angle);
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
    w->objects[w->object_count].pos.x = d->pos_x;
    w->objects[w->object_count].pos.y = d->pos_y;
    w->objects[w->object_count].angle = d->angle;
    w->objects[w->object_count].speed.x = d->speed_x;
    w->objects[w->object_count].speed.y = d->speed_y;
    w->objects[w->object_count].speed_a = d->speed_a;
    w->object_count++;
    return w->object_count - 1;
}

struct physic_aabb get_primitive_aabb(const struct physic_primitive *p, struct vector2f offset,
                                      float angle)
{
    switch (p->type) {
    case PPT_RECTANGLE: {
        float a = p->angle + angle;
        struct vector2f p1 = rotate_axis((struct vector2f) { p->w/2,  p->h/2 }, a);
        struct vector2f p2 = rotate_axis((struct vector2f) { p->w/2, -p->h/2 }, a);
        float w = MAX(fabs(p1.x), fabs(p2.x));
        float h = MAX(fabs(p1.y), fabs(p2.y));
        return (struct physic_aabb) { .min.x = offset.x - w,
                                      .max.x = offset.x + w,
                                      .min.y = offset.y - h,
                                      .max.y = offset.y + h,
                                      .delta.x = 2 * w,
                                      .delta.y = 2 * h };
    }
    case PPT_CIRCLE:
        return (struct physic_aabb) { .min.x = offset.x - p->r,
                                      .max.x = offset.x + p->r,
                                      .min.y = offset.y - p->r,
                                      .max.y = offset.y + p->r,
                                      .delta.x = p->r * 2,
                                      .delta.y = p->r * 2 };
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
        struct vector2f offset = rotate_axis((struct vector2f){ p->offset.x, p->offset.y },
                                             o->angle);
        struct physic_aabb aabb = get_primitive_aabb(p, offset, o->angle);

        if (i) {
            o->aabb.min.x = MIN(o->aabb.min.x, aabb.min.x);
            o->aabb.max.x = MAX(o->aabb.max.x, aabb.max.x);
            o->aabb.min.y = MIN(o->aabb.min.y, aabb.min.y);
            o->aabb.max.y = MAX(o->aabb.max.y, aabb.max.y);
        } else {
            o->aabb = aabb;
        }
    }
    o->aabb.delta.x = o->aabb.max.x - o->aabb.min.x;
    o->aabb.delta.y = o->aabb.max.y - o->aabb.min.y;
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
    p->offset.x = d->offset_x;
    p->offset.y = d->offset_y;
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

int physic_object_apply_impulse(struct physic_world *w, int od, const struct vector2f *impulse,
                                                                const struct vector2f *point)
{
    (void)point;
    check_true(w);
    check_true((unsigned)od < w->object_count);
    // p = v*m => v = p/m => v = p*inv_m
    struct vector2f delta = vector_mul_on_scalar(impulse, w->objects[od].inv_mass);
    w->objects[od].speed = vector_add(&w->objects[od].speed, &delta);
    return 0;
}

int physic_update(struct physic_world *w, float elapsed_time_s)
{
    for (unsigned i = 0; i < w->object_count; i++) {
        struct vector2f offset = vector_mul_on_scalar(&w->objects[i].speed, elapsed_time_s);
        w->objects[i].pos = vector_add(&w->objects[i].pos, &offset);
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

struct vector2f rotate_axis(struct vector2f p, float angle)
{
    // https://en.wikipedia.org/wiki/Rotation_of_axes
    float c = cos(angle);
    float s = sin(angle);
    return (struct vector2f) {
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
            struct vector2f sum = vector_add(&o->aabb.min, &o->aabb.max);
            struct vector2f offset = vector_mul_on_scalar(&sum, 0.5);
            struct vector2f pos = vector_add(&o->pos, &offset);
            w->draw_api->aabb(pos.x, pos.y, o->aabb.delta.y, o->aabb.delta.x, o->is_collided);
        }

        if (w->draw_flag & PDDF_OBJECTS) {
            for (unsigned q = 0; q < o->primitives_count; q++) {
                const struct physic_primitive *p = &o->primitives[q];
                struct vector2f offset = rotate_axis(p->offset, o->angle);
                offset = vector_add(&offset, &o->pos);
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
