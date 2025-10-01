#ifndef RAYTRACINGWEEKEND_USER_INTERFACE_H
#define RAYTRACINGWEEKEND_USER_INTERFACE_H

#include <misc/cpp/imgui_stdlib.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "scene_presets.h"
#include "ui_components.h"
#include "viewport.h"

class user_interface
{
public:
	static ImVec4 const color_mesh;
	static ImVec4 const color_mat;
	static ImVec4 const color_tex;

	static float popup_color_storage[3];
	static std::string popup_string_storage;

	user_interface()
	{
		SetupImGuiStyle(ImGui::GetIO());
	}

	void render(viewport& _viewport)
	{
		ImGui::DockSpaceOverViewport();
		bool open_scn_popup = false;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Help", "", &show_help);
				ImGui::Separator();
				ImGui::MenuItem("Viewport", "", &show_viewport);
				ImGui::Separator();
				ImGui::MenuItem("Render Settings", "", &show_render);
				ImGui::MenuItem("Camera Settings", "", &show_camera);
				ImGui::Separator();
				ImGui::MenuItem("Scene", "", &show_scene);
				ImGui::MenuItem("Objects", "", &show_geometry);
				ImGui::MenuItem("Materials", "", &show_material);
				ImGui::MenuItem("Textures", "", &show_texture);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scenes"))
			{
				ImGui::SeparatorText("Create");

				if (ImGui::MenuItem("Empty scene")) {
					preset_scene_creator::selection = Empty;
					open_scn_popup = true;
				}

				ImGui::SeparatorText("Load demos");

				if(ImGui::MenuItem("Plain scene")) {
					preset_scene_creator::selection = Sky;
					open_scn_popup = true;
				}
				if(ImGui::MenuItem("Absolutely no shiny sphere")) {
					preset_scene_creator::selection = Chrome;
					open_scn_popup = true;
				}
				if(ImGui::MenuItem("Weird Cornell box")) {
					preset_scene_creator::selection = Cornell;
					open_scn_popup = true;
				}
				if(ImGui::MenuItem("Lots of spheres")) {
					preset_scene_creator::selection = Spheres;
					open_scn_popup = true;
				}
				if(ImGui::MenuItem("Emissive")) {
					preset_scene_creator::selection = Dark;
					open_scn_popup = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (open_scn_popup)
		{
			ImGui::OpenPopup("Confirm scene change?");
		}
		bool open_a = true;
		if (ImGui::BeginPopupModal("Confirm scene change?", &open_a))
		{
			ImGui::TextColored(ImVec4(1.0f,.0f,.0f,1.0f), "All data will be lost!");
			ImGui::TextWrapped("Press confirm to open the new scene. All textures, materials, objects and camera settings will be reset!");
			if (ImGui::Button("Confirm, change to new scene"))
			{
				preset_scene_creator::update_scene(_viewport, preset_scene_creator::selection);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (show_help) w_help(&show_help);
		if (show_viewport) w_viewport(&show_viewport, _viewport);
		if (show_render) w_renderSettings(&show_render, _viewport);
		if (show_camera) w_cameraSettings(&show_camera, _viewport);
		if (show_scene) w_scene(&show_scene, _viewport, _viewport.target_scene);
		if (show_geometry) w_geometries(&show_geometry, _viewport, _viewport.target_scene);
		if (show_material) w_materials(&show_material, _viewport, _viewport.target_scene);
		if (show_texture) w_textures(&show_texture, _viewport, _viewport.target_scene);
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
		ImGui::BulletText("This is a CPU raytracer, so rendering is heavily dependent on your CPU's multicore performance.");
		ImGui::BulletText("This engine works a bit differently than conventional editors. I highly recommend reading the content below.");
		ImGui::BulletText("Goto the top left and click on the \"windows\" menu to open the necessary editor windows to use Raytrack.");

		if (ImGui::CollapsingHeader("Introduction"))
		{
			ImGui::BulletText("Raytrack is a raytracer, it works by simulating rays of light bouncing around in the scene.");
			ImGui::BulletText("Raytracing is very accurate and versatile, but quite slow.");
			ImGui::BulletText("Raytrack is designed to be as responsive as possible, but it still may take a few second to render any changes.");
			ImGui::Spacing();
			ImGui::BulletText("Only objects added to your scene will show up in your render.");
			ImGui::BulletText("Most UI elements have tooltips. Hover to see relevant help information.");
			ImGui::BulletText("Made by KIP. If you have any questions, ask them in the Github issues page");
			ImGui::Text("ICYMI, this is very much a toy renderer! Not for serious use :)");
			ImGui::TextLinkOpenURL("GitHub", "https://github.com/kipjm/raytrack");
			ImGui::SameLine();
			ImGui::TextLinkOpenURL("KIP Website", "https://kip.gay");
		}

		if (ImGui::CollapsingHeader("Editor windows"))
		{
			ImGui::BulletText("You can resize, dock, and undock any windows.");
			ImGui::BulletText("Help: This help window!");
			ImGui::BulletText("Viewport: Your render will be displayed here");
			ImGui::BulletText("Render Settings: Global rendering and performance settings that persist between scenes.");
			ImGui::BulletText("Camera Settings: Scene-specific camera parameters. They will be reset when you change scenes.");
			ImGui::BulletText("Objects, Materials, Textures: Manages your staging resources, they will be reset when you change scenes. Learn more below!");
			ImGui::BulletText("Scene: Only objects in this scene list will be rendered. Reset between scenes!");
		}

		if (ImGui::CollapsingHeader("User Interface"))
		{
			ImGui::BulletText("Windows can be windowed or docked, and they can be resized arbitrairly.");
			ImGui::BulletText("Most value fields can be dragged to be modified incrementally.");
			ImGui::BulletText("Ctrl-Click or double click a field to enter a specific value.");
			ImGui::BulletText("Objects, materials, and textures can be dragged and dropped into reference fields.");
			ImGui::BulletText("Objects can also be dragged and dropped into the scene list or compound lists.");
			ImGui::BulletText("Hover on fields/text to get tooltips.");
			ImGui::BulletText("Click the X to cancel a modal operation.");
		}

		if (ImGui::CollapsingHeader("Optimization (Render settings)"))
		{
			ImGui::BulletText("Raytrack focuses on responsiveness than speed of render completion.");
			ImGui::BulletText("Raytrack provides many parameters to optimize your renders. Here are the main ones:");
			ImGui::BulletText("Render threads: more threads mean your render gets finished quicker. But this is dependent on your multicore performance.");
			ImGui::BulletText("Render probability: This dictates the rough percentage of pixels that will be drawn in randomly by a thread in a frame.");
			ImGui::BulletText("By setting this value low, you'll get a more responsive render at the cost of taking longer to get the final render.");
			ImGui::BulletText("The fill probability helps clean up the noise of black pixels. When pixels are neglected because of randomness, this probability will be used for this pixel instead.");
			ImGui::BulletText("If you want a traditional slow renderer, change render probability to 1.");
			ImGui::Spacing();
			ImGui::BulletText("Bounded Volume Hierachy is also automatically calculated when you update the scene.");
			ImGui::BulletText("But sometimes bounding boxes may fail to refresh. Modify object values to trigger a recalculation!");
		}

		if (ImGui::CollapsingHeader("Coordinate System"))
		{
			ImGui::BulletText("Raytrack uses a Y-up right handed coordinate system.");
			ImGui::BulletText("X+ is right, Y+ is up, and Z- is forward.");
			ImGui::BulletText("For rotation, we use right-handed euler angles in degrees.");
			ImGui::BulletText("Rotation is applied in the order of X, Y, Z.");
			ImGui::BulletText("All angles are in degrees, and meter can be considered the length unit.");
		}

		if (ImGui::CollapsingHeader("Scene"))
		{
			ImGui::BulletText("A scene is a collection of objects, materials, textures, and camera settings.");
			ImGui::BulletText("However, not all objects will be rendered in a scene by default.");
			ImGui::BulletText("Only objects in the scene list (scene window) will be rendered.");
			ImGui::BulletText("The scene list contains references to objects.");
			ImGui::BulletText("If you want to modify any object's properties, do it in the objects window.");
			ImGui::BulletText("The scene will automatically refresh and re-render when you change anything.");
		}

		if (ImGui::CollapsingHeader("Objects"))
		{
			ImGui::BulletText("There are two types of objects: Geometry and modifiers.");
			ImGui::BulletText("Geometry objects displays the parameterized geometry with the selected material.");
			ImGui::BulletText("Modifiers takes an object reference, then displays the modified copy of that object.");
			ImGui::BulletText("Please do not make circular references using object slots! That WILL crash the program.");
		}

		if (ImGui::CollapsingHeader("Materials"))
		{
			ImGui::BulletText("Materials dictate how light is redirected when it hits an object.");
			ImGui::BulletText("Materials only have an effect when they're referenced by a object.");
			ImGui::BulletText("Some materials may reference other materials. Please no circular references!");
			ImGui::BulletText("As always, you can drag and drop into reference fields.");
		}

		if (ImGui::CollapsingHeader("Textures"))
		{
			ImGui::BulletText("Textures are just 2D data mapped to the UVs of an object.");
			ImGui::BulletText("Similar to materials and objects, you can drag and drop.");
			ImGui::BulletText("If you want a material to be a singular color, create a color texture.");
			ImGui::BulletText("There's a useful button next to texture slots, it can quickly create color textures.");
			ImGui::BulletText("You can also load your own image files using an image texture.");
		}

		if (ImGui::CollapsingHeader("About"))
		{
			ImGui::Text("Raytrack");
			ImGui::Text("Version 1.2");
			ImGui::Text("Changelog");
			ImGui::BulletText("Debug console updates");
			ImGui::BulletText("Experimental low-light per-pixel dynamic sampling");
		}

		ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Happy rendering!");
		ImGui::Text("KIP");
		ImGui::End();
	}

	ImVec2 prev_resolution;
	void w_viewport(bool* p_open, viewport& _viewport)
	{
		if (!ImGui::Begin("Viewport", p_open))
		{
			ImGui::End();
			return;
		}

		ImGui::Text("Resolution: %d x %d", _viewport.get_width(), _viewport.get_height());
		if (_viewport.get_camera().auto_resolution)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1,1,0,1), "(Auto resolution enabled)");
		}
		ImGui::Text("Samples: %d", _viewport.get_current_sample_count());

		if (_viewport.is_waiting())
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1,0,0,1), "(OUTDATED IMAGE! Waiting for new render!)");
		}

		// Auto resolution
		if (_viewport.get_camera().auto_resolution)
		{
			auto current_res = ImGui::GetContentRegionAvail();
			if (current_res.x != prev_resolution.x || current_res.y != prev_resolution.y)
			{
				prev_resolution = current_res;
				_viewport.set_resolution(current_res.x, current_res.y);
			}
		}

		// Auto scaling image
		auto available_space = ImGui::GetContentRegionAvail();
		auto ratio = std::min(available_space.x / _viewport.get_width(), available_space.y / _viewport.get_height());
		ratio = std::min(ratio, 8.0f); // max ratio
		auto scaled_size = ImVec2(_viewport.get_width() * ratio, _viewport.get_height() * ratio);

		ImGui::SetCursorPosX((available_space.x - scaled_size.x) * 0.5f);

		ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(_viewport.get_texture_id())), scaled_size);

		ImGui::End();
	}

	void w_renderSettings(bool* p_open, viewport& _viewport)
	{
		if (!ImGui::Begin("Render Settings", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}
		ImGui::SetItemTooltip("These settings persist across scenes, but gets reset after application closes.");

		ImGui::SeparatorText("Rendering");

		int mb = _viewport.get_max_bounces();
		if (ImGui::DragInt("Max ray bounces", &mb)) _viewport.set_max_bounces(mb);
		ImGui::SetItemTooltip("Maximum number of bounces a ray can have before getting terminated. Higher = better quality & slower rendering");

		double bi = _viewport.get_bias();
		if (ImGui::InputDouble("bias", &bi)) _viewport.set_bias(bi);
		ImGui::SetItemTooltip("A small number. Fixes rendering issues. Do not touch this if you don't know what you're doing!");


		ImGui::SeparatorText("Performance");

		int tc = _viewport.get_workers_count();
		if (ImGui::InputInt("Render threads", &tc, 1, 10)) _viewport.set_worker_count(tc);
		ImGui::SetItemTooltip("For multithreaded rendering. How many worker threads to render your image at the same time. A reasonable number will make rendering faster by utilizing your entire system, but setting this too high will reduce performance.");

		int sc = _viewport.get_sample_count();
		if (ImGui::DragInt("Samples per worker", &sc, 1)) _viewport.set_sample_count(sc);
		ImGui::SetItemTooltip("How many samples for each pixel should a worker take for each iteration. A low number (like 1) will make rendering more responsive, but a higher number will give you higher quality results faster.");

		int ms = _viewport.get_min_samples();
		if (ImGui::DragInt("Target samples count", &ms, 1)) _viewport.set_min_samples(ms);
		ImGui::SetItemTooltip("After rendering a while, workers will prefer rendering pixels with less than this amount of samples based on the fill probability.");

		double br = _viewport.get_basic_ratio();
		if (ImGui::DragDouble("Render probability", &br, 0.1, 0.01, 1)) _viewport.set_basic_ratio(br);
		ImGui::SetItemTooltip("Approximate ratio of pixels a worker will fill each iteration. A small ratio will make rendering more responsive, but slower.");

		double fr = _viewport.get_fill_ratio();
		if (ImGui::DragDouble("Fill probability", &fr, 0.1, 0.01, 1)) _viewport.set_fill_ratio(fr);
		ImGui::SetItemTooltip("A special ratio to use for pixels with less samples than target after a while. Setting this relatively high will make workers actively fill holes in your render.");

		int ds = _viewport.get_dark_samples();
		if (ImGui::DragInt("Dark fill count", &ds)) _viewport.set_dark_samples(ds);
		ImGui::SetItemTooltip("[EXPERIMENTAL] Number of additional samples for a pixel to help reduce noise in low-light areas, where black adds this number of samples, white adds none. Set to 0 to disable feature.");

		ImGui::End();
	}

	void w_cameraSettings(bool* p_open, viewport& _viewport)
	{
		if (!ImGui::Begin("Camera Settings", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}
		ImGui::SetItemTooltip("Scene-specific camera settings. They change between each scene.");

		camera& cam = _viewport.get_camera();

		ImGui::SeparatorText("Resolution");

		ImGui::Checkbox("Scale to window", &cam.auto_resolution);

		ImGui::BeginDisabled(cam.auto_resolution);

		int iw = cam.image_width;
		int ih = cam.image_height;
		int res[] = {iw, ih};
		if (ImGui::DragInt2("Resolution", res)) _viewport.set_resolution(res[0], res[1]);
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

		if (dirty) _viewport.mark_dirty();

		ImGui::End();
	}

private:
	int scene_selection = -1;

	void w_scene(bool *p_open, viewport& _viewport, scene& _scene)
	{
		if (!ImGui::Begin("Scene Hierarchy", p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::Button("Add"))
			ImGui::OpenPopup("Add object to scene");

		ImGui::BeginDisabled(!(scene_selection >= 0 && scene_selection < _scene.world.objects.size()));

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
				for (std::shared_ptr<hittable>& object : _scene.objects)
				{
					if (std::ranges::any_of(_scene.world.objects,
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
						_viewport.mark_scene_dirty();
						_scene.world.add(std::shared_ptr(object));
						ImGui::CloseCurrentPopup();
					}


					ImGui::TableNextColumn();
					ImGui::TextColored(color_mesh, object->get_human_type().c_str());
				}

				if (_scene.objects.empty())
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
				if (scene_selection < _scene.world.objects.size())
				{
					_viewport.mark_scene_dirty();
					_scene.world.remove(scene_selection);
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

			for (int i = 0; i < _scene.world.objects.size(); i++)
			{
				std::shared_ptr<hittable>& object = _scene.world.objects[i];

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				// Name
				if (ImGui::Selectable(object->name.c_str(), scene_selection == i, ImGuiSelectableFlags_SpanAllColumns))
					scene_selection = i;

				ImGui::TableNextColumn();
				ImGui::TextColored(color_mesh, object->get_human_type().c_str());
			}

			if (_scene.world.objects.empty())
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

				if (std::ranges::any_of(_scene.world.objects,
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
						_viewport.mark_scene_dirty();
						_scene.world.add(std::shared_ptr(ref)); // copy
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
	void w_geometries(bool* p_open, viewport& _viewport, scene& _scene)
	{
		if (!ImGui::Begin("Objects", p_open))
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

			ImGui::BeginDisabled(!(geo_selection >= 0 && geo_selection < _scene.objects.size()));

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

				for (int i = 0; i < _scene.objects.size(); i++)
				{
					std::shared_ptr<hittable>& object = _scene.objects[i];

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

				if (_scene.objects.empty())
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
					if (hittable_combo.create_prompt(_scene))
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
			if (geo_selection >= 0 && geo_selection < _scene.objects.size())
			{
				shared_ptr<hittable>& object = _scene.objects[geo_selection];

				name_slot(*object, _scene);
				ImGui::SetItemTooltip("You can rename it here.");

				ImGui::TextColored(color_mesh, object->get_human_type().c_str());
				ImGui::SetItemTooltip("Type of your object. Objects may be geometry, or modifiers.");
				ImGui::Separator();
				object->inspector_ui(_viewport, _scene);
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
	material_type_combo material_combo;
	int mat_selection = -1;
	void w_materials(bool* p_open, viewport& _viewport, scene& _scene)
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
				material_combo.selection = Diffuse; // uhh for some reason can't reinitialize
				ImGui::OpenPopup("Create material");
			}

			ImGui::BeginDisabled(!(mat_selection >= 0 && mat_selection < _scene.materials.size()));

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

				for (int i = 0; i < _scene.materials.size(); i++)
				{
					std::shared_ptr<material>& mat = _scene.materials[i];

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

				if (_scene.materials.empty())
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

				material_combo.combo();
				ImGui::Text("Description: ");
				ImGui::SameLine();
				ImGui::Text(material_combo.get_description());

				if (ImGui::Button("Create"))
				{
					material_combo.reset_props();
					ImGui::OpenPopup("Define material");
				}

				bool open_b = true;
				if (ImGui::BeginPopupModal("Define material", &open_b))
				{
					if (material_combo.create_prompt(_scene))
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
		if (ImGui::BeginChild("##matprop", ImVec2(0, 0), ImGuiChildFlags_NavFlattened))
		{
			if (mat_selection >= 0 && mat_selection < _scene.materials.size())
			{
				shared_ptr<material>& mat = _scene.materials[mat_selection];

				name_slot(*mat, _scene);
				ImGui::SetItemTooltip("You can rename it here.");

				ImGui::TextColored(color_mat, mat->get_human_type().c_str());
				ImGui::SetItemTooltip("Type of your material.");
				ImGui::Separator();
				mat->inspector_ui(_viewport, _scene);
			} else
			{
				ImGui::Text("Select a material to edit its properties.");
			}
		}
		ImGui::EndChild();
		ImGui::End();
	}

private:
	// Textures
	int tex_selection = -1;
	texture_type_combo texture_combo;

	void w_textures(bool* p_open, viewport& _viewport, scene& _scene)
	{
		if (!ImGui::Begin("Textures", p_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginChild("##textree", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
		{
			if (ImGui::Button("Create"))
			{
				texture_combo.selection = Color; // uhh for some reason can't reinitialize
				ImGui::OpenPopup("Create texture");
			}

			ImGui::BeginDisabled(!(tex_selection >= 0 && tex_selection < _scene.textures.size()));

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
			if (ImGui::Button("Deselect"))
			{
				tex_selection = -1;
			}

			ImGui::EndDisabled();


			static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
				| ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

			if (ImGui::BeginTable("tex_list", 2, table_flags))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Type");
				ImGui::TableHeadersRow();

				for (int i = 0; i < _scene.textures.size(); i++)
				{
					std::shared_ptr<texture>& tex = _scene.textures[i];

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					// Name
					if (ImGui::Selectable(tex->name.c_str(), tex_selection == i, ImGuiSelectableFlags_SpanAllColumns))
						tex_selection = i;

					if (ImGui::BeginDragDropSource())
					{
						ImGui::Text(tex->name.c_str());
						ImGui::SameLine();
						ImGui::TextColored(color_tex,
							(" (" + tex->get_human_type() + ")").c_str());

						ImGui::SetDragDropPayload("REF_TEX", &tex, sizeof(std::shared_ptr<texture>));
						ImGui::EndDragDropSource();
					}

					ImGui::TableNextColumn();
					ImGui::TextColored(color_tex, tex->get_human_type().c_str());
				}

				if (_scene.textures.empty())
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Press \"Create\" to start making textures!");
				}

				ImGui::EndTable();
			}

			// Object creation modal
			bool open_a = true;
			if (ImGui::BeginPopupModal("Create texture", &open_a))
			{
				ImGui::SeparatorText("Choose which type of texture to create.");

				texture_combo.combo();
				ImGui::Text("Description: ");
				ImGui::SameLine();
				ImGui::Text(texture_combo.get_description());

				if (ImGui::Button("Create"))
				{
					texture_combo.reset_props();
					ImGui::OpenPopup("Define texture");
				}

				bool open_b = true;
				if (ImGui::BeginPopupModal("Define texture", &open_b))
				{
					if (texture_combo.create_prompt(_scene))
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
		if (ImGui::BeginChild("##texprop", ImVec2(0, 0), ImGuiChildFlags_NavFlattened))
		{
			if (tex_selection >= 0 && tex_selection < _scene.textures.size())
			{
				shared_ptr<texture>& tex = _scene.textures[tex_selection];

				name_slot(*tex, _scene);
				ImGui::SetItemTooltip("You can rename it here.");

				ImGui::TextColored(color_tex, tex->get_human_type().c_str());
				ImGui::SetItemTooltip("Type of your texture.");
				ImGui::Separator();
				tex->inspector_ui(_viewport, _scene);
			} else
			{
				ImGui::Text("Select a texture to edit its properties.");
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
inline ImVec4 const user_interface::color_tex = ImVec4(.19f, .71f, .95f,1.0f);

inline float user_interface::popup_color_storage[3] = { 0.0f, 0.0f, 0.0f };
inline std::string user_interface::popup_string_storage = "";
#endif //RAYTRACINGWEEKEND_USER_INTERFACE_H