#pragma once

#include "hittable.h"

namespace Tmpl8
{
	

class sphere : public hittable
{
public:
	sphere() = default;
	sphere(point3 cen, float r, std::shared_ptr<material> m) : radius(r), mat_ptr(m) { center = cen; rad2 = radius * radius; invRad = 1.f / radius; };

	virtual bool hit(
		const ray& r, float t_min, float t_max, hit_record& rec) const override;

	virtual bool boundingBox(float t0, float t1, aabb& box) const override;

	static void get_sphere_uv(const point3& p, float& u, float& v)
	{
		  // p: a given point on the sphere of radius one, centered at the origin.
		  // u: returned value [0,1] of angle around the Y axis from X=-1.
		  // v: returned value [0,1] of angle from Y=-1 to Y=+1.
		  //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		  //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		  //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		const auto theta = acosf(-p.y);
		const auto phi = atan2f(-p.z, p.x) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}

public:
	float radius;
	float rad2;
	float invRad;
	std::shared_ptr<material> mat_ptr;
};
}
