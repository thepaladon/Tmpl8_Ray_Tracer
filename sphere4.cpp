#include "precomp.h"
#include "sphere4.h"

namespace Tmpl8
{
	bool sphere4::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
	{

        // -----------------------------
		// solve quartic equation
		// -----------------------------

        const vec3 ro = r.origin() - center;
        const vec3 d2 = r.direction() * r.direction();
        const vec3 d3 = d2 * r.direction();
        const vec3 o2 = ro * ro;
        const vec3 o3 = o2 * ro;

        const float ka = 1.0f / dot(d2, d2);

        const float k3 = ka * dot(ro, d3);
        const float k2 = ka * dot(o2, d2);
        const float k1 = ka * dot(o3, r.direction());
        const float k0 = ka * (dot(o2, o2) - r2 * r2);

        // -----------------------------
        // solve cubic
        // -----------------------------

        const float c2 = k2 - k3 * k3;
        const float c1 = k1 + 2.0f * k3 * k3 * k3 - 3.0f * k3 * k2;
        const float c0 = k0 - 3.0f * k3 * k3 * k3 * k3 + 6.0f * k3 * k3 * k2 - 4.0f * k3 * k1;

        const float p = c2 * c2 + c0 / 3.0f;
        const float q = c2 * c2 * c2 - c2 * c0 + c1 * c1;

        const float h = q * q - p * p * p;

        // -----------------------------
        // skip the case of three real solutions for the cubic, which involves four
        // complex solutions for the quartic, since we know this object is convex
        // -----------------------------

        if (h < 0.0f) return false;

        // one real solution, two complex (conjugated)
        const float sh = sqrt(h);

        const float s = sgnf(q + sh) * powf(abs(q + sh), 1.0f / 3.0f); // cuberoot
        const float t = sgnf(q - sh) * powf(abs(q - sh), 1.0f / 3.0f); // cuberoot
        const auto w = vec2(s + t, s - t);

        vec2  v = vec2(w.x + c2 * 4.0f, w.y * sqrt(3.0f)) * 0.5f;
        const float rads = v.length();
        const float d = -abs(v.y) / sqrt(rads + v.x) - c1 / rads - k3;

        if(d < t_min || d > t_max) { return false; }

        rec.t = d;

        const vec3 normalFormula = ro + rec.t * r.direction();
        const vec3 outward_normal = normalize(normalFormula * normalFormula * normalFormula);

		rec.p = ro + rec.t * r.direction() + center;
		rec.normal = outward_normal;
		rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

		//get_sphere4_uv(outward_normal, rec.u, rec.v);
        
        return true;
	}

	bool sphere4::boundingBox(float t0, float t1, aabb& box) const
	{
		box = aabb(
			center - vec3(rad, rad, rad),
			center + vec3(rad, rad, rad));
		return true;
	}

    inline void sphere4::get_sphere4_uv(const point3& p, float& u, float& v)
    {
        
    }

}