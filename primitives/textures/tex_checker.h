//

#ifndef RAYTRACINGWEEKEND_CHECKER_H
#define RAYTRACINGWEEKEND_CHECKER_H
#include "tex_color.h"
#include "../../texture.h"

class tex_checker : public texture
{
public:
	tex_checker(double scale, shared_ptr<texture> even, shared_ptr<texture> odd) :
		inv_scale(1.0/scale), even(even), odd(odd) {}

	tex_checker(double scale, const color& c1, const color& c2) :
		tex_checker(scale, make_shared<tex_color>(c1), make_shared<tex_color>(c2)) {}

	color value(double u, double v, const point3& p) const override
	{
		auto x_int = int(std::floor(inv_scale*u));
		auto y_int = int(std::floor(inv_scale*v));
		// auto z_int = int(std::floor(inv_scale*p.z()));

		bool is_even = (x_int + y_int) % 2 == 0;

		return is_even ? even->value(u, v, p) : odd->value(u, v, p);
	}

private:
	double inv_scale;
	shared_ptr<texture> even, odd;
};

#endif //RAYTRACINGWEEKEND_CHECKER_H