#include "Chams.h"
#include <algorithm>
#include "../../json.h"
#include <iostream>
#include <fstream>
#include "../../json.h"
#include "../../SourceSDK/Entities/CItem/CItem.h"
#include "../../SourceSDK/Entities/CBuilding/CBuilding.h"
#include "RenderGroups/RenderGroups.h"
using json = nlohmann::json;
Chams::Chams() {
	CheatModules["Chams"] = this;
	this->CanRender = true;
	LoadConfig();
}


void Chams::LoadConfig() {
	try {
		std::ifstream config;
		config.open("chams.json");
		if (config.fail()) {
			SaveConfig();
			return;
		}
		json j_config;
		config >> j_config;

		PlayerChams = j_config["PlayerChams"];
		PickupChams = j_config["PickupChams"];
		BuildingChams = j_config["BuildingChams"];
		Enabled = j_config["Enabled"];
		IgnoreTeammates = j_config["IgnoreTeammates"];
		IgnoreDormant = j_config["IgnoreDormant"];


		config.close();
	}
	catch (std::exception e)
	{
		assert(0 && "Chams config corrupted");
	}
}
void Chams::SaveConfig() {
	try {
		std::ofstream config("chams.json");
		json j_config;




		j_config["Enabled"] = Enabled;
		j_config["IgnoreTeammates"] = IgnoreTeammates;
		j_config["IgnoreDormant"] = IgnoreDormant;

		j_config["PlayerChams"] = PlayerChams;
		j_config["PickupChams"] = PickupChams;
		j_config["BuildingChams"] = BuildingChams;




		config << j_config;
		config.close();
	}
	catch (std::exception e)
	{
		assert(0 && "Chams config corrupted");
	}
}

IMaterial *Chams::CreateMaterial(bool ignorez, bool flat, bool wireframe, bool add_shine)
{
	static int created = 0;
	static const char tmp[] =
	{
		"\"%s\"\
			\n{\
			\n\t\"$basetexture\" \"vgui/white_additive\"\
			\n\t\"$envmap\" \"%s\"\
			\n\t\"$normalmapalphaenvmapmask\" \"%i\"\
			\n\t\"$envmapcontrast\" \"%i\"\
			\n\t\"$model\" \"1\"\
			\n\t\"$flat\" \"%i\"\
			\n\t\"$nocull\" \"0\"\
			\n\t\"$selfillum\" \"1\"\
			\n\t\"$halflambert\" \"1\"\
			\n\t\"$nofog\" \"0\"\
			\n\t\"$ignorez\" \"%i\"\
			\n\t\"$znearer\" \"0\"\
			\n\t\"$wireframe\" \"%i\"\
			\n}\n"
	};

	const char *baseType = (flat ? "UnlitGeneric" : "VertexLitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, add_shine ? "env_cubemap" : "", add_shine ? 1 : 0, add_shine ? 1 : 0, add_shine ? 1 : 0, (ignorez) ? 1 : 0, (wireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#textured_cham_material%i.vmt", created);
	created++;

	KeyValues *keyvalues = (KeyValues *)malloc(sizeof(KeyValues));
	CKeyValues::Initialize(keyvalues, (char*)baseType);
	CKeyValues::LoadFromBuffer(keyvalues, name, material);

	IMaterial *created_material = IMaterialSystem::factory()->CreateMaterial(name, keyvalues);

	if (!created_material)
		return nullptr;

	created_material->IncrementReferenceCount();
	return created_material;
}
IMaterial* Chams::CreateShinyMaterial() {
	static const char tmp[] =
	{
		"\"VertexLitGeneric\"\
			\n{\
			\n\t\"$basetexture\" \"vgui/white_additive\"\
			\n\t\"$envmap\" \"effects/cubemapper\"\
			\n\t\"$envmapcontrast\" \"10\"\
			\n\t\"$normalmapalphaenvmapmask\" \"1\"\
			\n\t\"$envmapcontrast\" \"1\"\
			\n\t\"$model\" \"1\"\
			\n\t\"$flat\" \"1\"\
			\n\t\"$rimlight\" \"1\"\
			\n\t\"$rimlightboost\" \"15\"\
			\n\t\"$rimlightexponent\" \"50\"\
			\n\t\"$rimmask\" \"0\"\
			\n\t\"$phong\" \"1\"\
			\n\t\"$phongboost\" \"100\"\
			\n\t\"$phongexponent\" \"200\"\
			\n\t\"$nocull\" \"0\"\
			\n\t\"$bumpmap\" \"water/tfwater001_normal\"\
			\n\t\"$selfillum\" \"1\"\
			\n\t\"$halflambert\" \"1\"\
			\n\t\"$basemapluminancephongmask\" \"1\"\
			\n\t\"$basemapalphaphongmask\" \"1\"\
			\n\t\"$selfillum\" \"1\"\
			\n\t\"$halflambert\" \"1\"\
			\n\t\"$nofog\" \"0\"\
			\n\t\"$znearer\" \"0\"\
			\n\t\"$wireframe\" \"%i\"\
			\n}\n"
	};	

	char name[512];
	sprintf_s(name, sizeof(name), "#shiny.vmt");

	KeyValues *keyvalues = (KeyValues *)malloc(sizeof(KeyValues));
	CKeyValues::Initialize(keyvalues, (char*)"VertexLitGeneric");
	CKeyValues::LoadFromBuffer(keyvalues, name, tmp);

	IMaterial *created_material = IMaterialSystem::factory()->CreateMaterial(name, keyvalues);

	if (!created_material)
		return nullptr;

	created_material->IncrementReferenceCount();
	return created_material;
}
void Chams::SetCham(IMaterial* material, Colour clr, float alpha) {
	float float_color[3] = { (float)(clr.R) / 255.0f,(float)(clr.G) / 255.0f,(float)(clr.b) / 255.0f };

	CRenderView::factory()->SetBlend(alpha);
	CRenderView::factory()->SetColorModulation(float_color);
	CModelRender::factory()->ForcedMaterialOverride(material);
}

void Chams::ResetCham() {
	float float_color[3] = { 1,1,1};
	CRenderView::factory()->SetBlend(1.0f);
	CRenderView::factory()->SetColorModulation(float_color);
	CModelRender::factory()->ForcedMaterialOverride(NULL);
}

void Chams::InitializeMaterials() {
	shaded = CreateMaterial(false, false, false, false);
	flat = CreateMaterial(false, true, false, false);
	wireframe = CreateMaterial(false, false, true, false);
	shiny = CreateShinyMaterial();
}
Colour Chams::GetTeamColor(CBaseEntity* pPlayer) {
	if (pPlayer->GetTeamNum() == 2)
		return Colour(251, 82, 79);
	if (pPlayer->GetTeamNum() == 3)
		return Colour(30, 145, 255);
	return Colour(255, 255, 255);
}
Colour Chams::GetChamColor(bool invisible, CBaseEntity* entity) {
	if (entity->IsPickup())
		return GetChamColor(invisible, (CItem*)entity);
	if (invisible) {
		switch (ActiveContainer->InvisibleChamColour) {
		
		case ChamColourType::Static:
			return ActiveContainer->InvisibleChamStaticColour;
		default:
		case ChamColourType::Team:
			return GetTeamColor(entity);
		case ChamColourType::Health:
			float health = std::clamp(entity->GetHealth(), 0, entity->GetMaxHealth());
			return GenerateGradient(ColourRed, ColourGreen, health / (float)entity->GetMaxHealth());
		}
	}
	else {
		switch (ActiveContainer->VisibleChamColour) {
		case ChamColourType::Static:
			return ActiveContainer->VisibleChamStaticColour;
		case ChamColourType::Team:
			return GetTeamColor(entity);
		default:
		case ChamColourType::Health:
			float health = std::clamp(entity->GetHealth(), 0, entity->GetMaxHealth());
			return GenerateGradient(ColourRed, ColourGreen, health / (float)entity->GetMaxHealth());
		}
	}
}
Colour Chams::GetChamColor(bool invisible, CItem* pickup) {
	if (invisible) {
		switch (PickupChams.InvisibleChamColour) {
		default:
		case ChamColourType::Static:
			return PickupChams.InvisibleChamStaticColour;
		case ChamColourType::Auto:
			if (pickup->GetType() == 1)
				return Colour(250, 200, 50);
			return Colour(40, 175, 100);
		}
	}
	else {
		switch (PickupChams.VisibleChamColour) {
		default:
		case ChamColourType::Static:
			return PickupChams.VisibleChamStaticColour;
		case ChamColourType::Auto:
			if (pickup->GetType() == 1)
				return Colour(250, 200, 50);
			return Colour(40, 175, 100);
		}
	}
}

void Chams::SetCham(CBaseEntity* entity, bool invisible) {
	if (invisible) {
		if (ActiveContainer->InvisibleChamsEnabled) {
			switch (ActiveContainer->InvisibleChamType) {
			default:
			case ChamType::Flat:
				flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				SetCham(flat, GetChamColor(true, (CBaseEntity*)entity), ActiveContainer->InvisibleChamOpacity);
				break;
			case ChamType::Shaded:
				shaded->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				SetCham(shaded, GetChamColor(true, (CBaseEntity*)entity), ActiveContainer->InvisibleChamOpacity);
				break;
			case ChamType::Wireframe:
				wireframe->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				SetCham(wireframe, GetChamColor(true, (CBaseEntity*)entity), ActiveContainer->InvisibleChamOpacity);
				break;
			case ChamType::Shiny:
				shiny->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				SetCham(shiny, GetChamColor(true, (CBaseEntity*)entity), ActiveContainer->InvisibleChamOpacity);
				break;
			}
			entity->DrawModel(0x1);
		}
	}
	else {
		if(ActiveContainer->VisibleChamsEnabled) {
			switch (ActiveContainer->VisibleChamType) {
			default:
			case ChamType::Flat:
				flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				SetCham(flat, GetChamColor(false, (CBaseEntity*)entity), ActiveContainer->VisibleChamOpacity);
				break;
			case ChamType::Shaded:
				shaded->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				SetCham(shaded, GetChamColor(false, (CBaseEntity*)entity), ActiveContainer->VisibleChamOpacity);
				break;
			case ChamType::Wireframe:
				wireframe->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				SetCham(wireframe, GetChamColor(false, (CBaseEntity*)entity), ActiveContainer->VisibleChamOpacity);
				break;
			case ChamType::Shiny:
				shiny->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				SetCham(shiny, GetChamColor(false, (CBaseEntity*)entity), ActiveContainer->VisibleChamOpacity);
				break;
			}
			entity->DrawModel(0x1);
		}
	}
}


void Chams::DrawPlayer(CBasePlayer* player) {
	ActiveContainer = &PlayerChams;
	DrawingChams = true;
	if (PlayerChams.InvisibleChamsEnabled) {
		SetCham(player, true);
		if (player->GetActiveWeapon())
			player->GetActiveWeapon()->DrawModel(0x1);
		ResetCham();
	}
	if (PlayerChams.VisibleChamsEnabled) {
		SetCham(player, false);
		if (player->GetActiveWeapon())
			player->GetActiveWeapon()->DrawModel(0x1);
		ResetCham();
	}
	DrawingChams = false;
}
void Chams::DrawPickup(CItem* pickup) {
	ActiveContainer = &PickupChams;
	DrawingChams = true;
	if (PickupChams.InvisibleChamsEnabled) {
		SetCham(pickup, true);
		ResetCham();
	}
	if (PickupChams.VisibleChamsEnabled) {
		SetCham(pickup, false);
		ResetCham();
	}
	DrawingChams = false;
}


void Chams::DrawBuilding(CBuilding* building) {
	ActiveContainer = &BuildingChams;
	DrawingChams = true;
	if (BuildingChams.InvisibleChamsEnabled) {
		SetCham(building, true);
		ResetCham();
	}
	if (BuildingChams.VisibleChamsEnabled) {
		SetCham(building, false);
		ResetCham();
	}
	DrawingChams = false;
}
void Chams::SceneEnd()
{
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		InitializeMaterials();
	}
	if (!Enabled)
		return;
	RenderGroups* renderGroups = (RenderGroups*)CheatModules["RenderGroups"];
	CBasePlayer* localPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	for (int i = 1; i <= CEntityList::factory()->GetHighestEntityIndex(); i++) {
		RenderGroup* renderGroup = renderGroups->FindMatchingRenderGroup(CEntityList::factory()->GetClientEntity(i));
		if (renderGroup) {
			ActiveContainer = &renderGroup->ChamsContainer;
			DrawingChams = true;
			if (renderGroup->ChamsContainer.InvisibleChamsEnabled) {
				SetCham(CEntityList::factory()->GetClientEntity(i), true);
				if (CEntityList::factory()->GetClientPlayer(i)->IsPlayer() && CEntityList::factory()->GetClientPlayer(i)->GetActiveWeapon() != NULL)
					CEntityList::factory()->GetClientPlayer(i)->GetActiveWeapon()->DrawModel(0x1);
				ResetCham();
			}
			if (renderGroup->ChamsContainer.VisibleChamsEnabled) {
				SetCham(CEntityList::factory()->GetClientEntity(i), false);
				if (CEntityList::factory()->GetClientPlayer(i)->IsPlayer() && CEntityList::factory()->GetClientPlayer(i)->GetActiveWeapon() != NULL)
					CEntityList::factory()->GetClientPlayer(i)->GetActiveWeapon()->DrawModel(0x1);
				ResetCham();
			}
			DrawingChams = false;
		}
	}
	/*for (int i = 1; i <= CEngineClient::factory()->GetMaxClients(); i++) {
		CBasePlayer* player = CEntityList::factory()->GetClientPlayer(i);
		if (!player || player->GetLifeState() == 2 || player == localPlayer)
			continue;
		if ((IgnoreTeammates && player->GetTeamNum() == localPlayer->GetTeamNum()) || (IgnoreDormant && player->IsDormant()))
			continue;
		DrawPlayer(CEntityList::factory()->GetClientPlayer(i));
	}


	for (int i = CEngineClient::factory()->GetMaxClients(); i <= CEntityList::factory()->GetHighestEntityIndex() ; i++) {
		CBaseEntity* entity = CEntityList::factory()->GetClientEntity(i);
		if (!entity)
			continue;
		if ((IgnoreTeammates && entity->GetTeamNum() == localPlayer->GetTeamNum()) || (IgnoreDormant && entity->IsDormant()))
			continue;

		if (entity->IsPickup()) {
			DrawPickup((CItem*)entity);
		}
		if (entity->IsBuilding()) {
			DrawBuilding((CBuilding*)entity);
		}
	}*/

}

void Chams::RenderMenu()
{
	if (ImGui::BeginTabBar("ChamsMainMenu")) {
		if (ImGui::BeginTabItem("Main")) {
			ImGui::Checkbox("Enabled", &Enabled);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Ignores")) {
			ImGui::Checkbox("Ignore Teammates", &IgnoreTeammates);
			ImGui::Checkbox("Ignore Dormant", &IgnoreDormant);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Player Chams")) {
			if (ImGui::BeginTabBar("PlayerChams")) {
				PlayerChams.RenderMenu();
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}		
		if (ImGui::BeginTabItem("Pickup Chams")) {
			if (ImGui::BeginTabBar("PickupChams")) {
				PickupChams.RenderMenu();
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Building Chams")) {
			if (ImGui::BeginTabBar("BuildingChams")) {
				BuildingChams.RenderMenu();
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

Chams g_Chams;

void ChamContainer::RenderMenu()
{
	if (ImGui::BeginTabItem("Invisible")) {
		if (ImGui::Checkbox("Enabled", &InvisibleChamsEnabled)) {
			ImGui::Combo("Invisible chams type", (int*)&InvisibleChamType, "Shiny\0Wallhack\0Flat\0Shaded\0Wireframe\0\0");

			ImGui::Combo("Invisible chams colour", (int*)&InvisibleChamColour, "Static\0Team\0Health\0Auto\0\0");
			ImGui::SliderFloat("Invisible chams opacity", &InvisibleChamOpacity, 0, 1);
			if (InvisibleChamColour == ChamColourType::Static) {
				ImVec4 static_invisible_cham_colour(InvisibleChamStaticColour);
				ImGui::ColorEdit3("Static colour", (float*)&static_invisible_cham_colour, ImGuiColorEditFlags_NoInputs);
				InvisibleChamStaticColour = static_invisible_cham_colour;
			}

		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Visible")) {
		if (ImGui::Checkbox("Enabled", &VisibleChamsEnabled)) {
			ImGui::Combo("Visible chams type", (int*)&VisibleChamType, "Shiny\0Wallhack\0Flat\0Shaded\0Wireframe\0\0");

			ImGui::Combo("Visible chams colour", (int*)&VisibleChamColour, "Static\0Team\0Health\0Auto\0\0");
			ImGui::SliderFloat("Visible chams opacity", &VisibleChamOpacity, 0, 1);
			if (VisibleChamColour == ChamColourType::Static) {
				ImVec4 static_Visible_cham_colour(VisibleChamStaticColour);
				ImGui::ColorEdit3("Static colour", (float*)&static_Visible_cham_colour, ImGuiColorEditFlags_NoInputs);
				VisibleChamStaticColour = static_Visible_cham_colour;
			}

		}
		ImGui::EndTabItem();
	}
}
