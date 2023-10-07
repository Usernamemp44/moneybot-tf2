#include "RenderGroups.h"
#include "../../../magic_enum.h"

RenderGroups::RenderGroups()
{
	CheatModules["RenderGroups"] = this;
	CanRender = true;
	RenderGroup* renderGroup = new RenderGroup();
	renderGroup->Name = _strdup("Debug Render Group");
	renderGroups.push_back(renderGroup);
}

void RenderGroups::RenderMenu()
{
	if (ImGui::BeginTabBar("RenderGroupsTabBar")) {
		if (ImGui::BeginTabItem("Render Groups")) {

			if (ImGui::ListBoxHeader("Current render groups"))
			{
				for (size_t n = 0; n < renderGroups.size(); n++)
				{
					const bool is_selected = (CurrentlySelectedRenderGroup == n);
					if (ImGui::Selectable(renderGroups[n]->Name.c_str(), is_selected))
						CurrentlySelectedRenderGroup = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::ListBoxFooter();

			}
			if (ImGui::Button("New render group")) {
				RenderGroup* renderGroup = new RenderGroup;
				renderGroup->Name = "New Render Group";
				renderGroups.push_back(renderGroup);
				CurrentlySelectedRenderGroup = renderGroups.size() - 1;
			}
			if (CurrentlySelectedRenderGroup != -1) {
				if (ImGui::Button("Delete render group")) {
					renderGroups.erase(renderGroups.begin() + CurrentlySelectedRenderGroup);
					CurrentlySelectedRenderGroup--;
				}
				if (CurrentlySelectedRenderGroup != 0) {
					if (ImGui::Button("Move up")) {
						std::iter_swap(renderGroups.begin() + CurrentlySelectedRenderGroup - 1, renderGroups.begin() + CurrentlySelectedRenderGroup);
						CurrentlySelectedRenderGroup = CurrentlySelectedRenderGroup - 1;
					}
				}
				if (renderGroups.size() > 1 && CurrentlySelectedRenderGroup != renderGroups.size() - 1)
				{
					if (CurrentlySelectedRenderGroup != 0) {
						ImGui::SameLine();
					}
					if (ImGui::Button("Move down")) {
						std::iter_swap(renderGroups.begin() + CurrentlySelectedRenderGroup, renderGroups.begin() + CurrentlySelectedRenderGroup + 1);
						CurrentlySelectedRenderGroup = CurrentlySelectedRenderGroup + 1;
					}
				}
			}

			ImGui::EndTabItem();
		}
		if (CurrentlySelectedRenderGroup != -1) {

			RenderGroup* activeRenderGroup = renderGroups[CurrentlySelectedRenderGroup];
			if (ImGui::BeginTabItem("Active Render Group")) {

				if (ImGui::BeginTabBar("ActiveRenderGroup")) {
					if (ImGui::BeginTabItem("Main")) {
						char balls[256];
						strcpy_s(balls, 256, activeRenderGroup->Name.c_str());
						ImGui::InputText("Name", balls, 256);
						activeRenderGroup->Name = balls;
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Filters")) {

						if (ImGui::BeginTabBar("FilterList")) {
							if (ImGui::BeginTabItem("Entities")) {
								constexpr auto& EntityTypes = magic_enum::enum_entries<EntityType>();
								for (size_t i = 0; i < EntityTypes.size(); i++) {
									bool isEnabled = activeRenderGroup->ValidEntityType(EntityTypes[i].first);
									bool temp = isEnabled;
									ImGui::Checkbox(EntityTypes[i].second.data(), &temp);
									if (temp != isEnabled) {
										if (isEnabled) {
											activeRenderGroup->RemoveEntityType(EntityTypes[i].first);
										}
										else {
											activeRenderGroup->AddEntityType(EntityTypes[i].first);
										}
									}
								}
								ImGui::EndTabItem();
							}
							if (ImGui::BeginTabItem("Conditions")) {
								ImGui::Checkbox("Must match all conditions", &activeRenderGroup->MatchAllOrOne);
								ImGui::Separator();
								constexpr auto& Conditions = magic_enum::enum_entries<Condition>();
								for (size_t i = 0; i < Conditions.size(); i++) {
									bool isEnabled = activeRenderGroup->ValidCondition(Conditions[i].first);
									bool temp = isEnabled;
									ImGui::Checkbox(Conditions[i].second.data(), &temp);
									if (temp != isEnabled) {
										if (isEnabled) {
											activeRenderGroup->RemoveCondition(Conditions[i].first);
										}
										else {
											activeRenderGroup->AddCondition(Conditions[i].first);
										}
									}
								}
								ImGui::EndTabItem();
							}
							ImGui::EndTabBar();
						}

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("ESP")) {
						if (ImGui::BeginTabBar("ESP") ){
							activeRenderGroup->EspContainer.RenderMenu();
							ImGui::EndTabBar();
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Chams")) {
						if (ImGui::BeginTabBar("Chams")) {
							activeRenderGroup->ChamsContainer.RenderMenu();
							ImGui::EndTabBar();
						}
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void RenderGroups::LoadConfig()
{
}

void RenderGroups::SaveConfig()
{
}

RenderGroup * RenderGroups::FindMatchingRenderGroup(CBaseEntity * entity)
{
	for (size_t i = 0; i < renderGroups.size(); i++) {
		if (renderGroups[i]->DoesFit(entity))
			return renderGroups[i];
	}
	return nullptr;
}

RenderGroups g_RenderGroups;