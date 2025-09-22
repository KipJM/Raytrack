#ifndef RAYTRACINGWEEKEND_SCENE_PRESETS_H
#define RAYTRACINGWEEKEND_SCENE_PRESETS_H
#include "scene.h"
#include "transformers.h"
#include "viewport.h"
#include "volume_convex.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/geometry/geo_disk.h"
#include "primitives/geometry/geo_quad.h"
#include "primitives/geometry/geo_sphere.h"
#include "primitives/materials/mat_diffuse.h"
#include "primitives/materials/mat_emissive.h"
#include "primitives/materials/mat_metallic.h"
#include "primitives/materials/mat_translucent.h"
#include "primitives/materials/mat_volumetric.h"
#include "primitives/textures/tex_checker.h"
#include "primitives/textures/tex_perlin.h"

enum scene_preset
{
	Empty,
	Sky,
	Cornell,
	Chrome,
	Spheres,
	Dark
};

class preset_scene_creator
{
public:
	static scene_preset selection;

	static scene& update_scene(viewport& viewport, scene_preset preset)
	{
		viewport.mark_scene_dirty(); // Stop all workers

		viewport.target_scene = create_scene(preset);

		viewport.init_new_camera();
		viewport.target_scene.mark_dirty(); // Restart workers in next frame
		viewport.set_resolution(viewport.get_width(), viewport.get_height());
		return viewport.target_scene;
	}

	static scene create_scene(scene_preset preset)
	{
		scene new_scene = scene();
		new_scene.world = hittable_list();
		new_scene.camera = camera(); // Reset all params, redundant but put here in case some fuck ups

		switch (preset)
		{
		case Empty:
			scn_empty(new_scene);
			break;
		case Sky:
			scn_sky(new_scene);
			break;
		case Cornell:
			scn_cornell(new_scene);
			break;
		case Chrome:
			scn_chrome(new_scene);
			break;
		case Spheres:
			scn_spheres(new_scene);
			break;
		case Dark:
			scn_dark(new_scene);
			break;
		}

		return new_scene;
	}

private:
	static void scn_empty(scene& scn)
	{
		scn.camera.background = color::one * 0.5;

		scn.camera.image_width = 600;
		scn.camera.image_height = 400;

		scn.camera.vfov = 75;
		scn.camera.position = point3(0, 0, 5);
		scn.camera.lookat = point3(0, 0, 0);
		scn.camera.vup = vec3(0, 1, 0);
		scn.camera.defocus_angle = 0;
		scn.camera.focus_distance = 20;
	}

	static void scn_sky(scene& scn)
	{
		scn.camera.background = color(.53, .81, .92);

		scn.camera.image_width = 600;
		scn.camera.image_height = 400;

		scn.camera.vfov = 75;
		scn.camera.position = point3(0, 2, 5);
		scn.camera.lookat = point3(0, 2, 0);
		scn.camera.vup = vec3(0, 1, 0);
		scn.camera.defocus_angle = 0;
		scn.camera.focus_distance = 20;

		auto tex_green = make_shared<tex_color>(color(.62, .78, .27));
		tex_green->name = "Green";
		auto mat_grass = make_shared<mat_diffuse>(tex_green);
		mat_grass->name = "Grass material";

		auto geo_ground = make_shared<geo_disk>(point3(-10, 0, 10), vec3(20, 0, 0), vec3(0, 0, -20), mat_grass);
		geo_ground->name = "Ground";

		scn.textures.push_back(tex_green);
		scn.materials.push_back(mat_grass);
		scn.objects.push_back(geo_ground);
		scn.world.add(geo_ground);
	}

	static void scn_cornell(scene& scn)
	{
		auto tex_red = make_shared<tex_color>(1, 0, 0);
		tex_red->name = "Red";
		auto tex_white = make_shared<tex_color>(.8, .8, .75);
		tex_white->name = "Off white";
		auto tex_green = make_shared<tex_color>(.12, .45, .15);
		tex_green->name = "Green";
		auto tex_black = make_shared<tex_color>(0, 0, 0);
		tex_black->name = "black";

		scn.textures.push_back(tex_red);
		scn.textures.push_back(tex_white);
		scn.textures.push_back(tex_green);
		scn.textures.push_back(tex_black);

		auto mat_red = make_shared<mat_diffuse>(tex_red);
		mat_red->name = "Red rough";
		auto mat_red_glass = make_shared<mat_translucent>(tex_red, 1.5);
		mat_red_glass->name = "Red Glass";
		auto mat_white = make_shared<mat_diffuse>(tex_white);
		mat_white->name = "White rough";
		auto mat_green = make_shared<mat_metallic>(tex_green, .9);
		mat_green->name = "Green rough metal";
		auto mat_emission = make_shared<mat_emissive>(tex_white, color::one * 7);
		mat_emission->name = "White Emissive";
		auto mat_volume = make_shared<mat_volumetric>(tex_black);
		mat_volume->name = "Black Volume";

		scn.materials.push_back(mat_red);
		scn.materials.push_back(mat_red_glass);
		scn.materials.push_back(mat_white);
		scn.materials.push_back(mat_green);
		scn.materials.push_back(mat_emission);
		scn.materials.push_back(mat_volume);

		auto wall_a = make_shared<geo_quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), mat_green);
		auto wall_b = make_shared<geo_quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), mat_red);
		auto wall_c = make_shared<geo_disk>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), mat_emission);
		auto wall_d = make_shared<geo_quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), mat_white);
		auto wall_e = make_shared<geo_quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), mat_white);
		auto wall_f = make_shared<geo_quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), mat_white);

		wall_a->name = "Green Wall";
		wall_b->name = "Red Wall";
		wall_c->name = "Light";
		wall_d->name = "White wall A";
		wall_e->name = "White Wall B";
		wall_f->name = "White Wall C";

		shared_ptr<hittable> sphere = make_shared<geo_sphere>(point3(384, 80, 140), 80, mat_red_glass);
		sphere->name = "Glass sphere";


		shared_ptr<hittable> cube1 = make_shared<geo_cube>(point3(0, 0, 0), point3(165, 330, 165), mat_white);
		cube1->name = "Cube A";

		shared_ptr<hittable> rot_cube1 = make_shared<trn_rotate>(cube1, vec3(60, 15, 65));
		shared_ptr<hittable> m_cube1 = make_shared<trn_move>(rot_cube1, vec3(265, 50, 295));


		shared_ptr<hittable> cube2 = make_shared<geo_cube>(point3(0, 0, 0), point3(165, 165, 165), mat_white);
		cube2->name = "Cube B";

		shared_ptr<hittable> rot_cube2 = make_shared<trn_rotate>(cube2, vec3(0, -18, 0));
		shared_ptr<hittable> m_cube2 = make_shared<trn_move>(rot_cube2, vec3(130, 0, 65));
		shared_ptr<hittable> v_cube2 = make_shared<volume_convex>(m_cube2, .008, mat_volume);


		shared_ptr<hittable_list> list = make_shared<hittable_list>();
		list->name = "Room";
		list->add(wall_a);
		list->add(wall_b);
		list->add(wall_c);
		list->add(wall_d);
		list->add(wall_e);
		list->add(wall_f);

		scn.objects.push_back(wall_a);
		scn.objects.push_back(wall_b);
		scn.objects.push_back(wall_c);
		scn.objects.push_back(wall_d);
		scn.objects.push_back(wall_e);
		scn.objects.push_back(wall_f);
		scn.objects.push_back(sphere);
		scn.objects.push_back(cube1);
		scn.objects.push_back(cube2);
		scn.objects.push_back(rot_cube1);
		scn.objects.push_back(rot_cube2);
		scn.objects.push_back(m_cube1);
		scn.objects.push_back(m_cube2);
		scn.objects.push_back(v_cube2);
		scn.objects.push_back(list);

		scn.world.add(list);
		scn.world.add(m_cube1);
		scn.world.add(v_cube2);
		scn.world.add(sphere);

		scn.camera.background = color::zero;

		scn.camera.image_width = 300;
		scn.camera.image_height = 300;

		scn.camera.vfov = 40;
		scn.camera.position = point3(278, 278, -800);
		scn.camera.lookat = point3(278, 278, 0);
		scn.camera.vup = vec3(0, 1, 0);
		scn.camera.defocus_angle = 0;
	}

	static void scn_chrome(scene& scn)
	{
		auto tex_red = make_shared<tex_color>("red", color(1, 0.127, 0.127));
		auto tex_blue = make_shared<tex_color>("blue", color(.405, 0, 1));
		auto tex_white = make_shared<tex_color>("white", color::one * 0.6);

		auto tex_check = make_shared<tex_checker>(.05, tex_red, tex_blue);
		tex_check->name = "checker";

		auto mat_ground = make_shared<mat_diffuse>("Ground", tex_check);
		auto mat_sphere = make_shared<mat_metallic>(tex_white, 0.05);
		mat_sphere->name = "Metal";

		auto geo_ground = make_shared<geo_quad>(point3(-10, 0, 10), vec3(20, 0, 0), vec3(0, 0, -20), mat_ground);
		geo_ground->name = "Ground";

		auto geo_sph = make_shared<geo_sphere>(point3(0, 1, 0), 1, mat_sphere);
		geo_sph->name = "Ball";

		scn.textures.push_back(tex_red);
		scn.textures.push_back(tex_white);
		scn.textures.push_back(tex_check);
		scn.textures.push_back(tex_blue);

		scn.materials.push_back(mat_ground);
		scn.materials.push_back(mat_sphere);

		scn.objects.push_back(geo_ground);
		scn.objects.push_back(geo_sph);

		scn.world.add(geo_ground);
		scn.world.add(geo_sph);

		scn.camera.background = color(.592, .624, .658);

		scn.camera.image_width = 600;
		scn.camera.image_height = 400;

		scn.camera.vfov = 75;
		scn.camera.position = point3(0, 1, 3);
		scn.camera.lookat = point3(0, 0, 0);
		scn.camera.vup = vec3(0, 1, 0);
		scn.camera.defocus_angle = 0;
		scn.camera.focus_distance = 20;
	}

	static void scn_spheres(scene& scn)
	{
		// Material
		auto tex_white = make_shared<tex_color>("white", color::one);
		auto tex_black = make_shared<tex_color>("black", color::zero);
		scn.textures.push_back(tex_white);
		scn.textures.push_back(tex_black);

		auto tex_check = make_shared<tex_checker>(0.1, tex_white, tex_black); tex_check->name = "checker";
		auto mat_ground = make_shared<mat_diffuse>("Ground mat", tex_check);
		scn.textures.push_back(tex_check);
		scn.materials.push_back(mat_ground);

		auto gr = make_shared<geo_disk>(point3(-25, 0, 25), vec3(50, 0, 0), vec3(0, 0, -50), mat_ground);
		gr->name = "Ground";
		scn.objects.push_back(gr);
		scn.world.add(gr);

		auto list = make_shared<hittable_list>("Spheres");
		scn.objects.push_back(list);

		auto mat_glass = make_shared<mat_translucent>(tex_white, 1.5);
		mat_glass->name = "Glass";
		scn.materials.push_back(mat_glass);

		int i = 1;
		for (int a = -11; a < 11; a++)
		{
			for (int b = -11; b < 11; b++)
			{
				i++;
				auto choose_mat = rand_double();
				point3 center(a + 0.9 * rand_double(), 0.2, b + 0.9 * rand_double());

				if ((center - point3(4, 0.2, 0)).length() > 0.9)
				{
					if (choose_mat < 0.8)
					{
						// diffuse
						auto albedo = color::random() * color::random();

						auto tex_sphere = make_shared<tex_color>("proc. tex" + std::to_string(i), albedo);
						auto mat_sphere = make_shared<mat_diffuse>("proc. mat " + std::to_string(i), tex_sphere);
						scn.textures.push_back(tex_sphere);
						scn.materials.push_back(mat_sphere);

						auto sph = make_shared<geo_sphere>(center, 0.2, mat_sphere);
						sph->name = "proc. sphere" + std::to_string(i);
						scn.objects.push_back(sph);
						list->add(sph);
					}
					else if (choose_mat < 0.95)
					{
						// metallic
						auto albedo = color::random(.5, 1);
						auto roughness = rand_double(0, .5);
						auto tex_sphere = make_shared<tex_color>("proc. tex" + std::to_string(i), albedo);
						auto mat_sphere = make_shared<mat_metallic>(tex_sphere, roughness);
						mat_sphere->name = "proc. mat " + std::to_string(i);
						scn.textures.push_back(tex_sphere);
						scn.materials.push_back(mat_sphere);

						auto sph = make_shared<geo_sphere>(center, 0.2, mat_sphere);
						sph->name = "proc. sphere" + std::to_string(i);
						scn.objects.push_back(sph);
						list->add(sph);
					}
					else
					{
						// transmission
						auto sph = make_shared<geo_sphere>(center, 0.2, mat_glass);
						sph->name = "proc. sphere" + std::to_string(i);
						scn.objects.push_back(sph);
						list->add(sph);
					}
				}
			}
		}
		scn.world.add(list);

		auto tex_b = make_shared<tex_color>("tex b", color(.4, .2, .1));
		auto tex_c = make_shared<tex_color>("tex c", color(.7, .6, .5));
		auto mat_b = make_shared<mat_diffuse>("mat b", tex_b);
		auto mat_c = make_shared<mat_metallic>(tex_c, 0); mat_c->name = "mat c";
		scn.textures.push_back(tex_b);
		scn.textures.push_back(tex_c);
		scn.materials.push_back(mat_b);
		scn.materials.push_back(mat_c);

		auto sphere1 = make_shared<geo_sphere>(point3(0, 1, 0), 1, mat_glass); sphere1->name = "Sphere A";
		auto sphere2 = make_shared<geo_sphere>(point3(-4, 1, 0), 1, mat_b); sphere2->name = "Sphere B";
		auto sphere3 = make_shared<geo_sphere>(point3(4, 1, 0), 1, mat_c); sphere3->name = "Sphere C";

		scn.objects.push_back(sphere1);
		scn.objects.push_back(sphere2);
		scn.objects.push_back(sphere3);

		scn.world.add(sphere1);
		scn.world.add(sphere2);
		scn.world.add(sphere3);

		scn.camera.image_width = 400;
		scn.camera.image_height = 225;

		scn.camera.vfov = 20;
		scn.camera.position = point3(13, 2, 3);
		scn.camera.lookat = point3(0, 0, 0);
		scn.camera.vup = vec3(0, 1, 0);

		scn.camera.defocus_angle = .6;
		scn.camera.focus_distance = 10.0;

	}

	static void scn_dark(scene& scn)
	{
		auto tex_white = make_shared<tex_color>("white", color::one);
		auto tex_marble = make_shared<tex_perlin>(tex_white, 4, 10);
		tex_marble->name = "Marble texture";
		auto mat_marble = make_shared<mat_diffuse>("marble rough", tex_marble);
		auto mat_glow = make_shared<mat_emissive>(tex_marble, color::one * 4);
		mat_glow->name = "Glow material";

		auto geo_sph = make_shared<geo_sphere>(point3(0, 2, 0), 2, mat_marble);
		geo_sph->name = "Sphere";
		auto geo_dis = make_shared<geo_disk>(point3(-100, 0, 100), vec3(200, 0, 0), vec3(0, 0, -200), mat_marble);
		geo_dis->name = "Ground";

		auto geo_light = make_shared<geo_quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), mat_glow);
		geo_light->name = "Light source";

		scn.textures.push_back(tex_white);
		scn.textures.push_back(tex_marble);
		scn.materials.push_back(mat_marble);
		scn.materials.push_back(mat_glow);
		scn.objects.push_back(geo_sph);
		scn.objects.push_back(geo_dis);
		scn.objects.push_back(geo_light);

		scn.world.add(geo_sph);
		scn.world.add(geo_dis);
		scn.world.add(geo_light);

		scn.camera.image_width = 400;
		scn.camera.image_height = 225;
		scn.camera.background = color::zero;

		scn.camera.vfov = 20;
		scn.camera.position = point3(26, 3, 6);
		scn.camera.lookat = point3(0, 2, 0);
		scn.camera.vup = vec3(0, 1, 0);

		scn.camera.defocus_angle = 0;
	}
};

inline scene_preset preset_scene_creator::selection = Empty;
#endif //RAYTRACINGWEEKEND_SCENE_PRESETS_H
