#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;


int to_sdr(double intensity)
{
	static const interval sdr_range(0.000, 0.999);
	return int(256 * sdr_range.clamp(intensity));
}

void write_color(std::ostream& out, const color& pixel_color)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	// PPM only support SDR. TODO: Use another format to achieve HDR
	int rbyte = to_sdr(r);
	int gbyte = to_sdr(g);
	int bbyte = to_sdr(b);

	// Write out color
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif