#include "precomp.h"
#include "light.h"

#include "material.h"

namespace Tmpl8
{

	bool light_list::hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light)
	{
		hit_record temp_rec;
		bool hit_anything = false;

		for (const auto& object : lightObjects)
		{
			if (object->hitLight(r, t_min, t_max, world, rec, light))
			{
				hit_anything = true;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}

	bool point_light::hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light)
	{
		const ray shadowRay = ray(rec.p * rec.ellipsoidSize, pos - rec.p * rec.ellipsoidSize);
		const float sRayNormal = dot(shadowRay.direction(), rec.normal);

		//Checking the angle between the normal and shadow ray and if it doesn't even face the light no need to check
		if (sRayNormal < 0)
		{
			light += vec3(0.f, 0.f, 0.f);
			return true;
		}

		const float shadowRayDist = (pos - rec.p * rec.ellipsoidSize).length();
		const float bisector = dot(shadowRay.direction(), pos - rec.p * rec.ellipsoidSize);
		const float falloff = (1.f / (shadowRayDist * shadowRayDist));
		constexpr float epsilon = .00001f;

		if (falloff >= epsilon) {
			if (!world->hit(shadowRay, t_min, fabsf(shadowRayDist), rec)) {
				const color diffuse = (falloff * m_intensity) * sRayNormal;
				const color specular = falloff * Max(0.f, powf(cos(bisector), m_specular));
				light += m_color * (diffuse + specular);

			}
		}
		return true;
	}

	bool area_light::hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light)
	{
		vec3 delta = pos2 - pos;
		vec3 offset = pos + (random_float() * delta);

		const ray shadowRay = ray(rec.p * rec.ellipsoidSize, (pos + offset) - rec.p * rec.ellipsoidSize);
		const float sRaynormal = dot(shadowRay.direction(), rec.normal);

		//Checking the angle between the normal and shadow ray and if it doesn't even face the light no need to check
		if (sRaynormal < 0)
		{
			light += vec3(0.f, 0.f, 0.f);
			return true;
		}

		const float shadowRayDist = ((pos + offset) - rec.p * rec.ellipsoidSize).length();
		const float bisector = dot(shadowRay.direction(), (pos + offset) - rec.p * rec.ellipsoidSize);
		const float falloff = (1.f / (shadowRayDist * shadowRayDist));
		constexpr float epsilon = .00001f;

		if (falloff >= epsilon) {
			if (!world->hit(shadowRay, t_min, fabsf(shadowRayDist), rec)) {
				const color diffuse = (falloff * m_intensity) * sRaynormal;
				const color specular = falloff * Max(0.f, powf(cos(bisector), m_specular));
				light += m_color * (diffuse + specular);
			}
		}
		return true;
	}

	bool directional_light::hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec,
		color& light)
	{
		const ray shadowRay = ray(rec.p * rec.ellipsoidSize, pos);
		const float sRaynormal = dot(shadowRay.direction(), rec.normal);

		//Checking the angle between the normal and shadow ray and if it doesn't even face the light no need to check
		if (sRaynormal < 0)
		{
			light += vec3(0.f, 0.f, 0.f);
			return true;
		}

		const float shadowRayDist = (rec.p * rec.ellipsoidSize).length();
		const float bisector = dot(shadowRay.direction(), pos);

		if (!world->hit(shadowRay, t_min, fabsf(shadowRayDist), rec)) {
			const color diffuse = sRaynormal;
			const color specular = Max(0.f, powf(cos(bisector), 50.f));
			light += m_color * (diffuse + specular);
		}

		return true;
	}


	

}