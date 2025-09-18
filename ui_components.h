#ifndef RAYTRACINGWEEKEND_UI_COMPONENTS_H
#define RAYTRACINGWEEKEND_UI_COMPONENTS_H
#include <memory>


// Reference slots

inline bool hittable_slot(const char* label, std::shared_ptr<hittable>& hittable_ref, hittable& self_exclude, scene& scene)
{
	bool changed = false;
	if (ImGui::BeginCombo(label, hittable_ref->name.c_str()))
	{
		// Searchable
		static ImGuiTextFilter filter;
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();
		}

		for (int i = 0; i < scene.objects.size(); i++)
		{
			std::shared_ptr<hittable>& i_obj = scene.objects[i];

			if (i_obj.get() == &self_exclude)
				continue; // prevent self-referencing. Other circular references may still happen though

			const bool is_selected = (hittable_ref.get() == i_obj.get());
			if (filter.PassFilter(i_obj->name.c_str()))
			{
				if (ImGui::Selectable((i_obj->name + " (" + i_obj->get_human_type() + ")").c_str(), is_selected))
				{
					hittable_ref = std::shared_ptr(i_obj);
					changed = true;
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SetItemTooltip("Please don\'t make any circular references! Otherwise the program will hang.");

	//TODO: drag and drop
	return changed;
}

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

	//TODO: Drag and drop
	return changed;
}

#endif //RAYTRACINGWEEKEND_UI_COMPONENTS_H
