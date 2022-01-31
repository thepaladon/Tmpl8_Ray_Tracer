#pragma once
#include "hittable.h"

namespace Tmpl8
{



	class sphere4 : public hittable
	{
	public: 
		sphere4() = default;
		sphere4(point3 pos, float p_rad, std::shared_ptr<material> p_mat_ptr) :rad(p_rad), mat_ptr(p_mat_ptr) { center = pos; r2 = rad * rad; };
		~sphere4() = default;

		bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
		bool boundingBox(float t0, float t1, aabb& box) const override;
		static void get_sphere4_uv(const point3& p, float& u, float& v);

	private:
		float rad;
		float r2;
		std::shared_ptr<material> mat_ptr;
;	};

	
}
