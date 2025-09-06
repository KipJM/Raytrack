#include <chrono>

#include "misc.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "primitives/materials/debug.h"
#include "primitives/geometry/sphere.h"
#include "primitives/geometry/disk.h"
#include "primitives/geometry/quad.h"
#include "primitives/textures/tex_checker.h"
#include "primitives/textures/tex_image.h"
#include "primitives/textures/tex_perlin.h"
#include "primitives/textures/tex_uv_debug.h"
#include "primitives/materials/diffuse.h"
#include "primitives/materials/metallic.h"
#include "primitives/materials/translucent.h"
#include "primitives/materials/debug.h"



//stub

void scn_lots_of_spheres()
{
	hittable_list world;

	// Material
	auto tex_checker = make_shared<::tex_checker>(0.32, color(0,0,0), color(1,1,1));
	auto mat_ground = make_shared<diffuse>(tex_checker);
	world.add(make_shared<disk>(point3(0,0,0), vec3(200, 0, 0), vec3(0,0,-200), mat_ground));

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

					mat_sphere = make_shared<diffuse>(albedo);
					world.add(make_shared<sphere>(center, center_end, 0.2, mat_sphere));
				}
				else if (choose_mat < 0.95)
				{
					// metallic
					auto albedo = color::random(.5, 1);
					auto roughness = rand_double(0, .5);
					mat_sphere = make_shared<metallic>(albedo, roughness);
					world.add(make_shared<sphere>(center, 0.2, mat_sphere));
				}
				else
				{
					// transmission
					mat_sphere = make_shared<translucent>(1.5);
					world.add(make_shared<sphere>(center, .2, mat_sphere));
				}
			}
		}
	}

	auto mat_a = make_shared<translucent>(1.5);
	auto mat_b = make_shared<diffuse>(color(.4,.2,.1));
	auto mat_c = make_shared<metallic>(color(.7,.6,.5), 0);

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

	cam.sample_count = 10;
	cam.max_bounces = 24;

	cam.render(world);
}

void scn_checked_spheres()
{
	hittable_list world;
	auto checker_texture = make_shared<tex_checker>(.03, color(.2, .3, .1), color(.9, .9, .9));

	world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<diffuse>(checker_texture)));
	world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<diffuse>(checker_texture)));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;

	cam.sample_count = 100;
	cam.max_bounces = 50;

	cam.output = std::ofstream("image.ppm");

	cam.vfov = 20;
	cam.position = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;
	cam.focus_distance = 10;

	cam.render(world);
}

void scn_earth()
{
	auto earth_texture = make_shared<tex_image>("earthmap.jpg");
	// auto earth_texture = make_shared<tex_uv_debug>();
	auto earth_material = make_shared<diffuse>(earth_texture);
	auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_material);

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.sample_count = 100;
	cam.max_bounces = 50;

	cam.vfov = 20;
	cam.position = point3(0,0,12);
	cam.lookat = point3(0,0,0);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = 0;

	cam.output = std::ofstream("image.ppm");

	cam.render(hittable_list(globe));
}

void scn_perlin()
{
	hittable_list world;

	auto noise_tex = make_shared<tex_perlin>(4, 7);
	auto noise_mat = make_shared<diffuse>(noise_tex);
	world.add(make_shared<sphere>(point3(0,2,0), 2, noise_mat));
	world.add(make_shared<sphere>(point3(0,-1000,0), 1000, noise_mat));

	camera cam;

	cam.aspect_ratio      = 16.0 / 9.0;
	cam.image_width       = 400;
	cam.sample_count	  = 100;
	cam.max_bounces       = 50;

	cam.vfov     = 20;
	cam.position = point3(13,2,3);
	cam.lookat   = point3(0,0,0);
	cam.vup      = vec3(0,1,0);

	cam.defocus_angle = 0;

	cam.output = std::ofstream("image.ppm");


	// cam.render(bvh_node(world));

	cam.render(world);
}

void scn_quads()
{
	hittable_list world;

	// mats
	auto left_red     = make_shared<diffuse>(color(1.0, 0.2, 0.2));
	auto back_green   = make_shared<diffuse>(color(0.2, 1.0, 0.2));
	auto right_blue   = make_shared<diffuse>(color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<diffuse>(color(1.0, 0.5, 0.0));
	auto lower_teal   = make_shared<diffuse>(color(0.2, 0.8, 0.8));

	// geo
	world.add(make_shared<disk>(point3(-3,-2,5), vec3(0,0,-4), vec3(0,4,0), left_red));
	world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
	world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
	world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
	world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

	camera cam;

	cam.aspect_ratio = 1.0;
	cam.image_width = 400;
	cam.sample_count = 20;
	cam.max_bounces = 50;

	cam.vfov = 80;
	cam.position = point3(0,0,9);
	cam.lookat = point3(0,0,0);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = 0;

	cam.output = std::ofstream("image.ppm");

	cam.render(hittable_list(make_shared<bvh_node>(world)));
}

int main(int argc, char** argv)
{
	// scn_lots_of_spheres();
	// scn_checked_spheres();
	// scn_earth();
	// scn_perlin();
	scn_quads();
}