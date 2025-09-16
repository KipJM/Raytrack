#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "misc.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "scene.h"
#include "transformers.h"
#include "user_interface.h"
#include "viewport.h"
#include "volume_convex.h"
#include "external/stb_image.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/geometry/geo_quad.h"
#include "primitives/materials/mat_diffuse.h"
#include "primitives/materials/mat_emissive.h"

void error_callback(int error, const char* description)
{
	std::cerr<<"Error: " << description << '\n';
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	// 	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void scn_cornell_box(scene& scn)
{
	scn.world = hittable_list();

	auto mat_red   = make_shared<mat_diffuse>(color(.65,.05,.05));	mat_red->name = "Red Diffuse";
	auto mat_white = make_shared<mat_diffuse>(color::one * .73);		mat_white->name = "White Diffuse";
	auto mat_green = make_shared<mat_diffuse>(color(.12,.45,.15));	mat_green->name = "Green Diffuse";
	auto mat_emission = make_shared<mat_emissive>(color::one * 7);	mat_emission->name = "White Emissive";

	auto wall_a = make_shared<geo_quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), mat_green);
	auto wall_b = make_shared<geo_quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), mat_red);
	auto wall_c = make_shared<geo_quad>(point3(343,554,332), vec3(-130,0,0),vec3(0,0,-105), mat_emission);
	auto wall_d = make_shared<geo_quad>(point3(0,0,0),vec3(555,0,0),vec3(0,0,555), mat_white);
	auto wall_e = make_shared<geo_quad>(point3(555,555,555),vec3(-555,0,0),vec3(0,0,-555), mat_white);
	auto wall_f = make_shared<geo_quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), mat_white);

	wall_a->name = "Green Wall";
	wall_b->name = "Red Wall";
	wall_c->name = "Light";
	wall_d->name = "White wall A";
	wall_e->name = "White Wall B";
	wall_f->name = "White Wall C";

	shared_ptr<hittable> cube1 = make_shared<geo_cube>(point3(0, 0, 0), point3(165, 330, 165), mat_white);
	shared_ptr<hittable> cube2 = make_shared<geo_cube>(point3(0, 0, 0), point3(165, 165, 165), mat_white);
	cube1->name = "Cube A";
	cube2->name = "Cube B";

	// shared_ptr<hittable> rx_cube1 = make_shared<trn_rotate_x>(cube1, 60);
	// shared_ptr<hittable> ry_cube1 = make_shared<trn_rotate_y>(cube1, 15);
	// shared_ptr<hittable> rz_cube1 = make_shared<trn_rotate_z>(cube1, 65);
	shared_ptr<hittable> rot_cube1 = make_shared<trn_rotate>(cube1, vec3(60, 15, 65));

	// shared_ptr<hittable> ry_cube2 = make_shared<trn_rotate_y>(cube2, -18);
	shared_ptr<hittable> rot_cube2 = make_shared<trn_rotate>(cube2, vec3(0,-18,0));

	shared_ptr<hittable> m_cube1 = make_shared<trn_move>(rot_cube1, vec3(265, 50, 295));
	shared_ptr<hittable> m_cube2 = make_shared<trn_move>(rot_cube2, vec3(130, 0, 65));

	shared_ptr<hittable> v_cube2 = make_shared<volume_convex>(m_cube2, .008, make_shared<mat_volumetric>(color::zero));

	scn.world.add(m_cube1);
	scn.world.add(v_cube2);

	scn.world.add(wall_a);
	scn.world.add(wall_b);
	scn.world.add(wall_c);
	scn.world.add(wall_d);
	scn.world.add(wall_e);
	scn.world.add(wall_f);

	scn.objects.push_back(cube1);
	scn.objects.push_back(cube2);
	scn.objects.push_back(wall_a);
	scn.objects.push_back(wall_b);
	scn.objects.push_back(wall_c);
	scn.objects.push_back(wall_d);
	scn.objects.push_back(wall_e);
	scn.objects.push_back(wall_f);
	// scn.objects.push_back(rx_cube1);
	// scn.objects.push_back(ry_cube1);
	// scn.objects.push_back(rz_cube1);
	// scn.objects.push_back(ry_cube2);
	scn.objects.push_back(rot_cube1);
	scn.objects.push_back(rot_cube2);
	scn.objects.push_back(m_cube1);
	scn.objects.push_back(m_cube2);
	scn.objects.push_back(v_cube2);

	scn.materials.push_back(mat_green);
	scn.materials.push_back(mat_emission);
	scn.materials.push_back(mat_red);
	scn.materials.push_back(mat_white);

	scn.camera = camera();

	scn.camera.background = color::zero;

	scn.camera.basic_ratio = .1;
	scn.camera.fill_ratio = .5;

	scn.camera.sample_count = 1;
	scn.camera.min_samples = 5;
	scn.camera.max_bounces = 20;

	scn.camera.vfov = 40;
	scn.camera.position = point3(278,278,-800);
	scn.camera.lookat = point3(278,278,0);
	scn.camera.vup = vec3(0,1,0);
	scn.camera.defocus_angle = 0;
}

int main(int argc, char* argv[])
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		// Init failed
		std::cerr << "Failed to initialize GLFW!" << '\n';
		exit(EXIT_FAILURE);
	}

	// Window creation
	GLFWwindow* window = glfwCreateWindow(1400, 800, "Raytrack", NULL, NULL);

	if (!window)
	{
		std::cerr << "Failed to create Window!" << '\n';
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// icon :3
	GLFWimage images[1];
	images[0].pixels = stbi_load("icon.png", &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(1);

	// Setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// imgui: setup backend
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Init ui
	user_interface ui;

	// Init viewport
	// TODO: do proper thingy
	scene scn;
	scn_cornell_box(scn);

	viewport vp(scn, 300, 300, 8);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// glTexImage2D();

		// Window loop
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);


		glViewport(0, 0, width, height);

		vp.update();
		ui.render(vp);
		// ImGui::ShowDemoWindow();

		// Display
		glClear(GL_COLOR_BUFFER_BIT); // or not :3
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Exit
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
