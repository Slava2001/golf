#ifndef INCLUDE_PHYSIC_OBJECT_H
#define INCLUDE_PHYSIC_OBJECT_H

#include "vector.h"

#include <stdbool.h>

struct physic_world;

enum physic_primitive_type {
    PPT_RECTANGLE,
    PPT_CIRCLE
};

#define PHYSIC_INFINITY_MASS 0

struct physic_primitive_description {
    enum physic_primitive_type type;
    float mass;
    float offset_x, offset_y, angle;
    union {
        struct { // rectangle
            float h;
            float w;
        };
        struct { // circle
            float r;
        };
    };
};

struct physic_primitive {
    enum physic_primitive_type type;
    float mass;
    struct vector2f offset;
    float angle;

    float moment_of_inertia;

    union {
        struct { // rectangle
            float h;
            float w;
        };
        struct { // circle
            float r;
        };
    };
};

struct physic_aabb {
    struct vector2f min;
    struct vector2f max;
    struct vector2f delta;
};

struct physic_object_description {
    float pos_x, pos_y, angle;
    float speed_x, speed_y, speed_a;
};

#define PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT 10

struct physic_object {
    float inv_mass;
    float inv_moment_of_inertia;
    struct physic_primitive primitives[PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT];
    unsigned primitives_count;
    struct physic_aabb aabb;
    bool is_collided;

    struct vector2f mass_center;
    struct vector2f zero_offset;
    float angle;
    struct vector2f speed;
    float speed_a;
};

/// @brief add object in world
/// @param w world
/// @param d object discription
/// @return object descriptor on ok, -1 on fail
int physic_add_object(struct physic_world *w, struct physic_object_description *d);
/// @brief add primitive in object
/// @param w world
/// @param od object discriptor
/// @param d primitive discription
/// @return primitive descriptor on ok, -1 on fail
int physic_add_primitive(struct physic_world *w, int od, struct physic_primitive_description *d);
/// @brief apply impulse to object
/// @param w world
/// @param od object descriptor
/// @param impulse impulse
/// @param point point to apply impulse
/// @return 0 on ok, -1 on fail
int physic_object_apply_impulse(struct physic_world *w, int od, const struct vector2f *impulse,
                                                                const struct vector2f *point);
int update_collaider(struct physic_world *w, int od);
int update_obj_mass(struct physic_world *w, int od);
struct physic_aabb get_primitive_aabb(const struct physic_primitive *p, struct vector2f offset,
                                      float angle);

#endif // INCLUDE_PHYSIC_OBJECT_H
