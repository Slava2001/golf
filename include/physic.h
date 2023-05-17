#ifndef INCLUDE_PHYSIC_H
#define INCLUDE_PHYSIC_H

#include <stdbool.h>

#define PHYSIC_INFINITY_MASS 0

struct physic_aabb {
    float min_x, max_x;
    float min_y, max_y;
    float delta_x, delta_y;
};

struct point2f {
    float x;
    float y;
};

struct physic_debug_drawer_api {
    void (*circle)(float x, float y, float r, float a);
    void (*box)(float x, float y, float h, float w, float a);
    void (*aabb)(float x, float y, float h, float w, bool is_collided);
};

enum physic_primitive_type {
    PPT_RECTANGLE,
    PPT_CIRCLE
};

struct physic_primitive_description {
    enum physic_primitive_type type;
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

struct physic_object_description {
    float mass;
    float pos_x, pos_y, angle;
    float speed_x, speed_y, speed_a;
};

#define PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT 10

struct physic_object {
    float inv_mass;
    struct physic_primitive primitives[PHYSIC_PRIMITIVIS_IN_OBJECT_MAX_COUNT];
    unsigned primitives_count;
    struct physic_aabb aabb;
    bool is_collided;

    float pos_x, pos_y, angle;
    float speed_x, speed_y, speed_a;
};

#define PHYSIC_OBJECT_MAX_COUNT 100

struct physic_world {
    int draw_flag;
    const struct physic_debug_drawer_api *draw_api;

    struct physic_object objects[PHYSIC_OBJECT_MAX_COUNT];
    unsigned object_count;
};

enum physic_debug_draw_flag {
    PDDF_OBJECTS = 1 << 0,
    PDDF_AABB    = 1 << 1
};

/// @brief init physic world
/// @param w world
/// @return 0 on ok, -1 on fail
int physic_init_world(struct physic_world *w);
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
/// @return 0 on ok, -1 on fail
int physic_object_apply_impulse(struct physic_world *w, int od, const struct point2f *impulse);
/// @brief update phisic
/// @param w world
/// @param elapsed_time_s elapsed time in seconds
/// @return 0 on ok, -1 on fail
int physic_update(struct physic_world *w, float elapsed_time_s);
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

#endif // INCLUDE_PHYSIC_H
