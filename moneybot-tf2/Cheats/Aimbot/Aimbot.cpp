#define NOMINMAX

#include "../../Misc/Math.h"
#include "Backtracking/LagRecords/LagRecords.h"
#include "Backtracking/LagRecords/LagRecords.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../SourceSDK/Interfaces/IGameMovement/IGameMovement.h"
#include "../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../SourceSDK/Interfaces/IEngineTrace/IEngineTrace.h"
#include "../CheatList.h"
#include "../../Misc/MoveFix.h"
#include "Aimbot.h"
#include "../imgui/imgui.h"
#include "Backtracking/Backtrack.h"
#include "../../Renderer.h"
#include "../Miscellaneous/Console/Console.h"
#include "../../SourceSDK/Interfaces/IVDebugOverlay/IVDebugOverlay.h"



std::vector<Aimbot::AimbotTarget> AimbotTargets;
Aimbot::Aimbot() {
	CheatModules["RageBot"] = this;
	CanRender = true;
}

void Aimbot::RenderMenu()
{
	if (ImGui::Checkbox("Enabled", &AimbotEnabled)) {
		ImGui::SliderFloat("Aimbot FOV", &AimbotFOV, 0.01f, 180);
		ImGui::Checkbox("Auto shoot", &AutoShoot);
		ImGui::Checkbox("Bullet Lines", &BulletNiggers);
	}
}

float Aimbot::GetFieldOfView(Vec3 angle, Vec3 source, Vec3 distance) {
	Vec3 ang, aim;
	float mag, uDotV;
	ang = Math::CalcAngle(source, distance);

	Math::MakeVector(angle, aim);
	Math::MakeVector(ang, ang);

	mag = sqrtf(pow(aim[0], 2) + pow(aim[1], 2) + pow(aim[2], 2));
	uDotV = aim.Dot(ang);

	return RAD2DEG(acos(uDotV / (pow(mag, 2))));
}

void Aimbot::AddTargets(CUserCmd_t* cmd) {
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	CBaseCombatWeapon* pWeapon = pLocalPlayer->GetActiveWeapon();

	Vec3 eyePos = pLocalPlayer->GetEyePos();

	AimbotTargets.clear();

	auto entityList = Groups::AllEntites();

	if (entityList.empty())
		return;

	for (CBasePlayer* pPlayer : entityList) {

		if (!pPlayer ||  pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum() || pPlayer == pLocalPlayer)
			continue;

		if (pPlayer->GetClientClass()->iClassID == CTFPlayer) {
		
			int hitbox = SelectHitbox(cmd, pPlayer);
			if (hitbox == -1)
				continue;

			AimbotTarget target{};

			target.pPlayer = pPlayer;
			target.vecPos  = pPlayer->GetHitboxPos(hitbox, true);
			target.iTargetType = HOSTILE;
			target.iHp = pPlayer->GetHealth();
			target.isMoving = (pPlayer->GetVelocity().Length2d() > 0.1f && (pPlayer->GetMoveType() == MOVETYPE_WALK));
			AimbotTargets.push_back(target);
		}

		if (pPlayer->GetClientClass()->iClassID == (CObjectSentrygun || CObjectDispenser || CObjectTeleporter)) {
			AimbotTarget building{};
			building.pPlayer = pPlayer;
			building.iTargetType = BULDING;
			building.vecPos = pPlayer->GetWorldSpaceCenter();
			building.iHp = pPlayer->GetHealth();
			building.isMoving = false; // wtf building was walking?
			AimbotTargets.push_back(building);
		}
	}
}

int Aimbot::SelectTarget(CUserCmd_t* cmd) {
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	int bestTargetIDX = -1;
	float desiredFov = AimbotFOV, maxFovDistance = 8192.0f, maxMeleeDistance = 8192.0f;
	Vec3 eyePos = pLocalPlayer->GetEyePos();
	Vec3 vecAimVector = cmd->ViewAngles.FromAngle();

	CBaseCombatWeapon* pWeapon = pLocalPlayer->GetActiveWeapon();
	if (!pWeapon)
		return -1;

	AddTargets(cmd);


	auto sortAllTargets = [](const AimbotTarget& a, const AimbotTarget& b) {
		
		// Prefer low hp
		if (a.iHp != b.iHp) {
			return a.iHp < b.iHp;
		}

		// Prefer moving
		if (a.isMoving != b.isMoving) {
			return a.isMoving > b.isMoving;
		}

		// Prefer closest
		if (a.flDistance != b.flDistance)
			return a.flDistance < b.flDistance;

		return a.pPlayer->GetIndex() > b.pPlayer->GetIndex();
	};

	if (AimbotTargets.empty())
		return -1;

	std::sort(AimbotTargets.begin(), AimbotTargets.end(), sortAllTargets);
	for (auto& Target : AimbotTargets) {

		auto aimDistance = [&](CBasePlayer* local, CBaseCombatWeapon* weapon) -> float {
			// Need more
			bool isDemo = weapon->GetItemDefinitionIndex() == Demoman_t_TheEyelander || weapon->GetItemDefinitionIndex() == Demoman_t_TheScotsmansSkullcutter;
			if (weapon->GetSlot() == 2 && !isDemo) { return 8.4f; }
			if (weapon->GetSlot() == 2 && isDemo) { return 12.0f; }
			if (local->GetClassId() == TF2_Pyro && weapon->GetSlot() == 0) { return 17.0f; }
			return 10000.0f;
		};

		float newDistance = aimDistance(pLocalPlayer, pWeapon);
		Vec3 eyeAngles = CEngineClient::factory()->GetViewAngles();

		Vec3 length = Target.vecPos - eyePos;
		float distance = ::sqrt(length.Length());
		if (distance < 1.0f)
			distance = 1.0f;

		float fovDistance = GetFieldOfView(eyeAngles, eyePos, Target.vecPos);

		if (fovDistance > desiredFov)
			continue;

		if (fovDistance < maxFovDistance && distance < newDistance) {
			maxFovDistance = fovDistance;
			newDistance = distance;
			bestTargetIDX = Target.pPlayer->GetIndex();
		}
		
	}
	return bestTargetIDX;
}

int Aimbot::SelectHitbox(CUserCmd_t* cmd, CBasePlayer* pPlayer) {

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	std::vector<int> hitboxes;

	hitboxes.push_back(HITBOX_HEAD);
	hitboxes.push_back(HITBOX_BODY);

	if (hitboxes.empty())
		return -1;

	int closestHitbox = -1;
	float closestFov = 300;
	Vec3 eyePos = pLocalPlayer->GetEyePos();
	Vec3 vecAimVector = cmd->ViewAngles.FromAngle();
	Vec3 eyeAngles = CEngineClient::factory()->GetViewAngles();
	for (int i =0 ; i < hitboxes.size(); i++){
		Vec3 vecTargetVector = pPlayer->GetHitboxPos(hitboxes[i], true) - eyePos;
		float fovRange = GetFieldOfView(eyeAngles, eyePos, pPlayer->GetHitboxPos(hitboxes[i], true));
		if (fovRange < closestFov || closestHitbox == -1) {
			closestHitbox = hitboxes[i];
			closestFov = fovRange;
		}	
	}

	return closestHitbox;
}

void Aimbot::AutoBackStab(CUserCmd* cmd) {
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	if (!pLocalPlayer || pLocalPlayer->GetLifeState() == LIFE_DEAD)
		return;

	if (pLocalPlayer->GetClassId() != TF2_Spy)
		return;

	CBaseCombatWeapon* pLocalWeapon = pLocalPlayer->GetActiveWeapon();
	if (!pLocalWeapon)
		return;

	CGameTrace tr;
	if (pLocalWeapon->DoSwingTrace(tr)) {

		// If we are in range to hit a player
		if (tr.m_pEntity && tr.m_pEntity->IsPlayer()) {

			CBasePlayer* pPlayer = reinterpret_cast<CBasePlayer*>(tr.m_pEntity);

			if (pPlayer && pPlayer->GetTeamNum() != pLocalPlayer->GetTeamNum()) {

				cmd->Buttons |= IN_ATTACK2;
			}
		}
	}	
}

Vec3 Aimbot::RunMultipoint(CBasePlayer* pPlayer, int curHitbox) {

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	
	auto* pModel = pPlayer->GetModel();
	if (!pModel)
		return Vec3{};

	studiohdr_t* hdr = reinterpret_cast<studiohdr_t*>(CModelInfo::factory()->GetStudioModel(pModel));
	if (!hdr)
		return Vec3{};

	matrix3x4 Matrix[128];
	if (!pPlayer->SetupBones(Matrix, 128, 0x100, GlobalVars()->CurTime))
		return Vec3{};

	mstudiohitboxset_t* boxSet = hdr->GetHitboxSet(pPlayer->GetHitboxSet());
	if (!boxSet)
		return Vec3{};

	mstudiobbox_t* box = boxSet->pHitbox(curHitbox);
	if (!box)
		return Vec3{};

	Vec3 shootPos = pLocalPlayer->GetEyePos();
	Vec3 hitboxCenter = (box->bbmax + box->bbmin) * 0.5f;
	Vec3 vecMin = box->bbmin;
	Vec3 vecMax = box->bbmax;
	Vec3 minDir{};
	Vec3 maxDir{};
	Vec3 angle{};
	Math::VectorAngles(hitboxCenter, vecMin);
	Math::VectorAngles(hitboxCenter, vecMax);
	Math::AngleVectors(vecMin, &minDir);
	Math::AngleVectors(vecMax, &maxDir);

	Vec3 min = hitboxCenter + minDir * 0.5f * 1.1f;
	Vec3 max = hitboxCenter + maxDir * 0.5f * 1.1f;

	std::array< Vec3, 8 > points = {
		Vec3{ min.x, min.y, min.z },
		Vec3{ min.x, max.y, min.z },
		Vec3{ max.x, max.y, min.z },
		Vec3{ max.x, min.y, min.z },
		Vec3{ max.x, max.y, max.z },
		Vec3{ min.x, max.y, max.z },
		Vec3{ min.x, min.y, max.z },
		Vec3{ max.x, min.y, max.z }
	};

	for (auto& it : points) {
		Vec3 transPoint{};
		Math::VectorTransform(it, Matrix[box->bone], transPoint);
		CTraceFilter filter;
		CGameTrace tr;
		Ray_t ray;
		ray.Init(shootPos, transPoint);

		// Might cause some fps issues not too sure
		IEngineTrace::factory()->TraceRay(ray, MASK_SHOT, &filter, &tr);

		if (tr.m_pEntity == pPlayer) {
			return transPoint;
		}
		
	}
	return pPlayer->GetHitboxPos(curHitbox);

}

void Aimbot::CreateMove(CUserCmd_t*& cmd) {
	if (!AimbotEnabled) return;

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	if (!pLocalPlayer)
		return;

	Vec3 vecShootPos = pLocalPlayer->GetEyePos();
	Vec3 vecAimVector = cmd->ViewAngles.FromAngle();
	float oSideMove = cmd->SideMove;
	float oForwardMove = cmd->ForwardMove;

	CBaseCombatWeapon* pLocalWeapon = pLocalPlayer->GetActiveWeapon();
	if (!pLocalWeapon)
		return;

	// Needs improvements
	AutoBackStab(cmd);

	int pTargetIndex = SelectTarget(cmd);
	if (pTargetIndex == -1)
		return;

	CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(pTargetIndex);
	// Make sure the player actually exists.
	if (pPlayer == nullptr)
		return;

	// Don't draw ESP on LocalPlayer
	if (pPlayer == pLocalPlayer)
		return;

	// Don't aimbot your team!!! (add a toggle for this)
	if (pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum())
		return;

	// Don't draw on dead or dormant players
	// TODO: Don't draw spectators ( once I figure out how to know if they are spectators )
	if (pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->IsDormant())
		return;

	int hitbox = SelectHitbox(cmd, pPlayer);
	if (hitbox == -1) 
		return;

	Vec3 aimPos = pPlayer->GetHitboxPos(hitbox, true);

	// Not tested
	aimPos = RunMultipoint(pPlayer, hitbox);

	if (!pPlayer->IsVisible(pLocalPlayer, vecShootPos, aimPos)) {

		// Maybe backtrack hitbox is visible idk if this fully works or not
		static LagRecords* lagRecords = (LagRecords*)CheatModules["LagRecords"];

		std::unique_lock<std::mutex> guard(LagRecordsCriticalSection);

		if (lagRecords->LagRecordsDeck[pPlayer->GetIndex()].empty() || lagRecords->LagRecordsDeck[pPlayer->GetIndex()].size() < 3)
			return;

		guard.unlock();
		static int lagRecordSize = lagRecords->LagRecordsDeck[pPlayer->GetIndex()].size() - 1;

		aimPos =  pPlayer->GetHitboxPos(lagRecords->LagRecordsDeck[pPlayer->GetIndex()].at(lagRecordSize).hitbox, true);

		if (!pPlayer->IsVisible(pLocalPlayer, vecShootPos, aimPos))
			return;
	}

	Vec3 vecPosition = aimPos;
	Math::VectorAngles(vecShootPos, vecPosition);
	vecPosition -= pLocalPlayer->GetVecPunchAngle() * 2.f;
	cmd->ViewAngles =  vecPosition.Clamp();

	ConVar* InterpRatio = ICvar::factory()->FindVar("cl_interp_ratio");
	ConVar* Interp = ICvar::factory()->FindVar("cl_interp");
	ConVar* UpdateRate = ICvar::factory()->FindVar("cl_updaterate");
	// Temporary before actual sprite bullet tracers
	auto GetLineColor = [&](){
		switch (hitbox) {
		case HITBOX_HEAD:
			return Colour(76, 247, 113);
		case HITBOX_BODY:
			return Colour(247, 193, 76);
		default:
			return Colour(255, 0, 0);
		}
	};

	if (pLocalWeapon->IsReadyToFire()) {
		if (AutoShoot)
			cmd->Buttons |= IN_ATTACK;

		if (cmd->Buttons & IN_ATTACK) {
			cmd->TickCount = TIME_TO_TICKS(pPlayer->GetSimulationTime() + std::max(Interp->GetFloat(), InterpRatio->GetFloat() / UpdateRate->GetFloat()));

			if (BulletNiggers) {
				Colour lineColor = GetLineColor();
				IVDebugOverlay::factory()->AddLineOverlayAlpha(vecShootPos, aimPos, lineColor.R, lineColor.G, lineColor.B, 100, true, 5.f);
			}
		}
	}
}

Aimbot g_Aimbot;



