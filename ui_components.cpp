#include "ui_components.h"

#include "transformers.h"
#include "volume_convex.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/geometry/geo_disk.h"
#include "primitives/geometry/geo_sphere.h"
#include "user_interface.h"

bool texture_slot(const char* label, std::shared_ptr<texture>& texture_ref, std::shared_ptr<texture>& self_exclude,
	scene& scene)
{
	bool changed = false;
	bool isnull = texture_ref == nullptr;
	if (ImGui::BeginCombo(label, isnull ? "" : texture_ref->name.c_str()))
	{
		// Searchable
		static ImGuiTextFilter filter;
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();
		}

		for (int i = 0; i < scene.textures.size(); i++)
		{
			std::shared_ptr<texture>& i_tex = scene.textures[i];

			if (self_exclude != nullptr && i_tex.get() == self_exclude.get()) // self exclude is ignored when in hittable mode(self_exclude = null)
				continue;

			const bool is_selected = !isnull && (texture_ref.get() == i_tex.get());
			if (filter.PassFilter(i_tex->name.c_str()))
			{
				if (ImGui::Selectable((i_tex->name + " (" + i_tex->get_human_type() + ")").c_str(), is_selected))
				{
					texture_ref = std::shared_ptr(i_tex);
					changed = true;
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SetItemTooltip("Please don\'t make any circular references! Otherwise the program will hang.");

	// Convenience: Create color
	ImGui::SameLine();

	if (ImGui::Button("Make color"))
	{
		user_interface::popup_color_storage[0] = 0.0f;
		user_interface::popup_color_storage[1] = 0.0f;
		user_interface::popup_color_storage[2] = 0.0f;
		user_interface::popup_string_storage = "";
		ImGui::OpenPopup("Make color texture");
	}

	bool open_b = true;
	if (ImGui::BeginPopupModal("Make color texture", &open_b))
	{
		// Make color!
		ImGui::Text("Use this to quickly create a solid color texture and apply it.");

		// name
		bool satisfied = true;
		std::string name_buf = user_interface::popup_string_storage;
		if (ImGui::InputText("Name", &name_buf))
		{
			if (name_buf.size() == 0 ||
				std::ranges::any_of(scene.textures,
									[&name_buf](const std::shared_ptr<texture>& sp) {
										return sp->name == name_buf; // Compare underlying raw pointers
									}))
			{
				// Duplicate
				ImGui::SetTooltip("It must be a unique, non-empty name.");
			} else
			{
				user_interface::popup_string_storage = name_buf;
			}
		}
		if (user_interface::popup_string_storage.empty())
		{
			ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f), "Name must not be empty.");
			satisfied = false;
		}

		// color
		ImGui::ColorEdit3("Color", user_interface::popup_color_storage);

		ImGui::BeginDisabled(!satisfied);
		if (ImGui::Button("Create"))
		{
			// Create texture
			auto tex = make_shared<tex_color>(user_interface::popup_color_storage[0], user_interface::popup_color_storage[1], user_interface::popup_color_storage[2]);
			tex->name = user_interface::popup_string_storage;
			scene.textures.push_back(tex);
			texture_ref = shared_ptr(tex);
			ImGui::CloseCurrentPopup();
			changed = true;
		}
		ImGui::EndDisabled();
		ImGui::EndPopup();
	}


	// Drag and drop
	if (ImGui::BeginDragDropTarget())
	{
		// Hover
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REF_TEX", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip))
		{
			std::shared_ptr<texture> ref = *static_cast<std::shared_ptr<texture>*>(payload->Data);

			if (self_exclude != nullptr && ref.get() == self_exclude.get())
			{
				// Is duplicate
				ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
				ImGui::SetTooltip("No circular references.");
			} else
			{
				ImGui::SetTooltip(std::format("Drop to set as {0}", ref->name).c_str());
				// Not Duplicate
				if (payload->IsDelivery()) // Dropped
				{
					changed = true;
					texture_ref = std::shared_ptr(ref);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	return changed;
}


bool hittable_type_combo::create_prompt(scene& scene)
{
	ImGui::Text(hittable_get_human_type(selection).c_str());

	bool satisified = true;
	// name field
	std::string name_buf = name;
	if (ImGui::InputText("Name", &name_buf))
	{
		if (name_buf.size() == 0 ||
			std::ranges::any_of(scene.objects,
			                    [&name_buf](const std::shared_ptr<hittable>& sp) {
				                    return sp->name == name_buf; // Compare underlying raw pointers
			                    }))
		{
			// Duplicate
			ImGui::SetTooltip("It must be a unique, non-empty name.");
		} else
		{
			name = name_buf;
		}
	}
	if (name.empty())
	{
		ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f), "Name must not be empty.");
		satisified = false;
	}

	if (selection == cube || selection == sphere || selection == quad || selection == disk || selection == volume)
	{
		material_slot("Material", material_ref, scene);
		if (material_ref == nullptr)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "A material must be assigned.");
			satisified = false;
		}
	}

	if (selection == mover || selection == rotator || selection == volume)
	{
		hittable_slot("Target Object", object_ref, scene.world, scene); // scene.world as self-exclude, since it will never be used
		if (object_ref == nullptr)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "The target Object (Shape) must be designated.");
		}
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "More parameters can be configured in the properties panel.");

	ImGui::BeginDisabled(!satisified);

	if (ImGui::Button("Create"))
	{
		switch (selection)
		{
		case cube:
			scene.objects.push_back(make_shared<geo_cube>(name, material_ref));
			break;
		case disk:
			scene.objects.push_back(make_shared<geo_disk>(name, material_ref));
			break;
		case quad:
			scene.objects.push_back(make_shared<geo_quad>(name, material_ref));
			break;
		case sphere:
			scene.objects.push_back(make_shared<geo_sphere>(name, material_ref));
			break;
		case list:
			scene.objects.push_back(make_shared<hittable_list>(name));
			break;
		case volume:
			scene.objects.push_back(make_shared<volume_convex>(name, object_ref, material_ref));
			break;
		case mover:
			scene.objects.push_back(make_shared<trn_move>(name, object_ref));
			break;
		case rotator:
			scene.objects.push_back(make_shared<trn_rotate>(name, object_ref));
			break;
		default:
			// NOT SUPPOSED TO HAPPEN!
			ImGui::SetTooltip("ERROR! Creation of a illegal object type! Please make a bug report!");
			return false;
		}
		ImGui::EndDisabled();
		return true;
	}

	ImGui::EndDisabled();
	return false;
}
