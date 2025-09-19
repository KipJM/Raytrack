#ifndef RAYTRACINGWEEKEND_UI_COMPONENTS_H
#define RAYTRACINGWEEKEND_UI_COMPONENTS_H
#include <memory>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "misc.h"
#include "scene.h"
#include "hittable.h"
#include "primitives/textures/tex_color.h"


// Name slots
inline bool name_slot(hittable& object, scene& scene)
{
	std::string name_buf = object.name; // copy
	if (ImGui::InputText("##name", &name_buf))
	{
		if (name_buf.size() == 0 ||
			std::ranges::any_of(scene.objects,
			[&object, &name_buf](const std::shared_ptr<hittable>& sp) {
					return sp.get() != &object && sp->name == name_buf; // Compare underlying raw pointers
			}))
		{
			// Duplicate
			ImGui::SetTooltip("It must be a unique, non-empty name.");
		} else
		{
			object.name = name_buf;
			return true;
		}
	}
	return false;
}

inline bool name_slot(material& mat, scene& scene)
{
	std::string name_buf = mat.name; // copy
	if (ImGui::InputText("##name", &name_buf))
	{
		if (name_buf.size() == 0 ||
			std::ranges::any_of(scene.materials,
			[&mat, &name_buf](const std::shared_ptr<material>& sp) {
					return sp.get() != &mat && sp->name == name_buf; // Compare underlying raw pointers
			}))
		{
			// Duplicate
			ImGui::SetTooltip("It must be a unique, non-empty name.");
		} else
		{
			mat.name = name_buf;
			return true;
		}
	}
	return false;
}


// Reference slots

inline bool hittable_slot(const char* label, std::shared_ptr<hittable>& hittable_ref, hittable& self_exclude, scene& scene)
{
	bool changed = false;
	bool isnull = hittable_ref == nullptr;
	if (ImGui::BeginCombo(label, isnull ? "" : hittable_ref->name.c_str()))
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

			const bool is_selected = !isnull && (hittable_ref.get() == i_obj.get());
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

	// Drag and drop
	if (ImGui::BeginDragDropTarget())
	{
		// Hover
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REF_GEO", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip))
		{
			std::shared_ptr<hittable> ref = *static_cast<std::shared_ptr<hittable>*>(payload->Data);

			if (ref.get() == &self_exclude)
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
					hittable_ref = std::shared_ptr(ref);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	return changed;
}

// material

inline bool material_slot(const char* label, std::shared_ptr<material>& material_ref, std::shared_ptr<material>& self_exclude, scene& scene)
{
	bool changed = false;
	bool isnull = material_ref == nullptr;
	if (ImGui::BeginCombo(label, isnull ? "" : material_ref->name.c_str()))
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

			if (self_exclude != nullptr && i_mat.get() == self_exclude.get()) // self exclude is ignored when in hittable mode(self_exclude = null)
				continue;

			const bool is_selected = !isnull && (material_ref.get() == i_mat.get());
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

	// Drag and drop
	if (ImGui::BeginDragDropTarget())
	{
		// Hover
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REF_MAT", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip))
		{
			std::shared_ptr<material> ref = *static_cast<std::shared_ptr<material>*>(payload->Data);

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
					material_ref = std::shared_ptr(ref);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	return changed;
}

inline bool material_slot(const char* label, std::shared_ptr<material>& material_ref, scene& scene)
{
	shared_ptr<material> null_mat = nullptr;
	return material_slot(label, material_ref, null_mat, scene);
}

// texture

bool texture_slot(const char* label, std::shared_ptr<texture>& texture_ref, std::shared_ptr<texture>& self_exclude, scene& scene);

inline bool texture_slot(const char* label, std::shared_ptr<texture>& texture_ref, scene& scene)
{
	shared_ptr<texture> null_tex = nullptr;
	return texture_slot(label, texture_ref, null_tex, scene);
}


// CREATION

class hittable_type_combo
{
public:
	hittable_type_combo() : selection(cube) {} // cube as default

	hittable_type selection;

	void combo()
	{
		if (ImGui::BeginCombo("Object type", hittable_get_human_type(selection).c_str()))
		{
			for (auto& type : types)
			{
				const bool is_selected = type.first == selection;
				if (ImGui::Selectable(hittable_get_human_type(type.first).c_str(), is_selected))
				{
					selection = type.first;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus(); // init focus to this~!
			}
			ImGui::EndCombo();
		}

	}

	const char* get_description()
	{
		for (auto& type : types)
		{
			if (type.first == selection)
				return type.second;
		}
		return "Unknown object. Proceed with caution.";
	}

	std::string name;
	std::shared_ptr<hittable> object_ref;
	std::shared_ptr<material> material_ref;

	void reset_props()
	{
		name = "";
		object_ref = nullptr;
		material_ref = nullptr;
	}

	bool create_prompt(scene& scene);

private:
	const std::pair<hittable_type, const char*> types[8] = {
		std::pair(cube, "(Shape) A world-aligned cube."), // Requires mat, name
		std::pair(sphere, "(Shape) A perfectly smooth sphere."), // Requires mat, name
		std::pair(quad, "(Shape) An arbitrary flat 3D equilateral."), // Requires mat, name
		std::pair(disk, "(Shape) A circular variant of a quad."), // Requires mat, name
		std::pair(mover, "(Modifier) Moves an existing object."), // Requires obj, name
		std::pair(rotator, "(Modifier) Rotates an existing object."), // Requires obj, name
		std::pair(volume, "(Modifier) Volumetrics in the shape of an object."), // Requires obj, mat, name
		std::pair(list, "(Modifier) A list of objects merged together for easier management.") // Requires name
	};
};


#endif //RAYTRACINGWEEKEND_UI_COMPONENTS_H
