#pragma once

#include "game.h"
#include "ray.h"
#include <complex>
#include "precomp.h"

namespace  Tmpl8
{
	class camera
	{
	public:
		camera(
			vec3 lookFrom,
			vec3 lookAt, 
			vec3 vup,
			float vfov,
			float aspect_ratio_of_cam,
			float aperture,
			float focus_dist)
		{
			auto theta = degrees_to_radians(vfov);
			auto h = tanf(theta / 2.f);
			auto viewport_height = 2.0f * h;
			auto viewport_width = aspect_ratio_of_cam * viewport_height;

			//complete orthonormal basis
			w = unit_vector(lookFrom - lookAt);
			u = unit_vector(cross(vup, w));
			v = cross(w, u);

			origin = lookFrom;
			focus_distance = focus_dist;
			horizontal = viewport_width * u * focus_dist;
			vertical = viewport_height * v * focus_dist;
			lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

			lens_radius = aperture / 2;

		}

		void setCameraPosition(vec3 lookFrom, vec3 lookAt, vec3 Rotation, float vFOV, bool key_depthOfField)
		{
			const float theta = degrees_to_radians(vFOV);
			float h = tanf(theta / 2.f);
			float viewport_height = 2.0f * h;
			float viewport_width = aspect_ratio * viewport_height;
			defocusBlurON = key_depthOfField;

			//complete orthonormal basis
			w = unit_vector(lookFrom - lookAt);
			u = unit_vector(cross(Rotation, w));
			v = cross(w, u);

			origin = lookFrom;
			horizontal = viewport_width * u * focus_distance;
			vertical = viewport_height * v * focus_distance;
			lower_left_corner = origin - horizontal / 2 - vertical / 2 - w * focus_distance;
		}

		void setFocusDist(float newFocusDist) { focus_distance = newFocusDist; }

		ray get_ray(float s, float t) const
		{
			vec3 rd = vec3(0.f);
			vec3 offset = vec3(0.f);
			if (defocusBlurON) {
				rd = lens_radius * vec3::random_in_unit_sphere();
				offset = u * rd.x + v * rd.y;
			}

			return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
		}

	private:
		bool defocusBlurON = false;
		std::complex<float> mycomplex = (2.20f, 2.0f);
		point3 origin;
		point3 lower_left_corner;
		vec3 horizontal;
		vec3 vertical;
		vec3 u, v, w;
		float lens_radius;
		float focus_distance;

		float rW, rX, rY, rZ;


	};

}
