#ifndef INCLUDE_VECTOR_H
#define INCLUDE_VECTOR_H

struct vector2f {
    float x;
    float y;
};

#define LOG_VECTOR(log_func, vector) log_func(#vector"(%f;%f)", vector.x, vector.y)

/// @brief Get vector len
/// @param a vector
/// @return vector len
float vector_len(const struct vector2f *a);
/// @brief Normalize vector
/// @param a vector
/// @return normolized vector
struct vector2f vector_normalize(const struct vector2f *a);
/// @brief Substract b from a
/// @param a vector a
/// @param b vector b
/// @return (a - b)
struct vector2f vector_sub(const struct vector2f *a, const struct vector2f *b);
/// @brief Add a and b
/// @param a vector a
/// @param b vector b
/// @return (a + b)
struct vector2f vector_add(const struct vector2f *a, const struct vector2f *b);
/// @brief Scalar multiply a and b
/// @param a vector a
/// @param b vector b
/// @return Scalar multiply a and b
float vector_scalar_mul(const struct vector2f *a, const struct vector2f *b);
/// @brief Get projection of a on b
/// @param a vector a
/// @param b vector b
/// @return projection of a on b
float vector_projection(const struct vector2f *a, const struct vector2f *b);
/// @brief Multiply vector a on scalar
/// @param a vector a
/// @param val scalar
/// @return vector a multipled on scalar
struct vector2f vector_mul_on_scalar(const struct vector2f *a, float val);

#endif // INCLUDE_VECTOR_H
