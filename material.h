#pragma once

#include "hittable.h"
#include "ray.h"
#include "texture.h"

namespace Tmpl8
{
	struct hit_record;

	class material
	{
	public:
		virtual bool scatter(
			const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;

		virtual color emitted(double u, double v, const point3& p) const {
			return color(0, 0, 0);
		}
		 
	};

	class lambertian : public material
	{
	public:
		lambertian(const color& a) : albedo(std::make_shared<solid_color>(a)) {}
		lambertian(std::shared_ptr<texture> a) : albedo(a) {}

		virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
		{
			auto scatter_direction = rec.normal + random_unit_vector();

			if (scatter_direction.near_zero())
			{
				scatter_direction = rec.normal;
			}

			scattered = ray(rec.p * rec.ellipsoidSize, scatter_direction);
			attenuation = albedo->value(rec.u, rec.v, rec.p * rec.ellipsoidSize);
			return true;
		}

	private:
		std::shared_ptr<texture> albedo;
	};

	class metal : public material
	{
	public:
		metal(const color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

		virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
		{
			const vec3 reflected = reflect(unit_vector(r_in.direction() * rec.ellipsoidSize), rec.normal); //calculation of the derivation.
			scattered = ray(rec.p * rec.ellipsoidSize, reflected + fuzz * vec3::random_in_unit_sphere());
			attenuation = albedo;
			return (dot(scattered.direction(), rec.normal) > 0);
		}

	private:
		color albedo;
		float fuzz;
	};

	class dielectric : public material
	{
	public:
		dielectric(vec3 color, float index_of_refraction, float density) : m_addColor(color), ir(index_of_refraction), density(density) {}

		bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
		{

			attenuation = color(1.0f, 1.0f, 1.0f) + m_addColor;
			const float refraction_ratio = rec.front_face ? (1.0f / ir) : ir;
			const vec3 unit_direction = unit_vector(r_in.direction() * rec.ellipsoidSize);
			const float cos_theta = fminf(dot(-unit_direction, rec.normal), 1.0f);
			const float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);

			const bool cannot_refract = refraction_ratio * sin_theta > 1.0f; 
			vec3 direction;

			if(!rec.front_face)
			{
				const float distanceTravelled = (rec.p - r_in.origin()).length();
				attenuation *= expf(-density * distanceTravelled);
			}

			if (cannot_refract || fresnel(rec.p, rec.normal, ir) > rand() / (RAND_MAX + 1.0f))
			{
				direction = reflect(unit_direction, rec.normal);
			}
			else
			{
				direction = refract(unit_direction, rec.normal, refraction_ratio);
			}

			scattered = ray(rec.p * rec.ellipsoidSize, direction);

			return true;
		}

	public:
		vec3 m_addColor;
		float ir;
		float density; //note, negative density results in a glowing sphere


	private:
		static float reflectanceSchlick(float cosine, float ref_idx)
		{
			//Use Schlick's approximation for reflectance
			float r0 = (1.f - ref_idx) / (1.f + ref_idx);
			r0 = r0 * r0;
			return r0 + (1.f + r0) * powf((1 - cosine), 5.f);
		}

		static float fresnel(const vec3& I, const vec3& N, const float& ior)
		{
			float kr;
			float cosi = clamp(-1, 1, dot(I, N));
			float etai = 1, etat = ior;
			if (cosi > 0) { std::swap(etai, etat); }
			// Compute sini using Snell's law
			float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
			// Total internal reflection
			if (sint >= 1) {
				kr = 1;
			}
			else {
				float cost = sqrtf(std::max(0.f, 1 - sint * sint));
				cosi = fabsf(cosi);
				float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
				float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
				kr = (Rs * Rs + Rp * Rp) / 2;
			}

			return kr;
			// As a consequence of the conservation of energy, transmittance is given by:
			// kt = 1 - kr;
		}

	};
}



