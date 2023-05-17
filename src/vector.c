#include "vector.h"

#include <math.h>

float vector_len(const struct vector2f *a)
{
    return sqrtf(a->x * a->x + a->y * a->y);
}

struct vector2f vector_normalize(const struct vector2f *a)
{
    float len = vector_len(a);
    return (struct vector2f) { .x = a->x / len, .y = a->y / len };
}

struct vector2f vector_sub(const struct vector2f *a, const struct vector2f *b)
{
    return (struct vector2f) { .x = a->x - b->x, .y = a->y - b->y };
}

struct vector2f vector_add(const struct vector2f *a, const struct vector2f *b)
{
    return (struct vector2f) { .x = a->x + b->x, .y = a->y + b->y };
}

float vector_scalar_mul(const struct vector2f *a, const struct vector2f *b)
{
    return a->x * b->x + a->y * b->y;
}

float vector_projection(const struct vector2f *a, const struct vector2f *b)
{
    return vector_scalar_mul(a, b) / vector_len(b);
}

struct vector2f vector_mul_on_scalar(const struct vector2f *a, float val)
{
    return (struct vector2f) { .x = a->x * val, .y = a->y * val };
}
