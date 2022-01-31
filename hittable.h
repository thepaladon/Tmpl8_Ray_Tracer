#pragma once
#include "precomp.h"

#include "aabb.h"
#include "ray.h"

namespace Tmpl8
{
    class material;

    struct hit_record {
        point3 p;
        vec3 normal;
        std::shared_ptr<material> mat_ptr;
        float t;
        float u; //UV for UV mapping
        float v;
        bool front_face = false;
        vec3 ellipsoidSize = 1.f; 

        virtual void set_face_normal(const ray& r, const vec3& outward_normal)
        {
            //This is for when a sphere is hit from the inside. we FLIP the hemisphere. 
            front_face = dot(r.direction() , outward_normal) < 0; 
            normal = front_face ? outward_normal : -outward_normal;
        }

        virtual void set_face_normal_ellipsoid(const ray& r, const vec3& outward_normal, const vec3 ellips_rad)
        {
            //This is for when a sphere is hit from the inside. we FLIP the hemisphere. 
            front_face = dot((r.direction() /ellips_rad ), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }


    };

    class hittable {
    public:
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
        virtual bool boundingBox(float t0, float t1, aabb& box) const = 0;
        void setPosition(vec3 newPosition) { center = newPosition; }
    public:
        vec3 center;
    };
}

