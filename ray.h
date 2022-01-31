#pragma once
#include "template.h"

namespace Tmpl8
{
    class ray {
    public:
        ray() = default; 
        ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction.normalized())
        {}

        point3 origin() const { return orig; }
        vec3 direction() const { return dir; }

        point3 at(float t) const {
            return orig + t * dir;
        }

    private:
        point3 orig;
        vec3 dir;
    };
}