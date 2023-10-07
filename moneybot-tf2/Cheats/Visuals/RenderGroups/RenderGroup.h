#pragma once
#include <map>
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../ESP.h"
#include "../Chams.h"
#include "../../../magic_enum.h"
enum EntityType {
	PLAYER,
	SENTRY,
	DISPENSER,
	TELEPORTER,
	HEALTH,
	AMMO,
	PROJECTILE,
};

enum Condition {
	LOCAL_PLAYER,
	FRIENDLY,
	HOSTILE,
	CLOAKED,
	DISGUISED,
	DORMANT,
	STEAM_FRIEND,
	ALIVE,

};

class RenderGroup {
public:
	std::string Name = "";
	int AllowedEntityTypes = 0;
	int Conditions = 0; 
	bool MatchAllOrOne = true;
	void AddEntityType(EntityType entityType);
	void RemoveEntityType(EntityType entityType);
	bool ValidEntityType(EntityType type);

	bool ValidCondition(Condition cond);
	void AddCondition(Condition cond);
	void RemoveCondition(Condition cond);
	bool DoesFit(CBaseEntity * entity);
	ESPContainer EspContainer;
	ChamContainer ChamsContainer;
	//NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenderGroup, Name, AllowedEntityTypes, Conditions, EspContainer, ChamsContainer);
};