#pragma once
#include "../BaseCheat.h"
#include "../../SourceSDK/Misc/Defines.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../SourceSDK/Entities/CItem/CItem.h"
#include "../../SourceSDK/Entities/CBuilding/CBuilding.h"
#include "../../SourceSDK/Interfaces/CModelInfo/CModelInfo.h"
#include "../../SourceSDK/Interfaces/CModelRender/CModelRender.h"
#include "../../SourceSDK/Interfaces/IMaterialSystem/IMaterialSystem.h"
#include "../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../SourceSDK/Interfaces/CRenderView/CRenderView.h"
#include "../../SourceSDK/Interfaces/CKeyValues/CKeyValues.h"
#include "../../imgui/imgui.h"
#include "../CheatList.h"
#include "../../Misc/Colour.h"

#include "../../json.h"
using json = nlohmann::json;
enum struct ChamType {
	Shiny,
	Wallhack,
	Flat,
	Shaded,
	Wireframe,
};
enum struct ChamColourType {
	Static,
	Team,
	Health,
	Auto,
};


class ChamContainer {
public:
	bool InvisibleChamsEnabled;
	ChamType InvisibleChamType;
	ChamColourType InvisibleChamColour;
	Colour InvisibleChamStaticColour;
	float InvisibleChamOpacity;
	bool VisibleChamsEnabled;
	ChamType VisibleChamType;
	ChamColourType VisibleChamColour;
	Colour VisibleChamStaticColour;
	float VisibleChamOpacity;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ChamContainer, InvisibleChamsEnabled, InvisibleChamType, InvisibleChamColour, InvisibleChamStaticColour, VisibleChamsEnabled, VisibleChamType, VisibleChamColour, VisibleChamStaticColour, VisibleChamOpacity, InvisibleChamOpacity);
	virtual void RenderMenu();
};





class Chams  : public BaseCheat{
public:
	
	ChamContainer* ActiveContainer;
	
	bool Enabled = true;
	Chams();

	void LoadConfig() override;

	void SaveConfig() override;

	bool IgnoreTeammates;
	bool IgnoreDormant;

	ChamContainer PlayerChams;
	ChamContainer PickupChams;
	ChamContainer BuildingChams;

	bool DrawingChams = false;
	IMaterial* flat;
	IMaterial* shaded;
	IMaterial* wireframe;
	IMaterial* shiny;

	IMaterial * CreateMaterial(bool ignorez, bool flat, bool wireframe, bool add_shine);
	IMaterial * CreateShinyMaterial();
	void SetCham(IMaterial * material, Colour clr, float alpha = 1.0f);
	void ResetCham();
	void InitializeMaterials();
	Colour GetTeamColor(CBaseEntity * pPlayer); 
	Colour GetChamColor(bool invisible, CBaseEntity* entity);
	Colour GetChamColor(bool invisible, CItem * pickup);
	void SetCham(CBaseEntity * entity, bool invisible);
	void DrawPlayer(CBasePlayer * player);
	void DrawPickup(CItem * pickup);
	void DrawBuilding(CBuilding * building);
	//bool DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &info, matrix3x4 *matrix) override;
	void SceneEnd() override;
	void RenderMenu() override;
};