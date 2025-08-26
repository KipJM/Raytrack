//
// Created by mayua on 2025/08/24.
//

#ifndef RAYTRACINGWEEKEND_RTWEEKEND_H
#define RAYTRACINGWEEKEND_RTWEEKEND_H

#include <limits>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>

// C++ std usings
using std::make_shared;
using std::shared_ptr;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Util funcs
inline double deg_to_rad(double deg)
{
	return deg * pi / 180.0;
}

/// Returns random double from [0, 1).
inline double rand_double()
{
	// c++ 11
	static std::uniform_real_distribution<double> distribution(0.0, 1.0); // [0, 1)
	static std::mt19937 generator;

	return distribution(generator);
}

/// Returns random double from [min, max).
inline double rand_double(double min, double max)
{
	return min + (max-min) * rand_double();
}

// Common headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif //RAYTRACINGWEEKEND_RTWEEKEND_H