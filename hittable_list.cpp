#include "hittable_list.h"

#include "imgui.h"
#include "scene.h"
#include "user_interface.h"
#include "viewport.h"

bool hittable_list::inspector_ui(viewport& _viewport, scene& _scene)
{
	ImGui::Text("A compound is a collection of objects. Please, no circular references.");
	if (ImGui::Button("Add"))
		ImGui::OpenPopup("Add object to compound");

	ImGui::BeginDisabled(!(selection >= 0 && selection < objects.size()));

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
		ImGui::OpenPopup("cmp_remove_item");

	ImGui::SameLine(ImGui::GetWindowWidth() - 70);
	if (ImGui::Button("Deselect"))
	{
		selection = -1;
	}

	ImGui::EndDisabled();

	if (ImGui::BeginPopupModal("Add object to compound", nullptr))
	{
		ImGui::Text("Select an object from your geometries.");

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();


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
				if (std::ranges::any_of(objects,
				                        [&object](const std::shared_ptr<hittable>& sp)
				                        {
					                        return sp.get() == object.get(); // Compare underlying raw pointers
				                        }))
				{
					// No duplicates
					continue;
				}

				if (object.get() == this)
					continue;

				count++;

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				// Name
				if (ImGui::Selectable(object->name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
				{
					// Add object to compound
					_viewport.mark_scene_dirty();
					add(std::shared_ptr(object));
					ImGui::CloseCurrentPopup();
				}


				ImGui::TableNextColumn();
				ImGui::TextColored(user_interface::color_mesh, object->get_human_type().c_str());
			}

			if (_scene.objects.empty())
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No objects! Create some at Geometries!");
			}
			else if (count == 0)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
				                   "Only one instance of each object can be added to compound.");
			}

			ImGui::EndTable();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("cmp_remove_item"))
	{
		ImGui::Text("Remove this object from compound? It will stay in geometries.");
		if (ImGui::Button("Confirm"))
		{
			// remove from scene
			if (selection < objects.size())
			{
				_viewport.mark_scene_dirty();
				remove(selection);
				selection = -1;
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::Text("You can also drag and drop geometries here to add them to the compound.");

	static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
		| ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollY;

	if (ImGui::BeginTable("cmp_list", 2, table_flags))
	{
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Type");
		ImGui::TableHeadersRow();

		for (int i = 0; i < objects.size(); i++)
		{
			std::shared_ptr<hittable>& object = objects[i];

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			// Name
			if (ImGui::Selectable(object->name.c_str(), selection == i, ImGuiSelectableFlags_SpanAllColumns))
				selection = i;

			ImGui::TableNextColumn();
			ImGui::TextColored(user_interface::color_mesh, object->get_human_type().c_str());
		}

		if (objects.empty())
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
			                   "Press add, or drag objects here.");
		}

		ImGui::EndTable();
	}

	if (ImGui::BeginDragDropTarget())
	{
		// Hovering
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
			"REF_GEO", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip))
		{
			std::shared_ptr<hittable> ref = *static_cast<std::shared_ptr<hittable>*>(payload->Data);

			if (std::ranges::any_of(objects,
			                        [&ref, this](const std::shared_ptr<hittable>& sp)
			                        {
				                        return sp.get() == ref.get(); // Compare underlying raw pointers
			                        }))
			{
				// Is duplicate
				ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
				ImGui::SetTooltip("This object already exist in the compound.");
			}
			else if (ref.get() == this)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
				ImGui::SetTooltip("A set cannot contain itself.");
			}
			else
			{
				ImGui::SetTooltip(std::format("Drop to add {0} to the compound.", ref->name).c_str());
				// Not Duplicate
				if (payload->IsDelivery()) // Dropped
				{
					_viewport.mark_scene_dirty();
					add(std::shared_ptr(ref)); // copy
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	return false;
}
