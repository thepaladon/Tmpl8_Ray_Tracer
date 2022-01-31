#pragma once
#include "hittable.h"

namespace Tmpl8
{

	class ellipsoid : public hittable
	{
	public:
		ellipsoid() = default;
		ellipsoid(point3 cen,  vec3 ellipseProperties, std::shared_ptr<material> m) : mat_ptr(m), ellipsoidRadius(ellipseProperties) 
		{
			center = cen;
			ellipsoidInvRad = vec3(1.f) / ellipsoidRadius;
		}
		
		static void get_sphere_uv(const point3& p, float& u, float& v)
		{
			// p: a given point on the sphere of radius one, centered at the origin.
			// u: returned value [0,1] of angle around the Y axis from X=-1.
			// v: returned value [0,1] of angle from Y=-1 to Y=+1.
			//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
			//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
			//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

			const float theta = acosf(-p.y);
			const float phi = atan2f(-p.z, p.x) + PI;

			u = phi / (2 * PI);
			v = theta / PI;
		}

		virtual bool hit(
			const ray& r, float t_min, float t_max, hit_record& rec) const override;

		bool boundingBox(float t0, float t1, aabb& box) const override;


	private:
		std::shared_ptr<material> mat_ptr; 
		vec3 ellipsoidRadius;
		vec3 ellipsoidInvRad;
	};

	inline bool ellipsoid::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
	{
		//Implementation Logic: 
		//step 1 -> imagine you have an ellipsoid and draw it as a sphere (ellipsoidRad(1.f,1.f,1.f))
		//step 2 -> stretch it with ray transformation and see whether that yields proper normal and shape 
		// ...if not see where you missed to divide the ellipsoid values
		//step 3 -> for the materials you'll have to do the opposite of the transformation to get proper results. a.k.a.
		//...take the opposite of what you did to stretch it to an ellipsoid 

		const vec3 oc = r.origin() * ellipsoidInvRad - center * ellipsoidInvRad;
		const float a = (r.direction() * ellipsoidInvRad).sqrLentgh() ;
		const float half_b = dot(oc, (r.direction() * ellipsoidInvRad));
		const float c = oc.sqrLentgh() - 1.f;

		const float discriminant = half_b * half_b - a * c;
		if (discriminant < 0) return false; //it doesn't hit
		const float sqrt_D = sqrtf(discriminant);

		//Find the nearest root that lies in the acceptable range
		float root = (-half_b - sqrt_D) / a;
		if (root < t_min || t_max < root)
		{
			root = (-half_b + sqrt_D) / a;
			if (root < t_min || t_max < root)
			{
				return false; //too far away / behind, so it doesn't hit 
			}
		}

		rec.t = root;
		rec.p = r.origin() * ellipsoidInvRad + r.direction() * rec.t * ellipsoidInvRad;
		const vec3 outward_normal = ((rec.p - center * ellipsoidInvRad) * ellipsoidInvRad).normalized();

		//Spherical coordinates
		
		//IMPLEMENTING PROPER NORMAL STRUGGLES
		/*
		const vec3 test = (r.origin() / ellipsoidRadius + r.direction() / ellipsoidRadius * rec.t).normalized();

		//DERIVATIVE SOLUTION
		const vec3 derivative1 = vec3(
			-ellipsoidRadius.x * sinf(phi) * cosf(theta),
			-ellipsoidRadius.y * sinf(phi) * sinf(theta),
			ellipsoidRadius.z * cosf(phi));

		const vec3 derivative2 = vec3(
		-ellipsoidRadius.x * cosf(phi) * sinf(theta),
		ellipsoidRadius.y * cosf(phi) * cosf(theta),
		0);

		//SPHERICAL FORMULA SOLUTION
		const vec3 outward_normal = vec3( 
		ellipsoidInvRad.x * cosf(phi) * cosf(theta),
		ellipsoidInvRad.y * cosf(phi) * sinf(theta), 
		ellipsoidInvRad.z * sinf(phi));

		const vec3 outward_normal = (((rec.p - center) * ellipsoidInvRad) * ((rec.p - center) * ellipsoidInvRad)).normalized();
		rec.normal = cross(derivative1, derivative2);*/

		rec.normal = outward_normal;
		rec.set_face_normal_ellipsoid(r, outward_normal, ellipsoidRadius);
		//get_sphere_uv(outward_normal, rec.u, rec.v);
		rec.ellipsoidSize = ellipsoidRadius;
		rec.mat_ptr = mat_ptr;

		return true;
	}

	inline bool ellipsoid::boundingBox(float t0, float t1, aabb& box) const
	{
		box = aabb(
		center - ellipsoidRadius,
		center + ellipsoidRadius);
	return true;
	}
}
