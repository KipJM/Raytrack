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
			break;
		case Cornell:
			scn_cornell(new_scene);
			break;
		case Chrome:
			break;
		case Spheres:
			break;
		case Dark:
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
		scn.camera.position = point3(0,0,5);
		scn.camera.lookat = point3(0, 0, 0);
		scn.camera.vup = vec3(0, 1, 0);
		scn.camera.defocus_angle = 0;
		scn.camera.focus_distance = 20;
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
};

inline scene_preset preset_scene_creator::selection = Empty;
#endif //RAYTRACINGWEEKEND_SCENE_PRESETS_H