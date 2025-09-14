#ifndef RAYTRACINGWEEKEND_USER_INTERFACE_H
#define RAYTRACINGWEEKEND_USER_INTERFACE_H
#include "imgui_internal.h"
#include "viewport.h"

class user_interface
{
public:
	bool show_help;
	bool show_viewport;
	bool show_render;
	bool show_camera;
	bool show_scene;
	bool show_geometry;
	bool show_material;
	bool show_texture;

	user_interface() : show_help(true), show_viewport(true), show_scene(false)
	{
		SetupImGuiStyle(ImGui::GetIO());
	}

	void render(viewport& viewport)
	{
		ImGui::DockSpaceOverViewport();
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Help", "", &show_help);
				ImGui::Separator();
				ImGui::MenuItem("Viewport", "", &show_viewport);
				ImGui::MenuItem("Render Settings", "", &show_render);
				ImGui::Separator();
				ImGui::MenuItem("Camera Settings", "", &show_camera);
				ImGui::MenuItem("Scene", "", &show_scene);
				ImGui::Separator();
				ImGui::MenuItem("Geometries", "", &show_geometry);
				ImGui::MenuItem("Materials", "", &show_material);
				ImGui::MenuItem("Textures", "", &show_texture);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (show_help) w_help(&show_help);
		if (show_viewport) w_viewport(&show_viewport, viewport);
		if (show_render) w_renderSettings(&show_render, viewport);
	}

	void w_help(bool *p_open)
	{
		if (!ImGui::Begin("Help", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}

		ImGui::SeparatorText("Welcome to Raytrack!");
		ImGui::TextColored(ImVec4(1,0,0,1), "This is very important! Please read!");
		ImGui::BulletText("This is a CPU raytracer, so rendering is sometimes slow based on your machine. Please be patient.");
		ImGui::BulletText("This engine works a bit differently than conventional editors. Highly recommend reading the content below.");
		ImGui::BulletText("Goto the top left and click on the \"windows\" menu to open the necessary editor windows to use Raytrack.");

		if (ImGui::CollapsingHeader("Introduction"))
		{
			ImGui::BulletText("Your render will show up on the Viewport.");
			ImGui::BulletText("The cornell box scene is autoloaded when you open Raytrack.");
			ImGui::BulletText("Adjust global rendering and performance settings with Rendering Settings.");
			ImGui::BulletText("Adjust scene-specific camera settings with Camera Settings.");
			ImGui::BulletText("Change the scene contents with the Scene, Geometries, Materials, and Textures windows.");
			ImGui::BulletText("You should see tooltips besides most fields. Read them to understand what a setting does.");
			ImGui::BulletText("Saving a scene is not currently implemented. Apologies.");
		}

		ImGui::End();
	}

	void w_viewport(bool* p_open, viewport& viewport)
	{
		if (!ImGui::Begin("Viewport", p_open))
		{
			ImGui::End();
			return;
		}
		ImGui::Text("Resolution: %d x %d", viewport.get_width(), viewport.get_height());
		ImGui::Text("Samples: %d", viewport.get_current_sample_count());

		// Auto scaling image
		auto available_space = ImGui::GetContentRegionAvail();
		auto ratio = std::min(available_space.x / viewport.get_width(), available_space.y / viewport.get_height());
		ratio = std::min(ratio, 8.0f); // max ratio
		auto scaled_size = ImVec2(viewport.get_width() * ratio, viewport.get_height() * ratio);

		ImGui::SetCursorPosX((available_space.x - scaled_size.x) * 0.5f);

		ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(viewport.get_texture_id())), scaled_size);

		ImGui::End();
	}

	void w_renderSettings(bool* p_open, viewport& viewport)
	{
		if (!ImGui::Begin("Render Settings", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}

		int a = 1;
		double b = 0.001;
		float c;
		ImGui::SeparatorText("Rendering");

		ImGui::DragInt("Max ray bounces", &a);
		ImGui::SetItemTooltip("Maximum number of bounces a ray can have before getting terminated. Higher = better quality & slower rendering");

		ImGui::InputDouble("bias", &b);
		ImGui::SetItemTooltip("A small number. Fixes rendering issues. Do not touch this if you don't know what you're doing!");

		ImGui::SeparatorText("Performance");

		ImGui::InputInt("Render threads", &a);
		ImGui::SetItemTooltip("For multithreaded rendering. How many worker threads to render your image at the same time. A reasonable number will make rendering faster by utilizing your entire system, but setting this too high will reduce performance.");

		ImGui::DragInt("Samples per worker", &a);
		ImGui::SetItemTooltip("How many samples for each pixel should a worker take for each iteration. A low number (like 1) will make rendering more responsive, but a higher number will give you higher quality results faster.");

		ImGui::DragInt("Target samples count", &a);
		ImGui::SetItemTooltip("After rendering a while, workers will prefer rendering pixels with less than this amount of samples based on the fill probability.");

		ImGui::DragFloat("Render probability", &c);

		ImGui::DragFloat("Fill probability", &c);


		ImGui::End();
	}

private:
	void SetupImGuiStyle(const ImGuiIO& io)
{
	// Setup font
	io.Fonts->AddFontFromFileTTF("font_inter.ttf", 16);
	// io.Fonts->GetTexDataAsRGBA32();

	// Windark style by DestroyerDarkNess from ImThemes (MIT License)
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 8.399999618530273f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 3.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 3.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 3.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 5.599999904632568f;
	style.ScrollbarRounding = 18.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 3.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.4705882370471954f, 0.843137264251709f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.4705882370471954f, 0.843137264251709f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.3294117748737335f, 0.6000000238418579f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.168627455830574f, 0.168627455830574f, 0.168627455830574f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.4705882370471954f, 0.843137264251709f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.0f, 0.3294117748737335f, 0.6000000238418579f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.4705882370471954f, 0.843137264251709f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.0f, 0.3294117748737335f, 0.6000000238418579f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.4705882370471954f, 0.843137264251709f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
};

#endif //RAYTRACINGWEEKEND_USER_INTERFACE_H