#pragma once

namespace Tmpl8
{
	class perlin
	{
	public:
		perlin()
		{
			rand_vec = new vec3[point_count];

			for (int i = 0; i < point_count; ++i)
			{
				rand_vec[i] = unit_vector(vec3::random());
			}

			perm_x = perlin_generate_perm();
			perm_y = perlin_generate_perm();
			perm_z = perlin_generate_perm();
		}

		~perlin()
		{
			delete[] rand_vec;
			delete[] perm_x;
			delete[] perm_y;
			delete[] perm_z;
		}

		float noise(const point3& p) const
		{
			auto u = p.x - floorf(p.x);
			auto v = p.y - floorf(p.y);
			auto w = p.z - floorf(p.z);

			auto i = static_cast<int>(floorf(p.x));
			auto j = static_cast<int>(floorf(p.y));
			auto k = static_cast<int>(floorf(p.z));
			vec3 c[2][2][2];

			for (int di = 0; di < 2; di++)
				for (int dj = 0; dj < 2; dj++)
					for (int dk = 0; dk < 2; dk++)
						c[di][dj][dk] = rand_vec
						[
							perm_x[(i + di) & 255] ^
							perm_y[(j + dj) & 255] ^
						perm_z[(k + dk) & 255]
						];

			return trilinear_interp(c, u, v, w);
		}

		float turbulence(const point3& p, int depth = 7) const
		{
			float accum = 0.0f;
			vec3 temp_p = p;
			float weight = 1.0f;

			for (int i = 0; i < depth; i++)
			{
				accum += weight * noise(temp_p);
				weight *= 0.5;
				temp_p *= 2;
			}

			return fabsf(accum);
		}

	private:
		static const int point_count = 256;
		vec3* rand_vec;
		int* perm_x;
		int* perm_y;
		int* perm_z;

		static int* perlin_generate_perm()
		{
			auto p = new int[point_count];

			for (int i = 0; i < point_count; i++) { p[i] = i; }

			permute(p, point_count);

			return p;
		}

		static void permute(int* p, int n)
		{
			for (int i = n - 1; i > 0; i--)
			{
				int target = static_cast<int>(random_float(0, (float)i));
				int tmp = p[i];
				p[i] = p[target];
				p[target] = tmp;
			}
		}

		static float trilinear_interp(vec3 c[2][2][2], float u, float v, float w) {
			auto uu = u * u * (3 - 2 * u);
			auto vv = v * v * (3 - 2 * v);
			auto ww = w * w * (3 - 2 * w);
			auto accum = 0.0f;


			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						vec3 weight_v(u - i, v - j, w - k);
						accum += (i * uu + (1 - i) * (1 - uu))
							* (j * vv + (1 - j) * (1 - vv))
							* (k * ww + (1 - k) * (1 - ww))
							* dot(c[i][j][k], weight_v);

					}



			return accum;
		}


	};
}




