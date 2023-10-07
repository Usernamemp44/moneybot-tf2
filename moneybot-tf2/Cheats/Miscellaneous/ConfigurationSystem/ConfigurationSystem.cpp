#include "ConfigurationSystem.h"
#include "../../CheatList.h"
#include "../Console/Console.h"
ConfigurationSystem::ConfigurationSystem() {
	CheatModules["Configs"] = this;
	this->CanRender = true;
}
void ConfigurationSystem::RenderMenu()
{
	if (ImGui::Button("Reload config")) {
		for (auto &x : CheatModules) {
			x.second->LoadConfig();
		}
		Console::puts("Configurations reloaded");
	}
	ImGui::SameLine();
	if (ImGui::Button("Save config")) {
		for (auto &x : CheatModules) {
			x.second->SaveConfig();
		}
		Console::puts("Configurations saved");
	}
}
ConfigurationSystem g_ConfigurationSystem;