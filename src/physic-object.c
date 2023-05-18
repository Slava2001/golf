#define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"
#include "physic-object.h"
#include "physic-util.h"

#include <string.h>

int update_obj_mass(struct physic_world *w, int od)
{
    struct physic_object *o = &w->objects[od];
    float mass = 0;

    struct vector2f mass_center = { 0 };
    for (unsigned i = 0; i < o->primitives_count; i++) {
        struct physic_primitive *p = &o->primitives[i];
        mass += p->mass;
        struct vector2f moment = vector_mul_on_scalar(&p->offset, p->mass);
        mass_center = vector_add(&mass_center, &moment);
    }
    o->inv_mass = mass == 0? 0: 1/mass;
    mass_center = vector_mul_on_scalar(&mass_center, o->inv_mass);
    o->mass_center = vector_add(&o->mass_center, &mass_center);
    o->zero_offset = vector_sub(&o->zero_offset, &mass_center);

    float moment_of_inertia = 0;
    for (unsigned i = 0; i < o->primitives_count; i++) {
        struct physic_primitive *p = &o->primitives[i];
        p->offset = vector_sub(&p->offset, &mass_center);
        float dist = vector_len(&p->offset);
        moment_of_inertia += p->moment_of_inertia + p->mass * dist * dist;
    }
    o->inv_moment_of_inertia = moment_of_inertia == 0? 0: 1/moment_of_inertia;
    LOG_VECTOR(logd, o->zero_offset);
    return 0;
}

int physic_add_object(struct physic_world *w, struct physic_object_description *d)
{
    log_enter();
    check_true(d);
    check_true(w->object_count < PHYSIC_OBJECT_MAX_COUNT);
    memset(&w->objects[w->object_count], 0, sizeof(struct physic_object));
    w->objects[w->object_count].mass_center.x = d->pos_x;
    w->objects[w->object_count].mass_center.y = d->pos_y;
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
        struct vector2f p1 = rotate_axis(&(struct vector2f) { p->w/2,  p->h/2 }, a);
        struct vector2f p2 = rotate_axis(&(struct vector2f) { p->w/2, -p->h/2 }, a);
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
        struct vector2f offset = rotate_axis(&(struct vector2f){ p->offset.x, p->offset.y },
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
    p->mass = d->mass;
    p->offset.x = d->offset_x + w->objects[od].zero_offset.x;
    p->offset.y = d->offset_y + w->objects[od].zero_offset.y;
    p->angle = d->angle;
    switch (d->type) {
    case PPT_RECTANGLE:
        p->h = d->h;
        p->w = d->w;
        p->moment_of_inertia = p->mass * (p->h * p->h + p->w * p->w) / 12;
        break;
    case PPT_CIRCLE:
        p->r = d->r;
        p->moment_of_inertia = p->mass * p->r * p->r / 2;
        break;
    default:
        err_if(true, "Unknown primitive type: %d", d->type);
    }
    w->objects[od].primitives_count++;
    try_do(update_collaider(w, od));
    try_do(update_obj_mass(w, od));
    return od * PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT + w->objects[od].primitives_count - 1;
}

int physic_object_apply_impulse(struct physic_world *w, int od, const struct vector2f *impulse,
                                                                const struct vector2f *point)
{
    check_true(w);
    check_true((unsigned)od < w->object_count);

    struct physic_object *o = &w->objects[od];
    struct vector2f radius_vec = vector_add(point, &o->zero_offset); // from center mass to point
    LOG_VECTOR(logd, radius_vec);

    float normal_impulse = vector_projection(impulse, &radius_vec);
    struct vector2f perpendicular_radius_vec = rotate_axis(&radius_vec, M_PI_2);
    float tangential = vector_projection(impulse,  &perpendicular_radius_vec);

    o->speed_a += tangential * o->inv_moment_of_inertia;

    struct vector2f norm_radius_vec = vector_normalize(&radius_vec);
    struct vector2f delta = vector_mul_on_scalar(&norm_radius_vec, normal_impulse * o->inv_mass);
    o->speed = vector_add(&o->speed, &delta);
    return 0;
}
