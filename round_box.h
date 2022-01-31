#pragma once
#include <xlocmon>

#include "hittable.h"

namespace Tmpl8 {

	class round_box : public hittable
	{
	public:
		round_box() = default;
        round_box(vec3 p_size, float p_rad, std::shared_ptr<material> p_mat_ptr): rad(p_rad), mat_ptr(p_mat_ptr) { center = p_size; }

        bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
        bool boundingBox(float t0, float t1, aabb& box) const override;

	private:
		float rad;
        std::shared_ptr<material> mat_ptr;
	};


    inline bool round_box::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
    {
        rec.ellipsoidSize = 1.f;

        // bounding box
        vec3 m = vec3(1.0) / r.direction();
        vec3 n = m * r.origin();
        vec3 k = abs3(m) * (center + rad);
        vec3 t1 = -n - k;
        vec3 t2 = -n + k;
        float tN = ffmax(ffmax(t1.x, t1.y), t1.z);
        float tF = ffmin(ffmin(t2.x, t2.y), t2.z);
        if (tN > tF || tF < 0.0) return false;
        float t = tN;

        // convert to first octant
        vec3 pos = r.origin() + t * r.direction();
        vec3 s = sgnv3(pos);
        r.origin() *= s;
        r.direction() *= s;
        pos *= s;

        // faces
        pos -= center;
        pos = ffmax(pos, vec3(pos.y, pos.z, pos.x));
        if (ffmin(ffmin(pos.x, pos.y), pos.z) < 0.0) return false;

        // some precomputation
        vec3 oc = r.origin() - center;
        vec3 dd = r.direction() * r.direction();
        vec3 oo = oc * oc;
        vec3 od = oc * r.direction();
        float ra2 = rad * rad;

        t = infinity;

        // corner
        {
            float b = od.x + od.y + od.z;
            float c = oo.x + oo.y + oo.z - ra2;
            float h = b * b - c;
            if (h > 0.0) t = -b - sqrt(h);
        }
        // edge X
        {
            float a = dd.y + dd.z;
            float b = od.y + od.z;
            float c = oo.y + oo.z - ra2;
            float h = b * b - a * c;
            if (h > 0.0)
            {
                h = (-b - sqrt(h)) / a;
                if (h > 0.0 && h < t && abs(r.origin().x + r.direction().x * h) < center.x) t = h;
            }
        }
        // edge Y
        {
            float a = dd.z + dd.x;
            float b = od.z + od.x;
            float c = oo.z + oo.x - ra2;
            float h = b * b - a * c;
            if (h > 0.0)
            {
                h = (-b - sqrt(h)) / a;
                if (h > 0.0 && h < t && abs(r.origin().y + r.direction().y * h) < center.y) t = h;
            }
        }
        // edge Z
        {
            float a = dd.x + dd.y;
            float b = od.x + od.y;
            float c = oo.x + oo.y - ra2;
            float h = b * b - a * c;
            if (h > 0.0) 
            {
                h = (-b - sqrt(h)) / a;
                if (h > 0.0 && h < t && abs(r.origin().z + r.direction().z * h) < center.z) t = h;
            }
        }

        if (t > 1e19) t = -1.0;

    	rec.t = t;
        rec.p = r.origin() + rec.t * r.direction();
        const vec3 outward_normal = sgnv3(pos) * normalize(abs3(pos) - center);
        rec.normal = outward_normal;
    	rec.set_face_normal(r, outward_normal);
        //get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat_ptr = mat_ptr;

        return t;
    }

    inline bool round_box::boundingBox(float t0, float t1, aabb& box) const
    {
        box = aabb(
            center - vec3(rad, rad, rad),
            center + vec3(rad, rad, rad));
        return true;
    }


}