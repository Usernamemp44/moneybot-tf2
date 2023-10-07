#include "Triggerbot.h"

#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"

#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/IGameMovement/IGameMovement.h"
#include "../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../../SourceSDK/Interfaces/IEngineTrace/IEngineTrace.h"
#include "../../CheatList.h"
#include "../../../Misc/Math.h"
#include "../../../SourceSDK/Interfaces/CModelInfo/CModelInfo.h"
#include "../../../imgui/imgui.h"

#include "../../../SourceSDK/Interfaces/IVDebugOverlay/IVDebugOverlay.h"

#include  "../Backtracking/Backtrack.h"
#include "../Backtracking/LagRecords/LagRecords.h"
Triggerbot::Triggerbot()
{
	CheatModules["Triggerbot"] = this;
	CanRender = true;
	//ShouldPredict = true;
}

CBaseEntity* Triggerbot::GetEntityOnCrosshair(Vec3 viewangles) {
	LagRecords* lagRecords = (LagRecords*)CheatModules["LagRecords"];

	Ray_t ray;
	CTraceFilter filter;
	CGameTrace trace;

	CBasePlayer* localPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	Vec3 forward;
	Math::AngleVectors(viewangles, &forward);
	forward = forward * MaximumDistance + localPlayer->GetEyePos();
	ray.Init(localPlayer->GetEyePos(), forward);


	// I WILL LITERALLY KILL YOU WHY DID YOU USE MASK_SHOT_HULL YOU FUCKING noob I SPENT LIKE AN HOUR LOOKING AT THIS SHIT FUCK YOU
	IEngineTrace::factory()->TraceRay(ray, MASK_SHOT, &filter, &trace);

	//if (trace.m_pEntity != nullptr && trace.m_pEntity->IsPlayer())
	//	IVDebugOverlay::factory()->AddLineOverlay(localPlayer->GetEyePos(), trace.endpos, 0, 255, 0, true, 1.5f);
	//else
	//	IVDebugOverlay::factory()->AddLineOverlay(localPlayer->GetEyePos(), trace.endpos, 255, 0, 0, true, 0.5f);

	return trace.m_pEntity;
}

void Triggerbot::CreateMove(CUserCmd_t*& cmd) {

	if (!Enabled)
		return;
	if (!GetAsyncKeyState(Key))
		return; //really shouldn't be using getasynckeystate, we have a wndproc hook
	if (cmd->Buttons & IN_ATTACK)
		return;

	CBasePlayer* localPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	if (!localPlayer)
		return;
	if (!localPlayer->GetLifeState() == LIFE_ALIVE)
		return;

	if (!localPlayer->GetActiveWeapon())
		return;

	if (ScopedOnly)
		if (localPlayer->GetClassNum() == TF2_Sniper)
			if (!(localPlayer->GetCond() & TFCond_Zoomed))
				return;

	//IVDebugOverlay::factory();

	CBaseEntity* lookingAt = GetEntityOnCrosshair(cmd->ViewAngles);
	LagRecords* lagRecords = (LagRecords*)CheatModules["LagRecords"];
	Vec3 OldAbsAngles;
	Vec3 OldAbsOrigin;
	int iTickCount = cmd->TickCount;
	BestTickResult result;
	result.success = false;
	for (int tick = 0; tick <= 1; tick++) {
		if (tick != 0) { // don't do it on the first tick
			LegitBacktrack* LegitBT = (LegitBacktrack*)CheatModules["LagRecords"];
			result = LegitBT->GetBestPlayer(cmd);

			// TODO: I SHOULD PROBABLY SET THESE BACK
			if (result.success) {
				OldAbsAngles = result.BestPlayer->GetAbsAngles();
				OldAbsOrigin = result.BestPlayer->GetAbsOrigin();
				result.BestPlayer->SetAbsAngles(result.BestRecord.absangles);
				result.BestPlayer->SetAbsOrigin(result.BestRecord.absorigin);
				result.BestPlayer->InvalidateBoneCache();
				iTickCount = result.BestRecord.tickcount;
				//IVDebugOverlay::factory()->AddBoxOverlay(result.BestRecord.absorigin,
				//	Vector3(-5, -5, -5), Vector3(5, 5, 5), Vector3(0, 0, 0), 255, 0, 0, 255, .2f);
			}
			else {
				continue;
			}
		}

		lookingAt = GetEntityOnCrosshair(cmd->ViewAngles);
		if (result.success) {
			result.BestPlayer->SetAbsAngles(OldAbsAngles);
			result.BestPlayer->SetAbsOrigin(OldAbsOrigin);
		}
		if (!lookingAt)
			continue; //find backtrack
		if (lookingAt->IsPlayer()) {
			cmd->Buttons |= IN_ATTACK;
		}
		if (lookingAt->IsBuilding() && (!IgnoreBuildings)) {
			cmd->Buttons |= IN_ATTACK;	
		}
		if (lookingAt->GetTeamNum() == localPlayer->GetTeamNum() && (IgnoreTeammates)) {
			cmd->Buttons &= ~IN_ATTACK;
		}

		if (lookingAt->IsPlayer()) {
			CBasePlayer* lookingAtPlayer = (CBasePlayer*)lookingAt;
			if ((lookingAtPlayer->GetCond() & TFCond_Disguised) && IgnoreDisguised)
				cmd->Buttons &= ~IN_ATTACK;
			if ((lookingAtPlayer->GetCond() & TFCond_Cloaked) && IgnoreCloaked)
				cmd->Buttons &= ~IN_ATTACK;
			if ((lookingAtPlayer->GetCond() & TFCond_Ubercharged) && IgnoreInvulnerable)
				cmd->Buttons &= ~IN_ATTACK;
		}

		if (cmd->Buttons & IN_ATTACK) {
			cmd->TickCount = iTickCount;
			break;
		}
	}

	if (!lookingAt)
		return;


	// BUG: THIS CAUSES ONLY THE CURRENT AND LAST BT TO BE TARGETED AND IS BORKED ):
	//if (cmd->Buttons & IN_ATTACK) {
	//	cmd->Buttons &= ~IN_ATTACK;
	//	matrix3x4 bones[256];
	//	CBasePlayer* lookingAtPlayer = (CBasePlayer*)lookingAt;
	//	lookingAtPlayer->GetBoneMatrix(bones, 256);

	//	DWORD *pModel = lookingAtPlayer->GetModel();

	//	studiohdr_t *pHdr = (studiohdr_t *)CModelInfo::factory()->GetStudioModel(pModel);


	//	mstudiohitboxset_t *pSet = pHdr->GetHitboxSet(lookingAtPlayer->GetHitboxSet());
	//	Vec3 end;
	//	Math::AngleVectors(cmd->ViewAngles, &end);
	//	end = end * MaximumDistance + localPlayer->GetEyePos();
	//	for (int i = 0; i < pSet->numhitboxes; i++) {
	//		if (Hitgroup) {
	//			if (Hitgroup == 1) 
	//				if (i != HITBOX_HEAD)
	//					continue;
	//		
	//			if (Hitgroup == 2)
	//				if (i == HITBOX_HEAD)
	//					continue;
	//		}
	//		mstudiobbox_t *pBox = pSet->pHitbox(i);
	//		Vec3 min, max;
	//		min = pBox->bbmin * 0.8;
	//		max = pBox->bbmax * 0.8;
	//		if (IEngineTrace::IntersectRayWithOBB(localPlayer->GetEyePos(), end - localPlayer->GetEyePos(), bones[pBox->bone], min, max)) {
	//			cmd->Buttons |= IN_ATTACK;
	//		}
	//	}
	//}
}

void Triggerbot::RenderMenu() {
	ImGui::BeginTabBar("Triggerbot");

	if (ImGui::BeginTabItem("Main")) {
		if (ImGui::Checkbox("Enabled", &Enabled)) {
			ImGui::Keybind("Triggerkey", &Key);
			ImGui::SliderFloat("Maximum distance", &MaximumDistance, 0, 1000);
			ImGui::SliderFloat("Scale factor", &ScaleFactor, 0, 1);
			ImGui::Combo("Hitgroup", &Hitgroup, "Any\0Head\0Body\0\0");
			ImGui::Checkbox("Backstab only", &BackstabOnly);
			ImGui::Checkbox("Scoped only", &ScopedOnly);
		}
		ImGui::EndTabItem();
	}
	if (Enabled) {
		if (ImGui::BeginTabItem("Ignores")) {

			ImGui::Checkbox("Ignore Buildings", &IgnoreBuildings);
			ImGui::Checkbox("Ignore Teammates", &IgnoreTeammates);
			ImGui::Checkbox("Ignore Dormants", &IgnoreDormants);
			ImGui::Checkbox("Ignore Disguised", &IgnoreDisguised);
			ImGui::Checkbox("Ignore Cloaked", &IgnoreCloaked);
			ImGui::Checkbox("Ignore Invulnerable", &IgnoreInvulnerable);

			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
}

Triggerbot g_Triggerbot;