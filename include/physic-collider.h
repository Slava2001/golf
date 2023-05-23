#ifndef INCLUDE_PHYSIC_COLLIDER_H
#define INCLUDE_PHYSIC_COLLIDER_H

struct physic_world w;

/// @brief Resolve collisons
/// @param w world
/// @return 0 on ok, -1 on fail
int physic_resolve_collisions(struct physic_world *w);

#endif // INCLUDE_PHYSIC_COLLIDER_H
