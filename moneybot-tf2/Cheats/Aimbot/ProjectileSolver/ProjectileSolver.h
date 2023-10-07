#pragma once

#include "../../BaseCheat.h"
#include "../../../SourceSDK/Misc/Vector3.h"
#include "../../../imgui/imgui.h"
#include "../../../SourceSDK/Interfaces/ConVar/ConVar.h"


class CBasePlayer;
class CBaseCombatWeapon;
enum {
	SOLVER_LINEAR,
	SOLVER_ENGINE
};
class ProjectileSolver : public BaseCheat {
private:
	ConVar* Gravity;
public:
	bool Enabled = false;	
	ProjectileSolver();
	float GetWeaponProjectileGravity(CBaseCombatWeapon* weapon);
	float GetWeaponProjectileSpeed(CBaseCombatWeapon * weapon);
	Vec3 PredictEntityPosition(CBasePlayer* pPlayer);
	Vec3 SolveLinearPlayerPosition(CBasePlayer * player, float time);
	float CalculateTravelTime(CBasePlayer * player, float time);
	CUserCmd ConstructUserCommand(CBasePlayer * player);
	Vec3 SolveEnginePlayerPosition(CBasePlayer * player, float time);
	Vec3 SolvePlayerPosition(CBasePlayer * player, float time, uint8_t solverType);
	void Render() override;
	void RenderMenu() override;
	void CreateMove(CUserCmd*&) override;
};
