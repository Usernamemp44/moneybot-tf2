#include "ESP.h"
#include "../CheatList.h"

#include "../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../SourceSDK/Entities/CBuilding/CBuilding.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../SourceSDK/Utils/gamemovement/CTFGameMovement.h"
#include "../Miscellaneous/PlayerInformation/PlayerInformation.h"

#include "../../Misc/Math.h"

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx9.h"
#include "../../Renderer.h"

#include "../../Misc/Colour.h"

#include "../../json.h"
#include <iostream>
#include <fstream>
#include "../../json.h"

#include "RenderGroups/RenderGroups.h"
using json = nlohmann::json;
ESP g_ESP;

#pragma region "BAWLS"
void ESP::LoadConfig() {
	try {
		std::ifstream config;
		config.open("esp.json");
		if (config.fail()) {
			SaveConfig();
			return;
		}
		json j_config;
		config >> j_config;



		Enabled = j_config["Enabled"];
		IgnoreTeammates = j_config["IgnoreTeammates"];
		IgnoreDormants = j_config["IgnoreDormants"];

		PlayerContainer = j_config["PlayerContainer"];
		BuildingContainer = j_config["BuildingContainer"];
		PickupContainer = j_config["PickupContainer"];
		config.close();
	}
	catch (std::exception e)
	{
		assert(0 && "ESP config corrupted");
	}
}
void ESP::SaveConfig() {
	try {
		std::ofstream config("esp.json");
		json j_config;

		j_config["Enabled"] = Enabled;
		j_config["IgnoreTeammates"] = IgnoreTeammates;
		j_config["IgnoreDormants"] = IgnoreDormants;

		j_config["PlayerContainer"] = PlayerContainer;
		j_config["BuildingContainer"] = BuildingContainer;
		j_config["PickupContainer"] = PickupContainer;
		config << j_config;
		config.close();
	}
	catch (std::exception e)
	{
		assert(0 && "ESP config corrupted");
	}
}

#pragma endregion "BAWLS"

ESP::ESP() {
	CheatModules["ESP"] = this;
	CanRender = true;
	LoadConfig();
}

void ESP::RenderMenu() {
	if (ImGui::BeginTabBar("ESPMainMenu")) {
		if (ImGui::BeginTabItem("Main")) {
			ImGui::Checkbox("Enabled", &Enabled);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Ignores")) {
			ImGui::Checkbox("Ignore teammates", &IgnoreTeammates);
			ImGui::Checkbox("Ignore dormants", &IgnoreDormants);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Player ESP")) {
			if (ImGui::BeginTabBar("PlayerESP")) {
				PlayerContainer.RenderMenu();
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Pickup ESP")) {
			if (ImGui::BeginTabBar("PickupESP")) {
				PickupContainer.RenderMenu();
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Building ESP")) {
			if (ImGui::BeginTabBar("BuildingESP")) {
				BuildingContainer.RenderMenu();
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

Vec3 ESP::GetRightAngle(Vec3 vecMyPos, Vec3 vecTargetPos) {
	Vec3 vecAngleToPos;
	Math::VectorAngles(vecTargetPos - vecMyPos, vecAngleToPos);

	Vec3 vecRight;
	Math::AngleVectors(vecAngleToPos, NULL, &vecRight, NULL);

	return vecRight;
}

Colour ESP::GetTeamColor(CBaseEntity* pPlayer) {
	if (pPlayer->GetTeamNum() == 2)
		return Colour(251, 82, 79);
	if (pPlayer->GetTeamNum() == 3)
		return Colour(30, 145, 255);
	return Colour(255, 255, 255);

}


void ESP::UpdateSettings(CBasePlayer* pPlayer) {

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	Settings[pPlayer->GetIndex()].clear();

	ESPSettings espSettings;


	if (!pPlayer->IsDormant() && dormantProgress[pPlayer->GetIndex()] <= 0.3f) {
		dormantProgress[pPlayer->GetIndex()] = 0.3f;
	}

	// Players (hostiles, friendlies, local)
	if (pPlayer->IsPlayer()) {

		if (pPlayer != pLocalPlayer && pPlayer->GetTeamNum() != pLocalPlayer->GetTeamNum()) {

			if (true /* Enabled checkbox */) {
				espSettings.enabled = true;
			}

			if (true /* Bounding box checkbox*/) {
				espSettings.box = true;
			}
			espSettings.health = true;
			espSettings.name = true;
			espSettings.weapon = false;

		}
		else if (pPlayer != pLocalPlayer && pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum()) {

			if (false /* Teamamtes enabled*/) {
				espSettings.enabled = false;
			}

			if (false /* Bounding box teammates */) {
				espSettings.box = false;
			}
			espSettings.health = false;
			espSettings.name = false;
			espSettings.weapon = false;
		}
		else if (pPlayer == pLocalPlayer) {
			espSettings.enabled = false;
			espSettings.health = false;
			espSettings.name = false;
			espSettings.weapon = false;

		}
	}
	else if (pPlayer->IsBuilding()) {
		espSettings.enabled = false;
		espSettings.health = false;
		espSettings.name = false;
		espSettings.weapon = false;
	}
	else if (pPlayer->IsPickup()) {
		espSettings.enabled = false;
		espSettings.box = false;
		espSettings.health = false;
		espSettings.name = false;
		espSettings.weapon = false;
	}
	Settings[pPlayer->GetIndex()].push_back(espSettings);
}

void ESP::DrawPlayer(CBasePlayer* pPlayer) {
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	PlayerInformation* playerInfo = (PlayerInformation*)CheatModules["PlayerInformation"];

	Vec3 vecLocalPlayerPos = pLocalPlayer->GetEyePos();

	// Make sure the player actually exists.
	if (pPlayer == nullptr)
		return;

	float animationRate = 1.0f / 0.3f;
	float rate = GlobalVars()->FrameTime * animationRate;
	float& anim = dormantProgress[pPlayer->GetIndex()];
	float alpha = anim;

	if (!pPlayer->IsDormant()) {
		anim = 1.0f;
	}
	else {
		if (anim < 0.3f) {
			rate *= 0.01f;
		}

		anim = std::clamp(anim -= rate, 0.f, 1.0f);
		if (dormantProgress[pPlayer->GetIndex()] <= 0.f)
			return;
	}

	// Don't draw on dead or dormant players
	// TODO: Don't draw spectators ( once I figure out how to know if they are spectators )
	if (pPlayer->GetLifeState() == LIFE_DEAD)
		return;

	Vec3 vecPlayerOrigin = playerInfo->players[pPlayer->GetIndex()].origin;
	Vec3 vecPlayerToScreen = Math::WorldToScreen(vecPlayerOrigin);

	if (vecPlayerToScreen.x == -1 || vecPlayerToScreen.y == -1)
		return; // Coordinate is not visible

	// Get information about players
	PlayerInfo_t pPlayerInfo;
	CEngineClient::factory()->GetPlayerInfo(pPlayer->GetIndex(), &pPlayerInfo);

	// Get information needed for healthbar rendering
	int iHealth = pPlayer->GetHealth();
	Vec3 vecOBBMins = pPlayer->GetCollideableMins();
	Vec3 vecOBBMaxs = pPlayer->GetCollideableMaxs();

	Vec3 vecRight = GetRightAngle(vecLocalPlayerPos, vecPlayerOrigin);

	vecRight.z = 0;

	Vec3 vecTopEdge = vecPlayerOrigin;
	vecTopEdge.z += vecOBBMaxs.z;

	Vec3 vecBottomEdge = vecPlayerOrigin;
	Vec3 vecTopLeft = vecPlayerOrigin;

	vecOBBMaxs.z /= 2;

	Vec3 vecLeftEdge = vecPlayerOrigin + vecOBBMaxs * -vecRight;
	vecLeftEdge.z += vecOBBMaxs.z;

	Vec3 vecRightEdge = vecPlayerOrigin + vecOBBMaxs * vecRight;
	vecRightEdge.z += vecOBBMaxs.z;

	Vec3 vecTopEdgeScreen = Math::WorldToScreen(vecTopEdge);
	if (InvalidWorldToScreen(vecTopEdgeScreen))
		return;

	Vec3 vecBottomEdgeScreen = Math::WorldToScreen(vecBottomEdge);
	if (InvalidWorldToScreen(vecBottomEdgeScreen))
		return;

	Vec3 vecLeftEdgeScreen = Math::WorldToScreen(vecLeftEdge);
	if (InvalidWorldToScreen(vecLeftEdgeScreen))
		return;

	Vec3 vecRightEdgeScreen = Math::WorldToScreen(vecRightEdge);
	if (InvalidWorldToScreen(vecRightEdgeScreen))
		return;

	//Vec3 vecTopLeftScreen = Math::WorldToScreen(vecTopLeft);
	//Vec3 vecBottomLeftScreen = Math::WorldToScreen(vecBottomLeft);

	float iClampedHealth = std::clamp(iHealth, 0, pPlayer->GetMaxHealth());
	float fHealthPercent = (float)iClampedHealth / (float)pPlayer->GetMaxHealth();

	if (Settings[pPlayer->GetIndex()].empty())
		return;

	for (auto setting : Settings[pPlayer->GetIndex()]) {
		if (!setting.enabled) continue;

		if (setting.box) {
			DrawRectangle((int)vecLeftEdgeScreen.x - 1, (int)vecRightEdgeScreen.x - 1, (int)vecTopEdgeScreen.y + 2, (int)vecBottomEdgeScreen.y + 2, false, Colour(0, 0, 0, 255).dword, 0, false);
			DrawRectangle((int)vecLeftEdgeScreen.x, (int)vecRightEdgeScreen.x, (int)vecTopEdgeScreen.y, (int)vecBottomEdgeScreen.y, false, Colour(200, 200, 200, 255 * alpha).dword, 0, false);
		}
		if (setting.health) {
			DrawRectangle((int)vecLeftEdgeScreen.x - 3, (int)vecLeftEdgeScreen.x - 7, (int)vecTopEdgeScreen.y - 2, (int)vecBottomEdgeScreen.y + 1, true, ColourBlack.dword, 0, false);
			DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 6, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fHealthPercent + vecBottomEdgeScreen.y),
				(int)vecBottomEdgeScreen.y, true, Colour(0, 255, 70, 255 * alpha).dword, 0, false);

			char buf[512];
			sprintf_s(buf, 512, "%0.0f", (double)iHealth);

			//DrawTextCache(buf, (int)vecLeftEdgeScreen.x - 6, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fHealthPercent + vecBottomEdgeScreen.y), 12, 0xFFFFFFFF, true, TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);

			if (iHealth > pPlayer->GetMaxHealth())
			{
				int iOverheal = iHealth % pPlayer->GetMaxHealth();
				float fOverhealFraction = (float)iOverheal / (float)pPlayer->GetMaxHealth();

				DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fOverhealFraction + vecBottomEdgeScreen.y),
					(int)vecBottomEdgeScreen.y, true, ColourCyan.dword, 0, false);
				DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fOverhealFraction + vecBottomEdgeScreen.y) - 1,
					(int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fOverhealFraction + vecBottomEdgeScreen.y), true, ColourBlack.dword, 0, false);
			}
		}
	}
}

void ESP::DrawBuilding(CBuilding* pPlayer) {
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	PlayerInformation* playerInfo = (PlayerInformation*)CheatModules["PlayerInformation"];

	Vec3 vecLocalPlayerPos = playerInfo->players[pLocalPlayer->GetIndex()].eyepos;

	// Make sure the player actually exists.
	if (pPlayer == nullptr)
		return;

	// Don't draw on dead or dormant players
	// TODO: Don't draw spectators ( once I figure out how to know if they are spectators )
	if ((pPlayer->IsDormant() && IgnoreDormants))
		return;

	if (IgnoreTeammates && pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum())
		return;

	Vec3 vecPlayerOrigin = pPlayer->GetAbsOrigin();
	Vec3 vecPlayerToScreen = Math::WorldToScreen(vecPlayerOrigin);

	if (vecPlayerToScreen.x == -1 || vecPlayerToScreen.y == -1)
		return; // Coordinate is not visible

	// Get information about players
	const char* className = "FUCK";
	switch (pPlayer->GetClassId()) {
	case CObjectSentrygun:
		className = ("Sentry Gun");
		break;
	case CObjectDispenser:
		className = ("Dispenser");
		break;
	case CObjectTeleporter:
		className = ("Teleporter");
		break;
	}
	char buffer[4096];
	sprintf_s(buffer, 4096, "Level %u %s", pPlayer->GetLevel(), className);

	// Get information needed for healthbar rendering
	int iHealth = pPlayer->GetHealth();
	Vec3 vecOBBMins = pPlayer->GetCollideableMins();
	Vec3 vecOBBMaxs = pPlayer->GetCollideableMaxs();

	Vec3 vecRight = GetRightAngle(vecLocalPlayerPos, vecPlayerOrigin);

	vecRight.z = 0;

	Vec3 vecTopEdge = vecPlayerOrigin;
	vecTopEdge.z += vecOBBMaxs.z;

	Vec3 vecBottomEdge = vecPlayerOrigin;
	Vec3 vecTopLeft = vecPlayerOrigin;

	vecOBBMaxs.z /= 2;

	Vec3 vecLeftEdge = vecPlayerOrigin + vecOBBMaxs * -vecRight;
	vecLeftEdge.z += vecOBBMaxs.z;

	Vec3 vecRightEdge = vecPlayerOrigin + vecOBBMaxs * vecRight;
	vecRightEdge.z += vecOBBMaxs.z;

	Vec3 vecTopEdgeScreen = Math::WorldToScreen(vecTopEdge);
	if (InvalidWorldToScreen(vecTopEdgeScreen))
		return;

	Vec3 vecBottomEdgeScreen = Math::WorldToScreen(vecBottomEdge);
	if (InvalidWorldToScreen(vecBottomEdgeScreen))
		return;

	Vec3 vecLeftEdgeScreen = Math::WorldToScreen(vecLeftEdge);
	if (InvalidWorldToScreen(vecLeftEdgeScreen))
		return;

	Vec3 vecRightEdgeScreen = Math::WorldToScreen(vecRightEdge);
	if (InvalidWorldToScreen(vecRightEdgeScreen))
		return;

	//Vec3 vecTopLeftScreen = Math::WorldToScreen(vecTopLeft);
	//Vec3 vecBottomLeftScreen = Math::WorldToScreen(vecBottomLeft);

	float iClampedHealth = std::clamp(iHealth, 0, pPlayer->GetMaxHealth());
	float fHealthPercent = (float)iClampedHealth / (float)pPlayer->GetMaxHealth();

	if (BuildingContainer.BoundingBoxEnabled) {
		if (BuildingContainer.BoundingBoxColour == BoundingBoxColourType::Static)
			DrawRectangle((int)vecLeftEdgeScreen.x, (int)vecRightEdgeScreen.x, (int)vecTopEdgeScreen.y, (int)vecBottomEdgeScreen.y, false, BuildingContainer.BoundingBoxStaticColour.dword, 0, false);
		if (BuildingContainer.BoundingBoxColour == BoundingBoxColourType::Health)
			DrawRectangle((int)vecLeftEdgeScreen.x, (int)vecRightEdgeScreen.x, (int)vecTopEdgeScreen.y, (int)vecBottomEdgeScreen.y, false, GenerateGradient(ColourRed.dword, ColourGreen.dword, fHealthPercent).dword, 0, false);
		if (BuildingContainer.BoundingBoxColour == BoundingBoxColourType::Team)
			DrawRectangle((int)vecLeftEdgeScreen.x, (int)vecRightEdgeScreen.x, (int)vecTopEdgeScreen.y, (int)vecBottomEdgeScreen.y, false, GetTeamColor(pPlayer).dword, 0, false);

		if (BuildingContainer.BoundingBoxOutline) {
			DrawRectangle((int)vecLeftEdgeScreen.x - 1, (int)vecRightEdgeScreen.x + 1, (int)vecTopEdgeScreen.y - 1, (int)vecBottomEdgeScreen.y + 1, false, ColourBlack.dword, 0, false);
			DrawRectangle((int)vecLeftEdgeScreen.x + 1, (int)vecRightEdgeScreen.x - 1, (int)vecTopEdgeScreen.y + 1, (int)vecBottomEdgeScreen.y - 1, false, ColourBlack.dword, 0, false);
		}
	}
	// Draw the outline
	if (BuildingContainer.HealthBarEnabled) {
		DrawRectangle((int)vecLeftEdgeScreen.x - 3, (int)vecLeftEdgeScreen.x - 9, (int)vecTopEdgeScreen.y - 1, (int)vecBottomEdgeScreen.y + 1, true, ColourBlack.dword, 0, false);

		switch (BuildingContainer.HealthBar) {
		case HealthBarType::Static:
			DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fHealthPercent + vecBottomEdgeScreen.y),
				(int)vecBottomEdgeScreen.y, true, BuildingContainer.HealthBarStaticColour.dword, 0, false);
			break;
		case HealthBarType::Gradient:
			DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fHealthPercent + vecBottomEdgeScreen.y),
				(int)vecBottomEdgeScreen.y, true, GenerateGradient(BuildingContainer.HealthBarGradientEndColour, BuildingContainer.HealthBarGradientStartColour, fHealthPercent).dword, 0, false);
			break;
		case HealthBarType::FullGradient:
			DrawGradientRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) + vecBottomEdgeScreen.y),
				(int)vecBottomEdgeScreen.y, BuildingContainer.HealthBarGradientStartColour.dword, BuildingContainer.HealthBarGradientEndColour.dword, false);
			DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * (fHealthPercent)+vecBottomEdgeScreen.y),
				(int)vecTopEdgeScreen.y, true, ColourBlack.dword, 0, false);
			break;
		}

		char buf[512];
		sprintf_s(buf, 512, "%0.0f", (double)iHealth);

		DrawTextCache(buf, (int)vecLeftEdgeScreen.x - 6, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fHealthPercent + vecBottomEdgeScreen.y), 12, 0xFFFFFFFF, true, TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);

		if (iHealth > pPlayer->GetMaxHealth())
		{
			int iOverheal = iHealth % pPlayer->GetMaxHealth();
			float fOverhealFraction = (float)iOverheal / (float)pPlayer->GetMaxHealth();

			DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fOverhealFraction + vecBottomEdgeScreen.y),
				(int)vecBottomEdgeScreen.y, true, ColourCyan.dword, 0, false);
			DrawRectangle((int)vecLeftEdgeScreen.x - 4, (int)vecLeftEdgeScreen.x - 8, (int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fOverhealFraction + vecBottomEdgeScreen.y) - 1,
				(int)((vecTopEdgeScreen.y - vecBottomEdgeScreen.y) * fOverhealFraction + vecBottomEdgeScreen.y), true, ColourBlack.dword, 0, false);
		}
	}
	DrawTextCache(buffer, vecLeftEdgeScreen.x + (vecRightEdgeScreen.x - vecLeftEdgeScreen.x) / 2, vecBottomEdgeScreen.y, 12.f, GetTeamColor(pPlayer).dword, true, TEXT_ALIGN_CENTER, TEXT_ALIGN_TOP);
}

void ESP::Render() {
	if (!Enabled)
		return;

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	// FUCK
	if (CTFGameMovement::Init(pLocalPlayer)) {		
		for (int i = 0; i <= 10; i++) {
			CTFGameMovement::FullWalkMove();
			Vec3 postStepOrigin = Math::WorldToScreen(CTFGameMovement::GetMove()->m_vecOrigin);
			if (!InvalidWorldToScreen(postStepOrigin))
				DrawRectangle(postStepOrigin.x - 2.5f, postStepOrigin.x + 2.5f, postStepOrigin.y - 2.5f, postStepOrigin.y + 2.5f, true, Colour(255, 255, 255, 255).dword, 0, true );
		}
		char buffer[4096];
		sprintf_s(buffer, 4096, "Player Velocity %f | Player BaseVelocity: %f | Player MaxSpeed: %f", CTFGameMovement::GetPlayer()->GetVelocity().Length(), CTFGameMovement::GetPlayer()->GetBaseVelocity().Length(), CTFGameMovement::GetMove()->m_flMaxSpeed);
		//DrawTextCache(buffer, 400, 100, 12.f, ColourWhite.dword, true, TEXT_ALIGN_CENTER, TEXT_ALIGN_TOP);

		char buffer2[4096];
		sprintf_s(buffer2, 4096, "Simulation Velocity %f | Simulation MaxSpeed: %f | Simulation Friction: %f", CTFGameMovement::GetMove()->m_vecVelocity.Length(), CTFGameMovement::GetMove()->m_flMaxSpeed, CTFGameMovement::GetMove()->m_surfaceFriction);
		//DrawTextCache(buffer2, 400, 175, 12.f, ColourWhite.dword, true, TEXT_ALIGN_CENTER, TEXT_ALIGN_TOP);

	}
	
	auto entityList = Groups::AllEntites();

	for (auto pPlayer : entityList) {
		UpdateSettings(pPlayer);
		CBuilding* pBuilding = reinterpret_cast<CBuilding*>( CEntityList::factory()->GetClientPlayer(pPlayer->GetIndex()));
		//DrawEntity(pPlayer);
		DrawPlayer(pPlayer);
		//DrawBuilding(pBuilding);
	}
}

void ESPContainer::RenderMenu()
{
	if (ImGui::BeginTabItem("Main")) {
		ImGui::Checkbox("Enabled", &Enabled);
		ImGui::EndTabItem();
	}
	if (Enabled) {
		if (ImGui::BeginTabItem("Health bar")) {
			if (ImGui::Checkbox("Enabled", &HealthBarEnabled)) {

				ImGui::Combo("Health bar type", (int*)&HealthBar, "Gradient\0Full Gradient\0Static\0\0");
				ImVec4 static_color((float)HealthBarStaticColour.r / 255, (float)HealthBarStaticColour.g / 255, (float)HealthBarStaticColour.b / 255, 0.0f);

				ImVec4 fullgradient_start_color((float)HealthBarGradientStartColour.r / 255, (float)HealthBarGradientStartColour.g / 255, (float)HealthBarGradientStartColour.b / 255, 0.0f);
				ImVec4 fullgradient_end_color((float)HealthBarGradientEndColour.r / 255, (float)HealthBarGradientEndColour.g / 255, (float)HealthBarGradientEndColour.b / 255, 0.0f);

				ImVec4 gradient_start_color((float)HealthBarGradientStartColour.r / 255, (float)HealthBarGradientStartColour.g / 255, (float)HealthBarGradientStartColour.b / 255, 0.0f);
				ImVec4 gradient_end_color((float)HealthBarGradientEndColour.r / 255, (float)HealthBarGradientEndColour.g / 255, (float)HealthBarGradientEndColour.b / 255, 0.0f);
				ImGui::PushItemWidth(128);
				switch (HealthBar) {
				case HealthBarType::Static:
					ImGui::ColorEdit3("Static colour", (float*)&static_color, ImGuiColorEditFlags_NoInputs);
					HealthBarStaticColour.r = (int)((float)(static_color.x) * 255);
					HealthBarStaticColour.g = (int)((float)(static_color.y) * 255);
					HealthBarStaticColour.b = (int)((float)(static_color.z) * 255);
					break;
				case HealthBarType::FullGradient:
					ImGui::ColorEdit3("Full gradient start colour", (float*)&fullgradient_start_color, ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit3("Full gradient end colour", (float*)&fullgradient_end_color, ImGuiColorEditFlags_NoInputs);
					HealthBarGradientStartColour.r = (fullgradient_start_color.x * 255); HealthBarGradientStartColour.g = (fullgradient_start_color.y * 255); HealthBarGradientStartColour.b = (fullgradient_start_color.z * 255);
					HealthBarGradientEndColour.r = (fullgradient_end_color.x * 255);	 HealthBarGradientEndColour.g = (fullgradient_end_color.y * 255);	  HealthBarGradientEndColour.b = (fullgradient_end_color.z * 255);
					break;
				case HealthBarType::Gradient:
					ImGui::ColorEdit3("Gradient start colour", (float*)&gradient_start_color, ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit3("Gradient end colour", (float*)&gradient_end_color, ImGuiColorEditFlags_NoInputs);
					HealthBarGradientStartColour.r = (gradient_start_color.x * 255); HealthBarGradientStartColour.g = (gradient_start_color.y * 255); HealthBarGradientStartColour.b = (gradient_start_color.z * 255);
					HealthBarGradientEndColour.r = (gradient_end_color.x * 255);	 HealthBarGradientEndColour.g = (gradient_end_color.y * 255);	  HealthBarGradientEndColour.b = (gradient_end_color.z * 255);
					break;
				}
				ImGui::PopItemWidth();;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Bounding box")) {
			(ImGui::Checkbox("Enabled", &BoundingBoxEnabled));
			if (BoundingBoxEnabled) {
				ImGui::Checkbox("Outline enabled", &BoundingBoxOutline);

				ImGui::Combo("Bounding box type", (int*)&BoundingBox, "2D Full\u00003D\0\0");
				ImVec4 static_color((float)BoundingBoxStaticColour.r / 255, (float)BoundingBoxStaticColour.g / 255, (float)BoundingBoxStaticColour.b / 255, 0.0f);



				ImGui::Combo("Bounding box color type", (int*)&BoundingBoxColour, "Static\0Team\0Health\0\0");
				switch (BoundingBoxColour) {
				case BoundingBoxColourType::Static:
					ImGui::PushItemWidth(128);
					ImGui::ColorEdit3("Static colour", (float*)&static_color, ImGuiColorEditFlags_NoInputs);
					ImGui::PopItemWidth();
					BoundingBoxStaticColour.r = (int)((float)(static_color.x) * 255);
					BoundingBoxStaticColour.g = (int)((float)(static_color.y) * 255);
					BoundingBoxStaticColour.b = (int)((float)(static_color.z) * 255);
					break;
				}

			}
			ImGui::EndTabItem();
		}
	}
} 
