#pragma once
#include "hittable.h"

namespace Tmpl8
{

	class light
	{
	public:
		virtual bool hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light) = 0;

		void setPos(vec3 newPos) { pos = newPos; }
		vec3 pos;
	};


	class light_list : public light
	{
	public:
		light_list() = default;
		light_list(std::shared_ptr<light> object) { add(object); }

		void clear() { lightObjects.clear(); }
		void add(const std::shared_ptr<light> object) { lightObjects.push_back(object); }

		virtual bool hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light) override;

		std::vector<std::shared_ptr<light>> lightObjects;
	}; 



	class point_light : public light
	{
	public:

		point_light() = default;
		point_light(const vec3 position, const float intensity, const float specular, const color c) : m_color(c), m_intensity(intensity), m_specular(specular) { pos = position; }
		~point_light() {};

		virtual bool hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light) override;

	private:
		color m_color;
		float m_intensity = 1;
		float m_specular = 1;
	};

	class area_light : public light
	{
	public:
		area_light() = default;
		area_light(const vec3 position, const vec3 position2, const float intensity, const float specular, const color c) : m_color(c), m_intensity(intensity), m_specular(specular) { pos = position; pos2 = position2; }
		~area_light() {};

		virtual bool hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light) override;

	private:
		point3 pos2;
		color m_color;
		float m_intensity = 1;
		float m_specular = 1;
	};

	class directional_light : public light
	{
	public:
		directional_light() = default;
		directional_light(const vec3 position, const color c) : m_color(c) { pos = position; }
		~directional_light() = default;

		virtual bool hitLight(const ray r, float t_min, float t_max, const hittable* world, hit_record rec, color& light) override;

	private:
		color m_color;
	};

}



