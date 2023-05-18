// #define LOG_LVL LOG_LVL_DEBUG
#include "util.h"
#include "physic-util.h"

struct vector2f rotate_axis(const struct vector2f *p, float angle)
{
    // https://en.wikipedia.org/wiki/Rotation_of_axes
    float c = cos(angle);
    float s = sin(angle);
    return (struct vector2f) {
        .x = p->x * c - p->y * s,
        .y = p->x * s + p->y * c
    };
}
