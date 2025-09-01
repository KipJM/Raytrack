#include <chrono>

#include "misc.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "primitives/materials/basic_materials.h"
#include "primitives/geometry/sphere.h"
#include "primitives/geometry/disk.h"
#include "primitives/textures/tex_checker.h"

//stub

int main()
{
	hittable_list world;

	// Material
	auto tex_checker = make_shared<::tex_checker>(0.32, color(0,0,0), color(1,1,1));
	auto mat_ground = make_shared<lambert>(tex_checker);
	world.add(make_shared<disk>(0,500, mat_ground));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto choose_mat = rand_double();
			point3 center(a + 0.9*rand_double(), 0.2, b+ 0.9*rand_double());

			if ((center - point3(4,0.2,0)).length() > 0.9)
			{
				shared_ptr<material> mat_sphere;

				if (choose_mat < 0.8)
				{
					// diffuse
					auto albedo = color::random() * color::random();

					auto center_end = center + vec3::up * rand_double(0,.5);

					mat_sphere = make_shared<lambert>(albedo);
					world.add(make_shared<sphere>(center, center_end, 0.2, mat_sphere));
				}
				else if (choose_mat < 0.95)
				{
					// metallic
					auto albedo = color::random(.5, 1);
					auto roughness = rand_double(0, .5);
					mat_sphere = make_shared<metal>(albedo, roughness);
					world.add(make_shared<sphere>(center, 0.2, mat_sphere));
				}
				else
				{
					// transmission
					mat_sphere = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, .2, mat_sphere));
				}
			}
		}
	}

	auto mat_a = make_shared<dielectric>(1.5);
	auto mat_b = make_shared<lambert>(color(.4,.2,.1));
	auto mat_c = make_shared<metal>(color(.7,.6,.5), 0);

	world.add(make_shared<sphere>(point3(0,1,0), 1, mat_a));
	world.add(make_shared<sphere>(point3(-4,1,0), 1, mat_b));
	world.add(make_shared<sphere>(point3(4,1,0), 1, mat_c));

	world = hittable_list(make_shared<bvh_node>(world));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.output = std::ofstream("image.ppm");

	cam.vfov = 20;
	cam.position = point3(13,2,3);
	cam.lookat = point3(0,0,0);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = .6;
	cam.focus_distance = 10.0;

	cam.sample_count = 100;
	cam.max_bounces = 24;

	cam.render(world);
}