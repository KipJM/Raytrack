#ifndef RAYTRACINGWEEKEND_USER_INTERFACE_H
#define RAYTRACINGWEEKEND_USER_INTERFACE_H

#include <misc/cpp/imgui_stdlib.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "ui_components.h"
#include "viewport.h"

class user_interface
{
public:
	static ImVec4 const color_mesh;
	static ImVec4 const color_mat;

	static float popup_color_storage[3];
	static std::string popup_string_storage;

	user_interface()
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
		if (show_camera) w_cameraSettings(&show_camera, viewport);
		if (show_scene) w_scene(&show_scene, viewport, viewport.target_scene);
		if (show_geometry) w_geometries(&show_geometry, viewport, viewport.target_scene);
		if (show_material) w_materials(&show_material, viewport, viewport.target_scene);
	}

private:

	bool show_help		= true;
	bool show_viewport	= true;
	bool show_render	= true;
	bool show_camera	= true;
	bool show_scene		= true;
	bool show_geometry	= true;
	bool show_material	= true;
	bool show_texture	= true;

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

	ImVec2 prev_resolution;
	void w_viewport(bool* p_open, viewport& viewport)
	{
		if (!ImGui::Begin("Viewport", p_open))
		{
			ImGui::End();
			return;
		}

		ImGui::Text("Resolution: %d x %d", viewport.get_width(), viewport.get_height());
		if (viewport.get_camera().auto_resolution)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1,1,0,1), "(Auto resolution enabled)");
		}
		ImGui::Text("Samples: %d", viewport.get_current_sample_count());

		if (viewport.is_waiting())
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1,0,0,1), "(OUTDATED IMAGE! Waiting for new render!)");
		}

		// Auto resolution
		if (viewport.get_camera().auto_resolution)
		{
			auto current_res = ImGui::GetContentRegionAvail();
			if (current_res.x != prev_resolution.x || current_res.y != prev_resolution.y)
			{
				prev_resolution = current_res;
				viewport.set_resolution(current_res.x, current_res.y);
			}
		}

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
		ImGui::SetItemTooltip("These settings persist across scenes, but gets reset after application closes.");

		ImGui::SeparatorText("Rendering");

		int mb = viewport.get_max_bounces();
		if (ImGui::DragInt("Max ray bounces", &mb)) viewport.set_max_bounces(mb);
		ImGui::SetItemTooltip("Maximum number of bounces a ray can have before getting terminated. Higher = better quality & slower rendering");

		double bi = viewport.get_bias();
		if (ImGui::InputDouble("bias", &bi)) viewport.set_bias(bi);
		ImGui::SetItemTooltip("A small number. Fixes rendering issues. Do not touch this if you don't know what you're doing!");


		ImGui::SeparatorText("Performance");

		int tc = viewport.get_workers_count();
		if (ImGui::InputInt("Render threads", &tc, 1, 10)) viewport.set_worker_count(tc);
		ImGui::SetItemTooltip("For multithreaded rendering. How many worker threads to render your image at the same time. A reasonable number will make rendering faster by utilizing your entire system, but setting this too high will reduce performance.");

		int sc = viewport.get_sample_count();
		if (ImGui::DragInt("Samples per worker", &sc, 1)) viewport.set_sample_count(sc);
		ImGui::SetItemTooltip("How many samples for each pixel should a worker take for each iteration. A low number (like 1) will make rendering more responsive, but a higher number will give you higher quality results faster.");

		int ms = viewport.get_min_samples();
		if (ImGui::DragInt("Target samples count", &ms, 1)) viewport.set_min_samples(ms);
		ImGui::SetItemTooltip("After rendering a while, workers will prefer rendering pixels with less than this amount of samples based on the fill probability.");

		float br = viewport.get_basic_ratio();
		if (ImGui::DragFloat("Render probability", &br, 0.1, 0.01, 1)) viewport.set_basic_ratio(br);
		ImGui::SetItemTooltip("Approximate ratio of pixels a worker will fill each iteration. A small ratio will make rendering more responsive, but slower.");

		float fr = viewport.get_fill_ratio();
		if (ImGui::DragFloat("Fill probability", &fr, 0.1, 0.01, 1)) viewport.set_fill_ratio(fr);
		ImGui::SetItemTooltip("A special ratio to use for pixels with less samples than target after a while. Setting this relatively high will make workers actively fill holes in your render.");


		ImGui::End();
	}

	void w_cameraSettings(bool* p_open, viewport& viewport)
	{
		if (!ImGui::Begin("Camera Settings", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}
		ImGui::SetItemTooltip("Scene-specific camera settings. They change between each scene.");

		camera& cam = viewport.get_camera();

		ImGui::SeparatorText("Resolution");

		ImGui::Checkbox("Scale to window", &cam.auto_resolution);

		ImGui::BeginDisabled(cam.auto_resolution);

		int iw = cam.image_width;
		int ih = cam.image_height;
		int res[] = {iw, ih};
		if (ImGui::DragInt2("Resolution", res)) viewport.set_resolution(res[0], res[1]);
		ImGui::SetItemTooltip("Disabled if auto resolution is enabled. Width x Height. Around 300x300 is recommended for fast renders.");
		// ImGui::Text("Please don't make the resolution too small, some weird bugs with multithreaded rendering may cause a crash");
		ImGui::EndDisabled();

		bool dirty = false;

		ImGui::SeparatorText("Transform");

		dirty += ImGui::DragDouble3("Position", cam.position.e, 0.1);
		ImGui::SetItemTooltip("The world position of the camera sensor.");

		dirty += ImGui::DragDouble3("Focus position", cam.lookat.e, 0.1);
		ImGui::SetItemTooltip("The camera will look at this point in space.");

		dirty += ImGui::DragDouble3("Up direction", cam.vup.e, 0.1);
		ImGui::SetItemTooltip("The Camera's up direction will align with this vector. Does not need to be normalized. (+Y is global up)");

		ImGui::SeparatorText("Focus");

		dirty += ImGui::DragDouble("Focus distance", &cam.focus_distance, 0.1);
		ImGui::SetItemTooltip("The distance from the camera the lens is focusing on");

		dirty += ImGui::DragDouble("Defocus angle", &cam.defocus_angle);
		ImGui::SetItemTooltip("The max angle light rays will differ when coming out of the lens");

		ImGui::SeparatorText("World");

		float* bg = cam.background.get_float();
		if (ImGui::ColorEdit3("Background color", bg))
		{
			cam.background.set_float(bg);
			dirty = true;
		}
		ImGui::SetItemTooltip("The sky color. This will influence the light cast onto objects as well.");

		if (dirty) viewport.mark_dirty();

		ImGui::End();
	}

private:
	int scene_selection = -1;

	void w_scene(bool *p_open, viewport& viewport, scene& scene)
	{
		if (!ImGui::Begin("Scene Hierarchy", p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::Button("Add"))
			ImGui::OpenPopup("Add object to scene");

		ImGui::BeginDisabled(!(scene_selection >= 0 && scene_selection < scene.world.objects.size()));

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
			ImGui::OpenPopup("scn_remove_item");

		ImGui::SameLine(ImGui::GetWindowWidth() - 70);
		if (ImGui::Button("Deselect"))
		{
			scene_selection = -1;
		}

		ImGui::EndDisabled();

		bool a_open = true;
		if (ImGui::BeginPopupModal("Add object to scene", &a_open))
		{
			ImGui::Text("Select an object from your geometries.");

			// ImGui::SameLine();
			// if (ImGui::Button("Cancel"))
			// 	ImGui::CloseCurrentPopup();


			static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
				| ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

			if (ImGui::BeginTable("add_list", 2, table_flags))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Type");
				ImGui::TableHeadersRow();

				int count = 0;
				for (std::shared_ptr<hittable>& object : scene.objects)
				{
					if (std::ranges::any_of(scene.world.objects,
					[&object](const std::shared_ptr<hittable>& sp) {
							return sp.get() == object.get(); // Compare underlying raw pointers
					}))
					{
						// No duplicates
						continue;
					}
					count++;

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Name
					if (ImGui::Selectable(object->name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
					{
						// Add object to scene
						viewport.mark_scene_dirty();
						scene.world.add(std::shared_ptr(object));
						ImGui::CloseCurrentPopup();
					}


					ImGui::TableNextColumn();
					ImGui::TextColored(color_mesh, object->get_human_type().c_str());
				}

				if (scene.objects.empty())
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "No objects! Create some at Geometries!");
				}
				else if (count == 0)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Only one instance of each object can be added to scene.");
				}

				ImGui::EndTable();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("scn_remove_item"))
		{
			ImGui::Text("Remove this object from render? It will stay in geometries.");
			if (ImGui::Button("Confirm"))
			{
				// remove from scene
				if (scene_selection < scene.world.objects.size())
				{
					viewport.mark_scene_dirty();
					scene.world.remove(scene_selection);
				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::Text("You can also drag and drop geometries here to add them to your render.");

		static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
		| ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("world_list", 2, table_flags))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Type");
			ImGui::TableHeadersRow();

			for (int i = 0; i < scene.world.objects.size(); i++)
			{
				std::shared_ptr<hittable>& object = scene.world.objects[i];

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				// Name
				if (ImGui::Selectable(object->name.c_str(), scene_selection == i, ImGuiSelectableFlags_SpanAllColumns))
					scene_selection = i;

				ImGui::TableNextColumn();
				ImGui::TextColored(color_mesh, object->get_human_type().c_str());
			}

			if (scene.world.objects.empty())
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Add or drag objects here to show them in your render.");
			}

			ImGui::EndTable();
		}

		if (ImGui::BeginDragDropTarget())
		{
			// Hovering
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REF_GEO", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip))
			{
				std::shared_ptr<hittable> ref = *static_cast<std::shared_ptr<hittable>*>(payload->Data);

				if (std::ranges::any_of(scene.world.objects,
					[&ref](const std::shared_ptr<hittable>& sp) {
							return sp.get() == ref.get(); // Compare underlying raw pointers
					}))
				{
					// Is duplicate
					ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
					ImGui::SetTooltip("This object already exist in your render.");
				} else
				{
					ImGui::SetTooltip(std::format("Drop to add {0} to your render.", ref->name).c_str());
					// Not Duplicate
					if (payload->IsDelivery()) // Dropped
					{
						viewport.mark_scene_dirty();
						scene.world.add(std::shared_ptr(ref)); // copy
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}

private:
	int geo_selection = -1;
	hittable_type_combo hittable_combo;
	void w_geometries(bool* p_open, viewport& viewport, scene& scene)
	{
		if (!ImGui::Begin("Geometries", p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginChild("##geotree", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
		{
			if (ImGui::Button("Create"))
			{
				hittable_combo.selection = cube; // uhh for some reason can't reinitialize
				ImGui::OpenPopup("Create object");
			}

			ImGui::BeginDisabled(!(geo_selection >= 0 && geo_selection < scene.objects.size()));

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
			if (ImGui::Button("Deselect"))
			{
				geo_selection = -1;
			}

			ImGui::EndDisabled();


			static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
				| ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

			if (ImGui::BeginTable("geo_list", 2, table_flags))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Type");
				ImGui::TableHeadersRow();

				for (int i = 0; i < scene.objects.size(); i++)
				{
					std::shared_ptr<hittable>& object = scene.objects[i];

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Name
					if (ImGui::Selectable(object->name.c_str(), geo_selection == i, ImGuiSelectableFlags_SpanAllColumns))
						geo_selection = i;

					if (ImGui::BeginDragDropSource())
					{
						ImGui::Text(object->name.c_str());
						ImGui::SameLine();
						ImGui::TextColored(color_mesh,
							(" (" + object->get_human_type() + ")").c_str());

						ImGui::SetDragDropPayload("REF_GEO", &object, sizeof(std::shared_ptr<hittable>));
						ImGui::EndDragDropSource();
					}

					ImGui::TableNextColumn();
					ImGui::TextColored(color_mesh, object->get_human_type().c_str());
				}

				if (scene.objects.empty())
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Press \"Create\" to start making objects!");
				}

				ImGui::EndTable();
			}

			// Object creation modal
			bool open_a = true;
			if (ImGui::BeginPopupModal("Create object", &open_a))
			{
				ImGui::SeparatorText("Choose which type of object to create.");

				hittable_combo.combo();
				ImGui::Text("Description: ");
				ImGui::SameLine();
				ImGui::Text(hittable_combo.get_description());

				if (ImGui::Button("Create"))
				{
					hittable_combo.reset_props();
					ImGui::OpenPopup("Define object");
				}

				bool open_b = true;
				if (ImGui::BeginPopupModal("Define object", &open_b))
				{
					if (hittable_combo.create_prompt(scene))
					{
						// No need to refresh viewport since objects are not added to render by default
						ImGui::ClosePopupToLevel(0, true);
					}
					ImGui::EndPopup();
				}

				ImGui::EndPopup();
			}


		}
		ImGui::EndChild();
		ImGui::SameLine();

		// Properties
		if (ImGui::BeginChild("##geoprop", ImVec2(0, 0), ImGuiChildFlags_NavFlattened))
		{
			if (geo_selection >= 0 && geo_selection < scene.objects.size())
			{
				shared_ptr<hittable>& object = scene.objects[geo_selection];

				name_slot(*object, scene);
				ImGui::SetItemTooltip("You can rename it here.");

				ImGui::TextColored(color_mesh, object->get_human_type().c_str());
				ImGui::SetItemTooltip("Type of your object. Objects may be geometry, or modifiers.");
				ImGui::Separator();
				object->inspector_ui(viewport, scene);
			} else
			{
				ImGui::Text("Select an object to edit its properties.");
			}
		}
		ImGui::EndChild();
		ImGui::End();
	}

private:
	// Materials
	int mat_selection = -1;
	void w_materials(bool* p_open, viewport& viewport, scene& scene)
	{
		if (!ImGui::Begin("Materials", p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginChild("##mattree", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
		{
			if (ImGui::Button("Create"))
			{
				// TODO
				// hittable_combo.selection = cube; // uhh for some reason can't reinitialize
				ImGui::OpenPopup("Create material");
			}

			ImGui::BeginDisabled(!(mat_selection >= 0 && mat_selection < scene.materials.size()));

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
			if (ImGui::Button("Deselect"))
			{
				mat_selection = -1;
			}

			ImGui::EndDisabled();


			static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
				| ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

			if (ImGui::BeginTable("mat_list", 2, table_flags))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Type");
				ImGui::TableHeadersRow();

				for (int i = 0; i < scene.materials.size(); i++)
				{
					std::shared_ptr<material>& mat = scene.materials[i];

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Name
					if (ImGui::Selectable(mat->name.c_str(), mat_selection == i, ImGuiSelectableFlags_SpanAllColumns))
						mat_selection = i;

					if (ImGui::BeginDragDropSource())
					{
						ImGui::Text(mat->name.c_str());
						ImGui::SameLine();
						ImGui::TextColored(color_mat,
							(" (" + mat->get_human_type() + ")").c_str());

						ImGui::SetDragDropPayload("REF_MAT", &mat, sizeof(std::shared_ptr<material>));
						ImGui::EndDragDropSource();
					}

					ImGui::TableNextColumn();
					ImGui::TextColored(color_mat, mat->get_human_type().c_str());
				}

				if (scene.materials.empty())
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Press \"Create\" to start making materials!");
				}

				ImGui::EndTable();
			}

			// Object creation modal
			bool open_a = true;
			if (ImGui::BeginPopupModal("Create material", &open_a))
			{
				ImGui::SeparatorText("Choose which type of material to create.");

				// TODO!
				// hittable_combo.combo();
				// ImGui::Text("Description: ");
				// ImGui::SameLine();
				// ImGui::Text(hittable_combo.get_description());
				//
				// if (ImGui::Button("Create"))
				// {
				// 	hittable_combo.reset_props();
				// 	ImGui::OpenPopup("Define material");
				// }
				//
				// bool open_b = true;
				// if (ImGui::BeginPopupModal("Define material", &open_b))
				// {
				// 	if (hittable_combo.create_prompt(scene))
				// 	{
				// 		// No need to refresh viewport since objects are not added to render by default
				// 		ImGui::ClosePopupToLevel(0, true);
				// 	}
				// 	ImGui::EndPopup();
				// }

				ImGui::EndPopup();
			}


		}
		ImGui::EndChild();
		ImGui::SameLine();

		// Properties
		if (ImGui::BeginChild("##matprop", ImVec2(0, 0), ImGuiChildFlags_NavFlattened))
		{
			if (mat_selection >= 0 && mat_selection < scene.materials.size())
			{
				shared_ptr<material>& mat = scene.materials[mat_selection];

				name_slot(*mat, scene);
				ImGui::SetItemTooltip("You can rename it here.");

				ImGui::TextColored(color_mat, mat->get_human_type().c_str());
				ImGui::SetItemTooltip("Type of your material.");
				ImGui::Separator();
				mat->inspector_ui(viewport, scene);
			} else
			{
				ImGui::Text("Select a material to edit its properties.");
			}
		}
		ImGui::EndChild();
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
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
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
	style.Colors[ImGuiCol_Button] = ImVec4(0.268627455830574f, 0.268627455830574f, 0.268627455830574f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.250980406999588f, 0.250980406999588f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.143, 0.222, 0.403, 1.0f);
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

inline ImVec4 const user_interface::color_mesh = ImVec4(.96f,.22f,.67f, 1.0f);
inline ImVec4 const user_interface::color_mat = ImVec4(.69f, .88f, .11f,1.0f);

inline float user_interface::popup_color_storage[3] = { 0.0f, 0.0f, 0.0f };
inline std::string user_interface::popup_string_storage = "";
#endif //RAYTRACINGWEEKEND_USER_INTERFACE_H