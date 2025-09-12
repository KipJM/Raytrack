#include "misc.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "scene.h"
#include "transformers.h"
#include "viewport.h"
#include "volume_convex.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/materials/mat_debug.h"
#include "primitives/geometry/geo_sphere.h"
#include "primitives/geometry/geo_disk.h"
#include "primitives/geometry/geo_quad.h"
#include "primitives/textures/tex_checker.h"
#include "primitives/textures/tex_image.h"
#include "primitives/textures/tex_perlin.h"
#include "primitives/textures/tex_uv_debug.h"
#include "primitives/materials/mat_diffuse.h"
#include "primitives/materials/mat_metallic.h"
#include "primitives/materials/mat_translucent.h"
#include "primitives/materials/mat_debug.h"
#include "primitives/materials/mat_emissive.h"


//stub

void scn_lots_of_spheres()
{
	hittable_list world;

	// Material
	auto tex_checker = make_shared<::tex_checker>(0.32, color(0,0,0), color(1,1,1));
	auto mat_ground = make_shared<mat_diffuse>(tex_checker);
	world.add(make_shared<geo_disk>(point3(0,0,0), vec3(200, 0, 0), vec3(0,0,-200), mat_ground));

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

					mat_sphere = make_shared<mat_diffuse>(albedo);
					world.add(make_shared<geo_sphere>(center, center_end, 0.2, mat_sphere));
				}
				else if (choose_mat < 0.95)
				{
					// metallic
					auto albedo = color::random(.5, 1);
					auto roughness = rand_double(0, .5);
					mat_sphere = make_shared<mat_metallic>(albedo, roughness);
					world.add(make_shared<geo_sphere>(center, 0.2, mat_sphere));
				}
				else
				{
					// transmission
					mat_sphere = make_shared<mat_translucent>(1.5);
					world.add(make_shared<geo_sphere>(center, .2, mat_sphere));
				}
			}
		}
	}

	auto mat_a = make_shared<mat_translucent>(1.5);
	auto mat_b = make_shared<mat_diffuse>(color(.4,.2,.1));
	auto mat_c = make_shared<mat_metallic>(color(.7,.6,.5), 0);

	world.add(make_shared<geo_sphere>(point3(0,1,0), 1, mat_a));
	world.add(make_shared<geo_sphere>(point3(-4,1,0), 1, mat_b));
	world.add(make_shared<geo_sphere>(point3(4,1,0), 1, mat_c));

	world = hittable_list(make_shared<bvh_node>(world));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	//cam.output = std::ofstream("image.ppm");

	cam.vfov = 20;
	cam.position = point3(13,2,3);
	cam.lookat = point3(0,0,0);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = .6;
	cam.focus_distance = 10.0;

	cam.sample_count = 10;
	cam.max_bounces = 24;

	std::vector<unsigned char> output;
	cam.ready();
	cam.render(world, output);
}

void scn_checked_spheres()
{
	hittable_list world;
	auto checker_texture = make_shared<tex_checker>(.03, color(.2, .3, .1), color(.9, .9, .9));

	world.add(make_shared<geo_sphere>(point3(0, -10, 0), 10, make_shared<mat_diffuse>(checker_texture)));
	world.add(make_shared<geo_sphere>(point3(0, 10, 0), 10, make_shared<mat_diffuse>(checker_texture)));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;

	cam.sample_count = 100;
	cam.max_bounces = 50;

	//cam.output = std::ofstream("image.ppm");

	cam.vfov = 20;
	cam.position = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;
	cam.focus_distance = 10;

	std::vector<unsigned char> output;
	cam.ready();
	cam.render(world, output);
}

void scn_earth()
{
	auto earth_texture = make_shared<tex_image>("earthmap.jpg");
	// auto earth_texture = make_shared<tex_uv_debug>();
	auto earth_material = make_shared<mat_diffuse>(earth_texture);
	auto globe = make_shared<geo_sphere>(point3(0,0,0), 2, earth_material);

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

	//cam.output = std::ofstream("image.ppm");

	std::vector<unsigned char> output;
	cam.ready();
	cam.render(hittable_list(globe), output);
}

void scn_perlin()
{
	hittable_list world;

	auto noise_tex = make_shared<tex_perlin>(4, 7);
	auto noise_mat = make_shared<mat_diffuse>(noise_tex);
	world.add(make_shared<geo_sphere>(point3(0,2,0), 2, noise_mat));
	world.add(make_shared<geo_sphere>(point3(0,-1000,0), 1000, noise_mat));

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

	//cam.output = std::ofstream("image.ppm");



	std::vector<unsigned char> output;
	cam.ready();
	cam.render(bvh_node(world), output);
	// cam.render(world);
}

void scn_quads()
{
	hittable_list world;

	// mats
	auto left_red     = make_shared<mat_diffuse>(color(1.0, 0.2, 0.2));
	auto back_green   = make_shared<mat_diffuse>(color(0.2, 1.0, 0.2));
	auto right_blue   = make_shared<mat_diffuse>(color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<mat_diffuse>(color(1.0, 0.5, 0.0));
	auto lower_teal   = make_shared<mat_diffuse>(color(0.2, 0.8, 0.8));

	// geo
	world.add(make_shared<geo_disk>(point3(-3,-2,5), vec3(0,0,-4), vec3(0,4,0), left_red));
	world.add(make_shared<geo_quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
	world.add(make_shared<geo_quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
	world.add(make_shared<geo_quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
	world.add(make_shared<geo_quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

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

	//cam.output = std::ofstream("image.ppm");

	std::vector<unsigned char> output;
	cam.ready();
	cam.render(hittable_list(make_shared<bvh_node>(world)), output);
}

void scn_simple_light()
{
	hittable_list world;

	auto marble = make_shared<tex_perlin>(4,10);
	world.add(make_shared<geo_sphere>(point3(0,2,0), 2, make_shared<mat_diffuse>(marble)));
	world.add(make_shared<geo_disk>(point3(-100,0,100), vec3(200,0,0), vec3(0,0,-200), make_shared<mat_diffuse>(marble)));

	auto mat_light = make_shared<mat_emissive>(marble, color(4,4,4));
	world.add(make_shared<geo_quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), mat_light));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.sample_count = 1000;
	cam.max_bounces = 50;
	cam.background = color::zero;

	cam.vfov = 20;
	cam.position = point3(26,3,6);
	cam.lookat = point3(0,2,0);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = 0;

	// //cam.output = std::ofstream("image.ppm");

	std::vector<unsigned char> output;
	cam.ready();
	cam.render(bvh_node(world), output);
}

void scn_cornell_box(scene& scn)
{
	hittable_list world;

	auto mat_red   = make_shared<mat_diffuse>(color(.65,.05,.05));
	auto mat_white = make_shared<mat_diffuse>(color::one * .73);
	auto mat_green = make_shared<mat_diffuse>(color(.12,.45,.15));
	auto mat_emission = make_shared<mat_emissive>(color::one * 7);

	world.add(make_shared<geo_quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), mat_green));
	world.add(make_shared<geo_quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), mat_red));
	world.add(make_shared<geo_quad>(point3(343,554,332), vec3(-130,0,0),vec3(0,0,-105), mat_emission));
	world.add(make_shared<geo_quad>(point3(0,0,0),vec3(555,0,0),vec3(0,0,555), mat_white));
	world.add(make_shared<geo_quad>(point3(555,555,555),vec3(-555,0,0),vec3(0,0,-555), mat_white));
	world.add(make_shared<geo_quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), mat_white));

	shared_ptr<hittable> cube1 = geo_cube(point3(0, 0, 0), point3(165, 330, 165), mat_white);
	shared_ptr<hittable> cube2 = geo_cube(point3(0, 0, 0), point3(165, 165, 165), mat_white);


	cube1 = make_shared<trn_rotate_x>(cube1, 60);
	cube1 = make_shared<trn_rotate_y>(cube1, 15);
	cube1 = make_shared<trn_rotate_z>(cube1, 65);

	cube2 = make_shared<trn_rotate_y>(cube2, -18);

	cube1 = make_shared<trn_move>(cube1, vec3(265, 50, 295));
	cube2 = make_shared<trn_move>(cube2, vec3(130, 0, 65));

	cube2 = make_shared<volume_convex>(cube2, .008, make_shared<mat_volumetric>(color::zero));
	world.add(cube1);
	world.add(cube2);

	camera cam;

	cam.background = color::zero;

	cam.sample_count = 1;
	cam.max_bounces = 50;

	cam.vfov = 40;
	cam.position = point3(278,278,-800);
	cam.lookat = point3(278,278,0);
	cam.vup = vec3(0,1,0);
	cam.defocus_angle = 0;

	// //cam.output = std::ofstream("image.ppm");

	// cam.render(world);
	scn.camera = cam;
	scn.world = world;
}

// int main(int argc, char** argv)
// {
// 	// scn_lots_of_spheres();
// 	// scn_checked_spheres();
// 	// scn_earth();
// 	// scn_perlin();
// 	// scn_quads();
// 	// scn_simple_light();
// 	scn_cornell_box();
//
// }