#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void error_callback(int error, const char* description)
{
	std::cerr<<"Error: " << description << '\n';
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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

	GLFWwindow* window = glfwCreateWindow(1400, 800, "Pathfinder", NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create Window!" << '\n';
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

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


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		// Window loop
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		// const float ratio = width / (float) height;

		// std::clog << "w: " << width << " h: " << height << '\n';

		glViewport(0, 0, width, height);

		// double time = glfwGetTime();

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