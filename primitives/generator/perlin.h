#ifndef RAYTRACINGWEEKEND_PERLIN_H
#define RAYTRACINGWEEKEND_PERLIN_H

#include "../../misc.h"

class perlin
{
public:
	perlin()
	{
		for (int i = 0; i < point_count; i++)
		{
			randvec[i] = unit_vector(vec3::random(-1,1));
		}

		perlin_generate_perm(perm_x);
		perlin_generate_perm(perm_y);
		perlin_generate_perm(perm_z);
	}

	double turbulence(const point3& p, int depth) const
	{
		auto accum =0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++)
		{
			accum += weight * noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}

		return std::fabs(accum);
	}

	double noise(const point3& p) const
	{
		auto u = p.x() - std::floor(p.x());
		auto v = p.y() - std::floor(p.y());
		auto w = p.z() - std::floor(p.z());

		auto i = int(std::floor(p.x()));
		auto j = int(std::floor(p.y()));
		auto k = int(std::floor(p.z()));
		vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = randvec[
						perm_x[(i+di) & 255] ^ perm_y[(j+dj) & 255] & perm_z[(k+dk) & 255]
						];
				}
			}
		}

		return perlin_interp(c, u, v, w);
	}

private:
	static const int point_count = 256;
	vec3 randvec[point_count];
	int perm_x[point_count];
	int perm_y[point_count];
	int perm_z[point_count];

	static void perlin_generate_perm(int* arr)
	{
		for (int i = 0; i < point_count; i++)
		{
			arr[i] = i;
		}

		permute(arr, point_count);
	}

	static void permute(int* arr, int n)
	{
		for (int i = n-1; i > 0; i--)
		{
			int target = rand_int(0, i);
			int tmp = arr[i];
			arr[i] = arr[target];
			arr[target] = tmp;
		}
	}

	static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w)
	{
		auto uu = u*u*(3-2*u);
		auto vv = v*v*(3-2*v);
		auto ww = w*w*(3-2*w);
		auto accum = 0.0;

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					vec3 weight_v(u-i, v-j, w-k);
					accum +=	(i*uu + (1-i)*(1-uu)) *
								(j*vv + (1-j)*(1-vv)) *
								(k*ww + (1-k)*(1-ww)) *
								dot(c[i][j][k], weight_v);
				}
			}
		}

		return accum;
	}
};

#endif //RAYTRACINGWEEKEND_PERLIN_H
