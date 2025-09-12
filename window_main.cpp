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

void RenderInterface(viewport& viewport)
{
	ImGui::DockSpaceOverViewport();
	static bool v_enabled = true;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Viewport", "", &v_enabled);
			ImGui::MenuItem("Camera Settings", "", &v_enabled);
			ImGui::MenuItem("Scene Hierarchy", "", &v_enabled);
			ImGui::MenuItem("Inspector", "", &v_enabled);
			ImGui::MenuItem("Geometries", "", &v_enabled);
			ImGui::MenuItem("Textures", "", &v_enabled);
			ImGui::MenuItem("Materials", "", &v_enabled);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (ImGui::Begin("Viewport"))
	{
		ImGui::Text("Resolution: %d x %d", viewport.get_width(), viewport.get_height());
		ImGui::Text("Samples: %d", viewport.get_current_sample_count());

		// Auto scaling image
		auto available_space = ImGui::GetContentRegionAvail();
		auto ratio = std::min(available_space.x / viewport.get_width(), available_space.y / viewport.get_height());
		ratio = std::min(ratio, 4.0f); // max ratio
		auto scaled_size = ImVec2(viewport.get_width() * ratio, viewport.get_height() * ratio);

		ImGui::SetCursorPosX((available_space.x - scaled_size.x) * 0.5f);

		ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(viewport.get_texture_id())),scaled_size);
	}

	ImGui::End();
}


void scn_cornell_box(scene& scn)
{
	scn.world = hittable_list();

	auto mat_red   = make_shared<mat_diffuse>(color(.65,.05,.05));
	auto mat_white = make_shared<mat_diffuse>(color::one * .73);
	auto mat_green = make_shared<mat_diffuse>(color(.12,.45,.15));
	auto mat_emission = make_shared<mat_emissive>(color::one * 7);

	scn.world.add(make_shared<geo_quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), mat_green));
	scn.world.add(make_shared<geo_quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), mat_red));
	scn.world.add(make_shared<geo_quad>(point3(343,554,332), vec3(-130,0,0),vec3(0,0,-105), mat_emission));
	scn.world.add(make_shared<geo_quad>(point3(0,0,0),vec3(555,0,0),vec3(0,0,555), mat_white));
	scn.world.add(make_shared<geo_quad>(point3(555,555,555),vec3(-555,0,0),vec3(0,0,-555), mat_white));
	scn.world.add(make_shared<geo_quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), mat_white));

	shared_ptr<hittable> cube1 = geo_cube(point3(0, 0, 0), point3(165, 330, 165), mat_white);
	shared_ptr<hittable> cube2 = geo_cube(point3(0, 0, 0), point3(165, 165, 165), mat_white);


	cube1 = make_shared<trn_rotate_x>(cube1, 60);
	cube1 = make_shared<trn_rotate_y>(cube1, 15);
	cube1 = make_shared<trn_rotate_z>(cube1, 65);

	cube2 = make_shared<trn_rotate_y>(cube2, -18);

	cube1 = make_shared<trn_move>(cube1, vec3(265, 50, 295));
	cube2 = make_shared<trn_move>(cube2, vec3(130, 0, 65));

	cube2 = make_shared<volume_convex>(cube2, .008, make_shared<mat_volumetric>(color::zero));
	scn.world.add(cube1);
	scn.world.add(cube2);

	scn.camera = camera();

	scn.camera.background = color::zero;

	scn.camera.sample_count = 1;
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
		RenderInterface(vp);

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
