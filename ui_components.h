#ifndef RAYTRACINGWEEKEND_UI_COMPONENTS_H
#define RAYTRACINGWEEKEND_UI_COMPONENTS_H
#include <memory>

#include "material.h"
#include "scene.h"

// Reference slots

inline bool material_slot(const char* label, std::shared_ptr<material>& material_ref, scene& scene)
{
	bool changed = false;
	if (ImGui::BeginCombo(label, material_ref->name.c_str()))
	{
		// Searchable
		static ImGuiTextFilter filter;
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();
		}

		for (int i = 0; i < scene.materials.size(); i++)
		{
			std::shared_ptr<material>& i_mat = scene.materials[i];
			const bool is_selected = (material_ref.get() == i_mat.get());
			if (filter.PassFilter(i_mat->name.c_str()))
			{
				if (ImGui::Selectable((i_mat->name + " (" + i_mat->get_human_type() + ")").c_str(), is_selected))
				{
					material_ref = std::shared_ptr(i_mat);
					changed = true;
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SetItemTooltip("Please don\'t make any circular references! Otherwise the program will hang.");

	return changed;
}

#endif //RAYTRACINGWEEKEND_UI_COMPONENTS_H
