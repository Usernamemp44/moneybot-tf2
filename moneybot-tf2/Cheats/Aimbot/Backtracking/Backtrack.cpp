#include "Backtrack.h"
#include "LagRecords/LagRecords.h"
#include "../../Visuals/Chams.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"

#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/IGameMovement/IGameMovement.h"
#include "../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/CModelInfo/CModelInfo.h"
#include "../../../SourceSDK/Interfaces/IVDebugOverlay/IVDebugOverlay.h"

#include "../../../SourceSDK/Misc/Defines.h"
#include "../../../Misc/Math.h"
#include "../../CheatList.h"
#include "../../Miscellaneous/Console/Console.h"

LegitBacktrack::LegitBacktrack() {
	CheatModules["LegitBacktrack"] = this;
	CanRender = true;
}

BestTickResult LegitBacktrack::GetBestPlayer(CUserCmd_t*& cmd) {
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	Vec3 vecEyePos = pLocalPlayer->GetEyePos();

	BestTickResult result;
	result.success = false;

	int iBestTick = 0;
	float fBestAngleDelta = FLT_MAX;

	LagRecords* lagRecords = (LagRecords*)CheatModules["LagRecords"];

	for (int ply = 1; ply <= GlobalVars()->MaxClients; ply++) {
		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(ply);

		// Make sure the player actually exists.
		if (pPlayer == nullptr)
			continue;

		// Don't do shit to myself wtf!!!
		if (pPlayer == pLocalPlayer)
			continue;

		if (pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->IsDormant())
			continue;

		if (pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum())
			continue;


		std::deque playerRecords = lagRecords->LagRecordsDeck[ply];

		if (playerRecords.size() == 0)
			continue;

		for (int record = playerRecords.size() - 1; record >= 0; record--) {
			//if (!lagRecords->IsTickValid(playerRecords[record].tickcount, cmd->TickCount))
			//	break;

			DWORD *pModel = pPlayer->GetModel();
			studiohdr_t *pHdr = (studiohdr_t *)CModelInfo::factory()->GetStudioModel(pModel);

			mstudiohitboxset_t *pSet = pHdr->GetHitboxSet(pPlayer->GetHitboxSet());
			mstudiobbox_t *pBox = pSet->pHitbox(PlayerHitboxes::HITBOX_CHEST);
			Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f;
			Vec3 CHEST;
			Math::VectorTransform(vPos, playerRecords[record].bonematrix[pBox->bone], CHEST);

			Vec3 vecAimAng = (CHEST - vecEyePos).ToAngle();
			float fAvgAimDelta = (fabs(vecAimAng.y - cmd->ViewAngles.y) + fabs(vecAimAng.y - cmd->ViewAngles.y)) / 2.f;

			if (fAvgAimDelta < fBestAngleDelta) {
				fBestAngleDelta = fAvgAimDelta;
				result.TickCount = playerRecords[record].tickcount;
				result.BestRecord = playerRecords[record];
				result.BestPlayer = pPlayer;

				result.success = true;
			}
		}
	}

	//IVDebugOverlay::factory()->AddLineOverlay(vecEyePos, result.BestRecord.absorigin, 0, 255, 0, true, 1.5f);
	return result;
}

void LegitBacktrack::CreateMove(CUserCmd_t*& cmd) {
	if (!Enabled || !(cmd->Buttons & IN_ATTACK))
		return;

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	CBaseCombatWeapon* pActiveWeapon = pLocalPlayer->GetActiveWeapon();

	if (pActiveWeapon && pActiveWeapon->GetDamageType() == DMG_BLAST)
		return;

	BestTickResult result = GetBestPlayer(cmd);
	
	if (result.success) {
		cmd->TickCount = result.TickCount;
	}
}

void LegitBacktrack::DrawPlayer(CBasePlayer* player) {
	Chams* chams = (Chams*)CheatModules["Chams"];
	chams->DrawingChams = true;

	chams->ActiveContainer = &BacktrackChams;

	if (BacktrackChams.InvisibleChamsEnabled) {
		chams->SetCham(player, true);
		chams->ResetCham();
	}
	if (BacktrackChams.VisibleChamsEnabled) {
		chams->SetCham(player, false);
		chams->ResetCham();
	}
	chams->DrawingChams = false;
}

void LegitBacktrack::SceneEnd() {
	if (!(BacktrackChams.VisibleChamsEnabled || BacktrackChams.InvisibleChamsEnabled))
		return;

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	LagRecords* lagRecords = (LagRecords*)CheatModules["LagRecords"];
	Chams* chams = (Chams*)CheatModules["Chams"];
	for (int ply = 1; ply <= GlobalVars()->MaxClients; ply++) {
		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(ply);

		std::deque playerRecords = lagRecords->LagRecordsDeck[ply];
		if (!pPlayer)
			continue;

		if (pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->IsDormant())
			continue;

		if (pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum())
			continue;

		if (playerRecords.size() == 0)
			continue;
		Vec3 oldAbsOrigin = pPlayer->GetAbsOrigin();
		Vec3 oldAbsAngles = pPlayer->GetAbsAngles(); 
		int record = 0;
		switch (LegitBacktrackCham) {

		case LegitBacktrackChamType::LastTickOnly:
		lasttickonly:
			pPlayer->SetAbsOrigin(playerRecords[playerRecords.size() - 1].absorigin);
			pPlayer->SetAbsAngles(playerRecords[playerRecords.size() - 1].absangles);
			pPlayer->InvalidateBoneCache();
			DrawPlayer(pPlayer);
			break;
		case LegitBacktrackChamType::BestTickOnly:
		/*	if (iBestRecord == -1)
				goto lasttickonly;
			
			pPlayer->SetAbsOrigin(playerRecords[iBestRecord].absorigin);
			pPlayer->SetAbsAngles(playerRecords[iBestRecord].absangles);
			pPlayer->InvalidateBoneCache();
			DrawPlayer(pPlayer);*/
			break;
		default:
			for ( record = playerRecords.size() - 1; record >= 0; record--) {
				if (record % 2 == 0)
					continue;
				if (Math::WorldToScreen(playerRecords[record].absorigin).x == -1)
					continue;
				pPlayer->SetAbsOrigin(playerRecords[record].absorigin);
				pPlayer->SetAbsAngles(playerRecords[record].absangles);
				pPlayer->InvalidateBoneCache();
				DrawPlayer(pPlayer);
			}
			break;
		}

		pPlayer->SetAbsOrigin(oldAbsOrigin);
		pPlayer->SetAbsAngles(oldAbsAngles);
	}
}
void LegitBacktrack::RenderMenu()
{
	if (ImGui::BeginTabBar("ChamsMainMenu")) {
		if (ImGui::BeginTabItem("Main")) {
			ImGui::Checkbox("Enabled", &Enabled);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Chams")) {
			if (ImGui::BeginTabBar("BacktrackChams")) {
				if (ImGui::BeginTabItem("Main")) {
					ImGui::Combo("Backtrack cham type", (int*)&LegitBacktrackCham, "Last Tick Only\0Best Tick Only\0All Ticks\0\0");
					ImGui::EndTabItem();
				}
				BacktrackChams.RenderMenu();
				ImGui::EndTabBar();
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}


}
LegitBacktrack g_LegitBacktrack;