#include "ui_components.h"

#include "user_interface.h"
#include "transformers.h"
#include "volume_convex.h"
#include "im-file-dialog/ImFileDialog.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/geometry/geo_disk.h"
#include "primitives/geometry/geo_sphere.h"
#include "primitives/materials/mat_debug.h"
#include "primitives/materials/mat_emissive.h"
#include "primitives/materials/mat_metallic.h"
#include "primitives/materials/mat_translucent.h"
#include "primitives/textures/tex_checker.h"
#include "primitives/textures/tex_image.h"
#include "primitives/textures/tex_perlin.h"
#include "primitives/textures/tex_uv_debug.h"

bool texture_slot(const char* label, std::shared_ptr<texture>& texture_ref, texture* self_exclude,
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

		for (auto & i_tex : scene.textures)
		{
				if (self_exclude != nullptr && i_tex.get() == self_exclude) // self exclude is ignored when in hittable mode(self_exclude = null)
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

	// Drag and drop
	if (ImGui::BeginDragDropTarget())
	{
		// Hover
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REF_TEX", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip))
		{
			std::shared_ptr<texture> ref = *static_cast<std::shared_ptr<texture>*>(payload->Data);

			if (self_exclude != nullptr && ref.get() == self_exclude)
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

	// Convenience: Create color
	ImGui::SameLine();

	if (ImGui::Button(("Make color##"+std::string(label)).c_str()))
	{
		user_interface::popup_color_storage[0] = 0.0f;
		user_interface::popup_color_storage[1] = 0.0f;
		user_interface::popup_color_storage[2] = 0.0f;
		user_interface::popup_string_storage = "";
		ImGui::OpenPopup(("Make color texture##"+std::string(label)).c_str());
	}

	bool open_b = true;
	if (ImGui::BeginPopupModal(("Make color texture##"+std::string(label)).c_str(), &open_b))
	{
		// Make color!
		ImGui::Text("Use this to quickly create a solid color texture and apply it.");

		// name
		bool satisfied = true;
		std::string name_buf = user_interface::popup_string_storage;
		if (ImGui::InputText("Name", &name_buf))
		{
			if (name_buf.empty() ||
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

	return changed;
}


bool hittable_type_combo::create_prompt(scene& scene)
{
	ImGui::TextColored(user_interface::color_mesh, hittable_get_human_type(selection).c_str());

	bool satisified = true;
	// name field
	std::string name_buf = name;
	if (ImGui::InputText("Name", &name_buf))
	{
		if (name_buf.empty() ||
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
			satisified = false;
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

bool material_type_combo::create_prompt(scene& scene)
{
	ImGui::TextColored(user_interface::color_mat, material_get_human_type(selection).c_str());

	bool satisfied = true;
	// name field
	std::string name_buf = name;
	if (ImGui::InputText("Name", &name_buf))
	{
		if (name_buf.empty() ||
			std::ranges::any_of(scene.materials,
			                    [&name_buf](const std::shared_ptr<material>& sp) {
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
		satisfied = false;
	}

	// only debug normal requires no texture ref
	if (selection != Debug_Normal)
	{
		texture_slot("Texture", tex_ref, scene);
		if (tex_ref == nullptr)
			satisfied = false;
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "More parameters can be configured in the properties panel.");

	ImGui::BeginDisabled(!satisfied);

	if (ImGui::Button("Create"))
	{
		switch (selection)
		{
		case Debug_Normal:
			scene.materials.push_back(make_shared<mat_debug_normal>(name));
			break;
		case Diffuse:
			scene.materials.push_back(make_shared<mat_diffuse>(name, tex_ref));
			break;
		case Emissive:
			scene.materials.push_back(make_shared<mat_emissive>(name, tex_ref));
			break;
		case Metallic:
			scene.materials.push_back(make_shared<mat_metallic>(name, tex_ref));
			break;
		case Translucent:
			scene.materials.push_back(make_shared<mat_translucent>(name, tex_ref));
			break;
		case Volumetric:
			scene.materials.push_back(make_shared<mat_volumetric>(name, tex_ref));
			break;

		default:
			// NOT SUPPOSED TO HAPPEN!
			ImGui::SetTooltip("ERROR! Creation of a illegal material type! Please make a bug report!");
			return false;
		}
		ImGui::EndDisabled();
		return true;
	}

	ImGui::EndDisabled();
	return false;
}

bool texture_type_combo::create_prompt(scene& scene)
{
	ImGui::TextColored(user_interface::color_tex, texture_get_human_type(selection).c_str());

	bool satisfied = true;
	// name field
	std::string name_buf = name;
	if (ImGui::InputText("Name", &name_buf))
	{
		if (name_buf.empty() ||
			std::ranges::any_of(scene.textures,
								[&name_buf](const std::shared_ptr<texture>& sp) {
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
		satisfied = false;
	}

	// Properties

	switch (selection)
	{
	case Color:
		{
			float* col_buf = color_ref.get_float();
			if (ImGui::ColorEdit3("Color", col_buf))
				color_ref.set_float(col_buf);
			break;
		}
	case Image:
		{
			ImGui::BeginDisabled();
			ImGui::InputText("File", &path);
			ImGui::EndDisabled();

			ImGui::SameLine();
			if (ImGui::Button("Pick"))
				ifd::FileDialog::Instance().Open("ImageOpenDialog", "Select an image",
					"Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");

			ImGui::SetItemTooltip("Press to choose an image from your filesystem.");

			if (path.empty())
			{
				ImGui::TextColored(ImVec4(1.0f,.0f,.0f,1.0f), "An image file must be picked.");
				satisfied = false;
			}

			if (ifd::FileDialog::Instance().IsDone("ImageOpenDialog")) {
				if (ifd::FileDialog::Instance().HasResult()) {
					path = ifd::FileDialog::Instance().GetResult().string();
				}
				ifd::FileDialog::Instance().Close();
			}
			break;
		}
	case Perlin:
		{
			texture_slot("Color", tex_ref_a, scene);
			if (tex_ref_a == nullptr)
			{
				ImGui::TextColored(ImVec4(1.0f,.0f,.0f,1.0f), "Color texture must be assigned!");
				satisfied = false;
			}
			break;
		}
	case UV:
		{
			ImGui::TextDisabled("No properties are needed to create this texture.");
			break;
		}
	case Checker:
		{
			texture_slot("Texture A", tex_ref_a, scene);
			ImGui::SetItemTooltip("Texture of the even squares");
			if (tex_ref_a == nullptr)
			{
				ImGui::TextColored(ImVec4(1.0f,.0f,.0f,1.0f), "Texture A must be assigned!");
				satisfied = false;
			}

			texture_slot("Texture B", tex_ref_b, scene);
			ImGui::SetItemTooltip("Texture of the odd squares");
			if (tex_ref_b == nullptr)
			{
				ImGui::TextColored(ImVec4(1.0f,.0f,.0f,1.0f), "Texture B must be assigned!");
				satisfied = false;
			}

			break;
		}
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "More parameters can be configured in the properties panel.");

	ImGui::BeginDisabled(!satisfied);

	if (ImGui::Button("Create"))
	{
		switch (selection)
		{
		case Color:
			scene.textures.push_back(make_shared<tex_color>(name, color_ref));
			break;
		case Image:
			scene.textures.push_back(make_shared<tex_image>(name, path.c_str()));
			break;
		case Perlin:
			scene.textures.push_back(make_shared<tex_perlin>(name, tex_ref_a));
			break;
		case UV:
			scene.textures.push_back(make_shared<tex_uv_debug>(name));
			break;
		case Checker:
			scene.textures.push_back(make_shared<tex_checker>(name, tex_ref_a, tex_ref_b));
			break;

		default:
			// NOT SUPPOSED TO HAPPEN!
			ImGui::SetTooltip("ERROR! Creation of a illegal texture type! Please make a bug report!");
			return false;
		}
		ImGui::EndDisabled();
		return true;
	}

	ImGui::EndDisabled();
	return false;
}
