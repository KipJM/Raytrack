#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"


//stub

int main()
{
	hittable_list world;

	world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
	world.add(make_shared<sphere>(point3(.3,0.3,-.5), 0.3));
	world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));


	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.output = std::ofstream("image.ppm");

	cam.render(world);
}