#pragma once

#include "hittable.h"
#include "aabb.h"
#include <vector>
using std::shared_ptr;
using std::make_shared;

namespace Tmpl8
{
	class hittable_list : public hittable {
	public:
		hittable_list() = default;
		hittable_list(hittable* object) { add(object); }

		void clear() { objects.clear(); }
		void add(hittable* object) { objects.push_back(object); } 
		void deleteAll() { for (auto& object : objects) { delete object; } }

		virtual bool hit(
			const ray& r, float t_min, float t_max, hit_record& rec) const override;
		virtual bool boundingBox(float t0, float t1, aabb& box) const override;


	public:
		std::vector<hittable*> objects;

	};

	inline bool hittable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
	{
		hit_record temp_rec;
		bool hit_anything = false;
		auto closest_so_far = t_max;

		for (const auto& object : objects)
		{
			if (object->hit(r, t_min, closest_so_far, temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	};

	inline bool hittable_list::boundingBox(float t0, float t1, aabb& box) const
	{
		if (objects.empty()) return false;

		aabb temp_box;
		bool first_box = true;

		for (const auto& object : objects) {
			if (!object->boundingBox(t0, t1, temp_box)) return false;
			box = first_box ? temp_box : surroundingBox(box, temp_box);
			first_box = false;
		}

		return true;
	}

}



