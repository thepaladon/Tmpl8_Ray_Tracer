#pragma once
#include <utility>

#include "perlin.h"
#include "precomp.h"
#include "stblib.h"

namespace Tmpl8
{

	class texture
	{
	public:
		virtual color value(float u, float v, const point3& p) const = 0;
		virtual void replaceTexture(const char* filename) = 0;
	};

	class solid_color : public texture
	{
	public:
		solid_color();
		solid_color(color c) : color_value(c) {}

		virtual void replaceTexture(const char* filename)  override {}
		solid_color(float red, float green, float blue) : solid_color(color(red, green, blue)) {}

		virtual color value(float, float, const point3&) const override
		{
			return color_value;
		}

	private:
		color color_value;

	};


	class checker_texture : public texture
	{
	public:
		checker_texture() = default;
		checker_texture(std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd) :even(std::move(_even)), odd(
			std::move(_odd)) {}

		checker_texture(color c1, color c2)
			: even(std::make_shared<solid_color>(c1)), odd(std::make_shared<solid_color>(c2)) {}

		virtual void replaceTexture(const char* filename)  override {}
		virtual color value(float u, float v, const point3& p) const override
		{
			float sines = sinf(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
			if (sines < 0) { return odd->value(u, v, p); }
			else { return even->value(u, v, p); }
		}

	private:
		std::shared_ptr<texture> odd;
		std::shared_ptr<texture> even;
	};

	class noise_texture : public texture
	{
	public:
		noise_texture() {}
		noise_texture(float sc) : scale(sc) {}

		virtual void replaceTexture(const char* filename)  override {}
		virtual color value(float u, float v, const point3& p) const override
		{
			return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z + 10 * noise.turbulence(p)));
		}

	private:
		perlin noise;
		float scale = 1;
	};



	class image_texture : public texture
	{
	public:
		const static int bytes_per_pixel = 3;

		image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

		image_texture(const char* filename) {
			int components_per_pixel = bytes_per_pixel;

			data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

			if (!data) {
				printf("Error: could not load texture image file. \n");
				width = height = 0;
			}

			bytes_per_scanline = bytes_per_pixel * width;
		}
		~image_texture() {
			delete data;

		}

		virtual void replaceTexture(const char* filename)  override
		{
			delete data;
			int components_per_pixel = bytes_per_pixel;
			data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

			if (!data) {
				printf("Error: could not load texture image file. \n");
				width = height = 0;
			}

			bytes_per_scanline = bytes_per_pixel * width;
		}

		color value(float u, float v, const point3& p) const override
		{
			if (data == nullptr) { return color(1.f, 0, 1.f); }

			u = clamp(u, 0.0f, 1.f);
			v = 1.0f - clamp(v, 0.0f, 1.f); // Flip V to image coords

			int i = static_cast<int>(u * width);
			int j = static_cast<int>(v * height);

			// Clamp integer mapping, since actual coordinates should be less than 1.0
			if (i >= width)  i = width - 1;
			if (j >= height) j = height - 1;


			constexpr float color_scale = 1.0f / 255.0f;
			auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

			return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
		}

	private:
		unsigned char* data;
		int width, height;
		int bytes_per_scanline;
	};

}
