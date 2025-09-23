#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// #include "stb_img_include.h"
#include "user_interface.h"

#include "im-file-dialog/ImFileDialog.h"

#include "misc.h"

#include "scene.h"
#include "scene_presets.h"
#include "transformers.h"
#include "viewport.h"
#include "volume_convex.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/geometry/geo_disk.h"
#include "primitives/geometry/geo_quad.h"
#include "primitives/geometry/geo_sphere.h"
#include "primitives/materials/mat_debug.h"
#include "primitives/materials/mat_diffuse.h"
#include "primitives/materials/mat_emissive.h"
#include "primitives/materials/mat_metallic.h"
#include "primitives/materials/mat_translucent.h"
#include "primitives/textures/tex_checker.h"
#include "primitives/textures/tex_image.h"
#include <Windows.h>
void error_callback(int error, const char* description)
{
	std::cerr<<"Error: " << description << '\n';
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	// 	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char* argv[])
{
	// hide console
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

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
	io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

	// imgui: setup backend
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// ImFileDialog
	// ImFileDialog requires you to set the CreateTexture and DeleteTexture
	ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return (void*)tex;
	};
	ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
		GLuint texID = (GLuint)((uintptr_t)tex);
		glDeleteTextures(1, &texID);
	};

	// Init ui
	user_interface ui;

	// Init viewport
	// TODO: do proper thingy
	viewport vp(preset_scene_creator::create_scene(Cornell), 300, 300, 8);

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
