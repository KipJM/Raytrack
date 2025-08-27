#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"


//stub

int main()
{
	hittable_list world;

	// Material
	auto material_ground = make_shared<lambert>(color(0.8, 0.8, 0.0));
	auto material_diffuse = make_shared<lambert>(color(0.1, 0.2, 0.5));
	auto material_dielectric = make_shared<dielectric>(1.5);
	auto material_inside = make_shared<dielectric>(1.0/1.5);
	auto material_metal_b  = make_shared<metal>(color(0.8, 0.6, 0.2), 0);

	world.add(make_shared<sphere>(point3(0,0,-1),			0.5, material_dielectric	));
	world.add(make_shared<sphere>(point3(0,0,-1),			0.3, material_inside		));
	world.add(make_shared<sphere>(point3(.3,0.3,-.5),		0.3, material_metal_b		));
	world.add(make_shared<sphere>(point3(-.5, .4, -1.1),	0.5, material_diffuse		));
	world.add(make_shared<sphere>(point3(0,-100.5,-1),		100, material_ground		));


	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.output = std::ofstream("image.ppm");

	cam.vfov = 20;
	cam.position = point3(-2,2,1);
	cam.lookat = point3(0,0,-1);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = 10.0;
	cam.focus_distance = 3.4;

	cam.sample_count = 100;
	cam.max_bounces = 50;

	cam.render(world);
}