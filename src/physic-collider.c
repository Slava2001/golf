// #define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic.h"
#include "physic-util.h"

struct collision_point {
    struct vector2f point;
    struct vector2f normal;
};

bool aabb_is_collided(const struct physic_aabb *a, const struct physic_aabb *b);
bool is_collided(struct physic_world *w, const struct physic_object *a, const struct physic_object *b);

bool is_collided_circle_circle(const struct vector2f *obj_a_pos, const struct physic_primitive *a,
                               const struct vector2f *obj_b_pos, const struct physic_primitive *b,
                               struct collision_point *point);

int physic_resolve_collisions(struct physic_world *w)
{
    if (w->object_count < 2) {
        return 0;
    }
    for (unsigned i = 0; i < w->object_count; i++) {
        bool res = false;
        for (unsigned q = 0; q < w->object_count; q++) {
            res |= i!= q && is_collided(w, &w->objects[i], &w->objects[q]);
        }
        w->objects[i].is_collided = res;
    }
    return 0;
}

bool is_collided(struct physic_world *w, const struct physic_object *a, const struct physic_object *b)
{
    if (!aabb_is_collided(&a->aabb, &b->aabb)) {
        return false;
    }

    for (unsigned i = 0; i < a->primitives_count; i++)
    {
        for (unsigned q = 0; q < b->primitives_count; q++)
        {
            const struct physic_primitive *ap = &a->primitives[i];
            struct vector2f ap_offset = rotate_axis(&(struct vector2f){ ap->offset.x, ap->offset.y }, a->angle);
            ap_offset = vector_add(&ap_offset, &a->mass_center);
            struct physic_aabb a_aabb = get_primitive_aabb(ap, ap_offset, a->angle);

            const struct physic_primitive *bp = &b->primitives[q];
            struct vector2f bp_offset = rotate_axis(&(struct vector2f){ bp->offset.x, bp->offset.y }, b->angle);
            bp_offset = vector_add(&bp_offset, &b->mass_center);
            struct physic_aabb b_aabb = get_primitive_aabb(bp, bp_offset, b->angle);

            bool is_collided = aabb_is_collided(&a_aabb, &b_aabb);
            w->draw_api->aabb(ap_offset.x, ap_offset.y, a_aabb.delta.y, a_aabb.delta.x, is_collided);
            w->draw_api->aabb(bp_offset.x, bp_offset.y, b_aabb.delta.y, b_aabb.delta.x, is_collided);

            if (is_collided) {
                struct collision_point point;
                bool rc = is_collided_circle_circle(&a->mass_center, ap, &b->mass_center, bp, &point);
                struct vector2f tmp = rotate_axis(&point.point, a->angle);
                struct vector2f real_point = vector_add(&tmp, &a->mass_center);
                w->draw_api->point(real_point.x, real_point.y, a->angle);
                return rc;
            }
        }
    }
    return false;
}

bool aabb_is_collided(const struct physic_aabb *a, const struct physic_aabb *b)
{
    if(a->max.x < b->min.x || a->min.x > b->max.x) return false;
    if(a->max.y < b->min.y || a->min.y > b->max.y) return false;
    return true;
}

bool is_collided_circle_circle(const struct vector2f *obj_a_pos, const struct physic_primitive *a,
                               const struct vector2f *obj_b_pos, const struct physic_primitive *b,
                               struct collision_point *point)
{
    ext_if(a->type != PPT_CIRCLE || b->type != PPT_CIRCLE, "wrong primitive type");

    struct vector2f a_pos = vector_add(obj_a_pos, &a->offset);
    struct vector2f b_pos = vector_add(obj_b_pos, &b->offset);

    struct vector2f radius_vec = vector_sub(&a_pos, &b_pos);
    if (vector_len(&radius_vec) > (a->r + b->r)) {
        return false;
    }

    point->normal = vector_normalize(&radius_vec);
    point->point = vector_mul_on_scalar(&point->normal, a->r);
    point->point = vector_add(&point->point, &a->offset);
    return true;
}
