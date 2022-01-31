#include "precomp.h"
#include "sphere.h"

namespace Tmpl8
{

	bool sphere::boundingBox(float t0, float t1, aabb& box) const
	{
		box = aabb(
			center - vec3(radius, radius, radius),
			center + vec3(radius, radius, radius));
		return true;
	}


	 bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
	{
		const vec3 oc = r.origin() - center;
		const float half_b = dot(oc, r.direction());
		const float c = oc.sqrLentgh() - rad2;

		const float discriminant = half_b * half_b - c;
		if (discriminant < 0) return false;
		const float sqrt_discr = sqrtf(discriminant);

		//Find the nearest root that lies in the acceptable range
		float root = (-half_b - sqrt_discr);

		if (root < t_min || t_max < root)
		{
			root = (-half_b + sqrt_discr);
			if (root < t_min || t_max < root)
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		const vec3 outward_normal = (rec.p - center) * invRad;
		rec.set_face_normal(r, outward_normal);
		get_sphere_uv(outward_normal, rec.u, rec.v);
		rec.mat_ptr = mat_ptr;

		return true;
	}

}