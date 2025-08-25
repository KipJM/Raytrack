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

// Common headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif //RAYTRACINGWEEKEND_RTWEEKEND_H