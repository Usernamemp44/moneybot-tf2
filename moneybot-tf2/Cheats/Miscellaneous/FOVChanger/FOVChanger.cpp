#include "FOVChanger.h"
#include "../../CheatList.h"
#include "../../../imgui/imgui.h"
#include "../../../json.h"
#include <iostream>
#include <fstream>

#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"

FOVChanger::FOVChanger() {
	CheatModules["FOVChanger"] = this;
	CanRender = true;
	LoadConfig();
}

void FOVChanger::RenderMenu()
{
	ImGui::SliderFloat("Custom FOV", &CustomFOV, 0, 180);
	ImGui::SliderFloat("Custom scoped FOV", &CustomScopedFOV, 0, 180);
}

void FOVChanger::SaveConfig()
{
	try {
		std::ofstream config("fov_changer.json");
		json j_config;
		j_config["CustomFOV"] = CustomFOV;
		j_config["CustomScopedFOV"] = CustomScopedFOV;
		config << j_config;
		config.close();
	}
	catch (std::exception e)
	{
		assert(0 && "FOVChanger config corrupted");
	}
}

void FOVChanger::LoadConfig()
{
	try {
		std::ifstream config;
		config.open("fov_changer.json");
		if (config.fail()) {
			SaveConfig();
			return;
		}
		json j_config;
		
		config >> j_config; 
		CustomFOV = j_config["CustomFOV"] ;
		CustomScopedFOV = j_config["CustomScopedFOV"] ;
		config.close();
	}
	catch (std::exception e)
	{
		assert(0 && "FOVChanger config corrupted");
	}
}

void FOVChanger::OverrideView(CViewSetup * viewSetup)
{
	viewSetup->fov = CustomFOV;

	CBasePlayer* localPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	if (localPlayer && localPlayer->GetCond() & TFCond_Zoomed)
		viewSetup->fov = CustomScopedFOV;
}

FOVChanger g_FOVChanger;