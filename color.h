#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;

// approximation: gamma 2 inverse
inline float linear_to_gamma(float linear_intensity)
{
	if (linear_intensity > 0)
	{
		return std::sqrt(linear_intensity);
	}

	return 0;
}

inline float to_sdr(float intensity)
{
	static const interval sdr_range(0.000, 1.000);
	return sdr_range.clamp(linear_to_gamma(intensity));
}

inline void write_color(std::vector<float>& out, const color& pixel_color)
{
	// auto r = pixel_color.x();
	// auto g = pixel_color.y();
	// auto b = pixel_color.z();
	//
	// // PPM only support SDR. TODO: Use another format to achieve HDR
	// unsigned char rbyte = to_sdr(r);
	// unsigned char gbyte = to_sdr(g);
	// unsigned char bbyte = to_sdr(b);
	//
	// // Write out color
	// out.push_back(rbyte);
	// out.push_back(gbyte);
	// out.push_back(bbyte);
	out.push_back(static_cast<float>(pixel_color.x()));
	out.push_back(static_cast<float>(pixel_color.y()));
	out.push_back(static_cast<float>(pixel_color.z()));
}
#endif