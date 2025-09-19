#include "ui_components.h"

#include "transformers.h"
#include "volume_convex.h"
#include "primitives/geometry/geo_cube.h"
#include "primitives/geometry/geo_disk.h"
#include "primitives/geometry/geo_sphere.h"

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
