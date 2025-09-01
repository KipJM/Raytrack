//

#ifndef RAYTRACINGWEEKEND_TEXTURE_H
#define RAYTRACINGWEEKEND_TEXTURE_H
#include "color.h"
#include "vec3.h"


class texture
{
public:
	virtual ~texture() = default;

	virtual color value(double u, double v, const point3& p) const = 0;

};


#endif //RAYTRACINGWEEKEND_TEXTURE_H