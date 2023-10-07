#pragma once

#include "../BaseCheat.h"
#include "../../SourceSDK/Misc/Vector3.h"
#include "../../SourceSDK/Entities/CBuilding/CBuilding.h"
#include "../../imgui/imgui.h"
#include "../../Misc/Colour.h"
#include "../../json.h"

class CBasePlayer;




enum struct HealthBarType {
	Gradient,
	FullGradient,
	Static,
};
enum BoundingBoxType {
	_2D,
	_3D,
};
enum struct BoundingBoxColourType {
	Static,
	Team,
	Health,
};
NLOHMANN_JSON_SERIALIZE_ENUM(HealthBarType, { {HealthBarType::Gradient, "Gradient"}, {HealthBarType::FullGradient, "FullGradient"}, {HealthBarType::Static, "Static"} });
NLOHMANN_JSON_SERIALIZE_ENUM(BoundingBoxType, { {BoundingBoxType::_2D, "2D"}, {BoundingBoxType::_3D, "3D"} });
NLOHMANN_JSON_SERIALIZE_ENUM(BoundingBoxColourType, { {BoundingBoxColourType::Static, "Static"}, {BoundingBoxColourType::Team, "Team"}, {BoundingBoxColourType::Health, "Health"} });

class ESPContainer { 
public:
	bool Enabled;
	bool HealthBarEnabled;
	HealthBarType			  HealthBar;
	Colour		  HealthBarStaticColour;
	Colour HealthBarGradientStartColour;
	Colour		HealthBarGradientEndColour;
	bool	  BoundingBoxEnabled;
	BoundingBoxType   BoundingBox;
	bool      BoundingBoxOutline;
	BoundingBoxColourType BoundingBoxColour;
	Colour BoundingBoxStaticColour; 
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ESPContainer, Enabled, HealthBarEnabled, HealthBar, HealthBarStaticColour, HealthBarGradientStartColour, HealthBarGradientEndColour, BoundingBoxEnabled, BoundingBox, BoundingBoxOutline, BoundingBoxColour, BoundingBoxStaticColour);

	void RenderMenu();
};


struct ESPSettings {
    bool enabled;
    bool box;
    bool health;
    bool weapon;
    bool name;
};
class ESP : public BaseCheat {
public:

    std::deque<ESPSettings> Settings[256];
    bool Enabled;
    bool IgnoreTeammates;
    bool IgnoreDormants;
    ESPContainer PlayerContainer;
    ESPContainer PickupContainer;
    ESPContainer BuildingContainer;
    ESP();
    void Render() override;
    void RenderMenu() override;
    void LoadConfig() override;
    void SaveConfig() override;

private:
    void UpdateSettings(CBasePlayer* pPlayer);
    void DrawPlayer(CBasePlayer* pPlayer);
    void DrawBuilding(CBuilding* pPlayer);
    Vec3 GetRightAngle(Vec3, Vec3);
    Colour GetTeamColor(CBaseEntity* pPlayer);
    void DrawEntity(CBaseEntity* entity);
    float dormantProgress[65]{};
};
