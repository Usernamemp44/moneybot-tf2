#include "ProjectileSolver.h"
#include "../../CheatList.h"

#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../../SourceSDK/Interfaces/IEngineTrace/IEngineTrace.h"

#include "../../../Renderer.h"
#include "../../../Misc/Colour.h"
#include "../../../Misc/Math.h"

#include "../../Miscellaneous/PlayerInformation/PlayerInformation.h"
#include "../../Prediction/EnginePrediction.h"


ProjectileSolver::ProjectileSolver()
{
	CheatModules["ProjectileSolver"] = this;
	Gravity = ICvar::factory()->FindVar("sv_gravity");
	this->CanRender = true;
	this->ShouldPredict = false;
}

float ProjectileSolver::GetWeaponProjectileGravity(CBaseCombatWeapon* weapon) {

	float out = 0.f;
	switch (weapon->GetItemDefinitionIndex())
	{
		case Soldier_m_RocketLauncher:
		case Soldier_m_RocketLauncherR:
		case Soldier_m_TheBlackBox:
		case Soldier_m_TheCowMangler5000:
		case Soldier_m_TheOriginal:
		case Soldier_m_FestiveRocketLauncher:
		case Soldier_m_TheBeggarsBazooka:
		case Soldier_m_SilverBotkillerRocketLauncherMkI:
		case Soldier_m_GoldBotkillerRocketLauncherMkI:
		case Soldier_m_RustBotkillerRocketLauncherMkI:
		case Soldier_m_BloodBotkillerRocketLauncherMkI:
		case Soldier_m_CarbonadoBotkillerRocketLauncherMkI:
		case Soldier_m_DiamondBotkillerRocketLauncherMkI:
		case Soldier_m_SilverBotkillerRocketLauncherMkII:
		case Soldier_m_GoldBotkillerRocketLauncherMkII:
		case Soldier_m_FestiveBlackBox:
		case Soldier_m_TheAirStrike:
		case Soldier_m_WoodlandWarrior:
		case Soldier_m_SandCannon:
		case Soldier_m_AmericanPastoral:
		case Soldier_m_SmalltownBringdown:
		case Soldier_m_ShellShocker:
		case Soldier_m_AquaMarine:
		case Soldier_m_Autumn:
		case Soldier_m_BlueMew:
		case Soldier_m_BrainCandy:
		case Soldier_m_CoffinNail:
		case Soldier_m_HighRollers:
		case Soldier_m_Warhawk: {
			out = 0.0f;
			break;
		}

		case Soldier_m_TheDirectHit: {
			out = 0.0f;
			break;
		}

		case Soldier_m_TheLibertyLauncher: {
			out = 0.0f;
			break;
		}

		case Demoman_m_GrenadeLauncher:
		case Demoman_m_GrenadeLauncherR:
		case Demoman_m_FestiveGrenadeLauncher:
		case Demoman_m_TheIronBomber:
		case Demoman_m_Autumn:
		case Demoman_m_MacabreWeb:
		case Demoman_m_Rainbow:
		case Demoman_m_SweetDreams:
		case Demoman_m_CoffinNail:
		case Demoman_m_TopShelf:
		case Demoman_m_Warhawk:
		case Demoman_m_ButcherBird: {
			out = 0.4f;
			break;
		}

		case Soldier_s_TheRighteousBison:
		case Engi_m_ThePomson6000: {
			out = 0.0f;
			break;
		}

		case Demoman_m_TheLooseCannon: {
			out = 0.4f;
			break;
		}

		case Demoman_m_TheLochnLoad: {
			out = 0.4f;
			break;
		}

		case Engi_m_TheRescueRanger:
		case Medic_m_FestiveCrusadersCrossbow:
		case Medic_m_CrusadersCrossbow: {
			out = 0.2f;
			break;
		}

		case Pyro_m_DragonsFury: {
			out = 0.0f;
			break;
		}

		case Pyro_m_FlameThrower:
		case Pyro_m_ThePhlogistinator:
		case Pyro_m_TheBackburner:
		case Pyro_m_TheDegreaser: {
			out = 0.0f;
			break;
		}

		case Pyro_s_TheDetonator:
		case Pyro_s_TheFlareGun:
		case Pyro_s_FestiveFlareGun:
		case Pyro_s_TheScorchShot: {
			out = 0.3f;
			break;
		}

		case Pyro_s_TheManmelter: {
			out = 0.2f;
			break;
		}

		case Medic_m_SyringeGun:
		case Medic_m_SyringeGunR:
		case Medic_m_TheBlutsauger:
		case Medic_m_TheOverdose: {
			out = 0.2f;
			break;
		}

		case Sniper_m_TheHuntsman:
		case Sniper_m_FestiveHuntsman:
		case Sniper_m_TheFortifiedCompound: {
			float charge = (GlobalVars()->CurTime - weapon->GetChargeTime());
			out = ((fminf(fmaxf(charge, 0.0f), 1.0f) * -0.4f) + 0.5f);
			break;
		}
	}

	return out;
}

float ProjectileSolver::GetWeaponProjectileSpeed(CBaseCombatWeapon* weapon) {
	float out = 0.f;
	switch (weapon->GetItemDefinitionIndex())
	{
	case Soldier_m_RocketLauncher:
	case Soldier_m_RocketLauncherR:
	case Soldier_m_TheBlackBox:
	case Soldier_m_TheCowMangler5000:
	case Soldier_m_TheOriginal:
	case Soldier_m_FestiveRocketLauncher:
	case Soldier_m_TheBeggarsBazooka:
	case Soldier_m_SilverBotkillerRocketLauncherMkI:
	case Soldier_m_GoldBotkillerRocketLauncherMkI:
	case Soldier_m_RustBotkillerRocketLauncherMkI:
	case Soldier_m_BloodBotkillerRocketLauncherMkI:
	case Soldier_m_CarbonadoBotkillerRocketLauncherMkI:
	case Soldier_m_DiamondBotkillerRocketLauncherMkI:
	case Soldier_m_SilverBotkillerRocketLauncherMkII:
	case Soldier_m_GoldBotkillerRocketLauncherMkII:
	case Soldier_m_FestiveBlackBox:
	case Soldier_m_TheAirStrike:
	case Soldier_m_WoodlandWarrior:
	case Soldier_m_SandCannon:
	case Soldier_m_AmericanPastoral:
	case Soldier_m_SmalltownBringdown:
	case Soldier_m_ShellShocker:
	case Soldier_m_AquaMarine:
	case Soldier_m_Autumn:
	case Soldier_m_BlueMew:
	case Soldier_m_BrainCandy:
	case Soldier_m_CoffinNail:
	case Soldier_m_HighRollers:
	case Soldier_m_Warhawk: {
		out = 1100.0f;
		break;
	}

	case Soldier_m_TheDirectHit: {
		out = 1980.0f;
		break;
	}

	case Soldier_m_TheLibertyLauncher: {
		out = 1540.0f;
		break;
	}

	case Demoman_m_GrenadeLauncher:
	case Demoman_m_GrenadeLauncherR:
	case Demoman_m_FestiveGrenadeLauncher:
	case Demoman_m_TheIronBomber:
	case Demoman_m_Autumn:
	case Demoman_m_MacabreWeb:
	case Demoman_m_Rainbow:
	case Demoman_m_SweetDreams:
	case Demoman_m_CoffinNail:
	case Demoman_m_TopShelf:
	case Demoman_m_Warhawk:
	case Demoman_m_ButcherBird: {
		out = 1217.0f;
		break;
	}

	case Soldier_s_TheRighteousBison:
	case Engi_m_ThePomson6000: {
		out = 1200.0f;
		break;
	}

	case Demoman_m_TheLooseCannon: {
		out = 1453.9f;
		break;
	}

	case Demoman_m_TheLochnLoad: {
		out = 1513.3f;
		break;
	}

	case Engi_m_TheRescueRanger:
	case Medic_m_FestiveCrusadersCrossbow:
	case Medic_m_CrusadersCrossbow: {
		out = 2400.0f;
		break;
	}

	case Pyro_m_DragonsFury: {
		out = 3000.0f;
		break;
	}

	case Pyro_m_FlameThrower:
	case Pyro_m_ThePhlogistinator:
	case Pyro_m_TheBackburner:
	case Pyro_m_TheDegreaser: {
		out = 1000.0f;
		break;
	}

	case Pyro_s_TheDetonator:
	case Pyro_s_TheFlareGun:
	case Pyro_s_FestiveFlareGun:
	case Pyro_s_TheScorchShot: {
		out = 2000.0f;
		break;
	}

	case Pyro_s_TheManmelter: {
		out = 3000.0f;
		break;
	}

	case Medic_m_SyringeGun:
	case Medic_m_SyringeGunR:
	case Medic_m_TheBlutsauger:
	case Medic_m_TheOverdose: {
		out = 1000.0f;
		break;
	}

	case Sniper_m_TheHuntsman:
	case Sniper_m_FestiveHuntsman:
	case Sniper_m_TheFortifiedCompound: {
		float charge = (GlobalVars()->CurTime - weapon->GetChargeTime());
		out = ((fminf(fmaxf(charge, 0.0f), 1.0f) * 800.0f) + 1800.0f);
		break;
	}
	}

	return out;
}

Vec3 ProjectileSolver::PredictEntityPosition(CBasePlayer* pPlayer) {

	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	if (!pLocalPlayer || pLocalPlayer->GetLifeState() == LIFE_DEAD)
		return Vec3(0, 0, 0);

	CBaseCombatWeapon* pWeapon = pLocalPlayer->GetActiveWeapon();
	if (!pWeapon)
		return Vec3(0, 0, 0);

	return Vec3();
}

Vec3 ProjectileSolver::SolveLinearPlayerPosition(CBasePlayer* player, float time) {
	PlayerInformation* playerInfo = (PlayerInformation*)CheatModules["PlayerInformation"];
	Vec3 solved{};
	Vec3 velocity = player->GetVelocity();//(playerInfo->players[player->GetIndex()].origin - playerInfo->players[player->GetIndex()].previousOrigin);
	Vec3 position = player->GetAbsOrigin();
	solved.x = position.x + (velocity.x * time);
	if (player->GetFlags() & FL_ONGROUND)
		solved.y = position.y + (velocity.y * time); // No sense in applying gravity when not on the ground
	else
		solved.y = position.y + (velocity.y * time) - Gravity->GetFloat()*time * time * 0.5;
	solved.z = position.z + (velocity.z * time);

	Ray ray;
	ray.Init(player->GetVecOrigin() + Vec3(0, 50, 0), solved + Vec3(0, 50, 0));
	CGameTrace trace;
	CTraceFilter filter;
	filter.pSkip = player;
	IEngineTrace::factory()->TraceRay(ray, MASK_SHOT, &filter, &trace);
	//solved = trace.endpos;
	return solved;
}

float ProjectileSolver::CalculateTravelTime(CBasePlayer* player, float time) {
	return 0.0f;
}

Vec3 ProjectileSolver::SolveEnginePlayerPosition(CBasePlayer* player, float time) {
	PlayerInformation* playerInfo = (PlayerInformation*)CheatModules["PlayerInformation"];
	EnginePrediction* enginePrediction = (EnginePrediction*)CheatModules["EnginePrediction"];
	int i = 0;
	Vec3 oldAbsOrigin = player->GetAbsOrigin();
	Vec3 oldVecOrigin = player->GetVecOrigin();
	Vec3 velocity = player->GetVelocity();
	int ticks = TIME_TO_TICKS(time);
	for (i = 0; i < ticks; i++) {
		CUserCmd cmd;
		Math::VectorAngles(velocity, cmd.ViewAngles);
		cmd.ViewAngles.x = cmd.ViewAngles.z = 0;
		cmd.Buttons |= IN_FORWARD;
		cmd.HasBeenPredicted = true;
		cmd.ForwardMove = velocity.Length() > 1.0f ? 450.0f : 0.0f;
		int currentFlags = player->GetFlags();
		int previousFlags = playerInfo->players[player->GetIndex()].previousFlags;
		bool isOnGround = currentFlags & FL_ONGROUND;
		bool wasOnGround = previousFlags & FL_ONGROUND;
		if (!isOnGround && wasOnGround)
			cmd.Buttons |= IN_JUMP;
		extern CUserCmd* currentCmd;
		//cmd.CommandNumber = currentCmd->CommandNumber;
		//cmd.TickCount = currentCmd->TickCount;
		//cmd.RandomSeed = currentCmd->RandomSeed;
		enginePrediction->FullPredict(&cmd, player, i, i == TIME_TO_TICKS(time) - 1);
		CUserCmd* balls = &cmd;
		playerInfo->CreateMove(balls);
	}
	Vec3 ret = player->GetAbsOrigin();
	player->SetVecOrigin(oldVecOrigin);
	player->SetAbsOrigin(oldAbsOrigin);

	return ret;
}

Vec3 ProjectileSolver::SolvePlayerPosition(CBasePlayer* player, float time, uint8_t solverType) {
	switch (solverType) {
	case SOLVER_LINEAR:
		return SolveLinearPlayerPosition(player, time);
		break;
	case SOLVER_ENGINE:
		return SolveEnginePlayerPosition(player, time);
		break;
	}
}

Vec3 projected_positions_engine[256];
Vec3 projected_positions_linear[256];

void ProjectileSolver::CreateMove(CUserCmd*& cmd) {
	if (!Enabled)
		return;
	if (cmd->TickCount % 1 != 0)
		return;
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	for (int ply = 1; ply <= GlobalVars()->MaxClients; ply++) {
		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(ply);

		// Make sure the player actually exists.
		if (pPlayer == nullptr)
			continue;

		// Don't draw ESP on LocalPlayer
		if (pPlayer == pLocalPlayer)
			continue;

		if (pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->IsDormant())
			continue;

		if (Math::WorldToScreen(pPlayer->GetAbsOrigin()).x == -1)
			continue;

		//float oldTickInterval = GlobalVars()->TickInterval;
		//GlobalVars()->TickInterval = (float)(1) / (float)(5);
		Vec3 solvedPosition = SolvePlayerPosition(pPlayer, GlobalVars()->TickInterval * 12, SOLVER_ENGINE); //5 seconds
		Vec3 solvedLinearPosition = SolvePlayerPosition(pPlayer, GlobalVars()->TickInterval * 12, SOLVER_LINEAR);
		//GlobalVars()->TickInterval = oldTickInterval;
		projected_positions_engine[pPlayer->GetIndex()] = solvedPosition;
		projected_positions_linear[pPlayer->GetIndex()] = solvedLinearPosition;

	}

}

void ProjectileSolver::Render()
{
	if (!Enabled)
		return;
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	for (int ply = 1; ply <= GlobalVars()->MaxClients; ply++) {
		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(ply);

		// Make sure the player actually exists.
		if (pPlayer == nullptr)
			continue;

		// Don't draw ESP on LocalPlayer
		if (pPlayer == pLocalPlayer)
			continue;

		if (pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->IsDormant())
			continue;

		Vec3 solvedPosition = projected_positions_engine[pPlayer->GetIndex()];
		Vec3 solvedLinearPosition = projected_positions_linear[pPlayer->GetIndex()];

		Vec3 vecPlayerToScreen = Math::WorldToScreen(solvedPosition);
		Vec3 vecPlayerToScreen2 = Math::WorldToScreen(solvedLinearPosition);

		Vec3 playerPositionToScreen = Math::WorldToScreen(pPlayer->GetVecOrigin());



		if (vecPlayerToScreen.x == -1 || vecPlayerToScreen.y == -1)
			continue; // Coordinate is not visible

		if (playerPositionToScreen.x == -1 || playerPositionToScreen.y == -1)
			continue; // Coordinate is not visible

		DrawLine(playerPositionToScreen.X, vecPlayerToScreen.x, playerPositionToScreen.Y, vecPlayerToScreen.y, ColourRed.dword, false);
		DrawRectangle(vecPlayerToScreen.x - 5, vecPlayerToScreen.x + 5, vecPlayerToScreen.y - 5, vecPlayerToScreen.y + 5, true, ColourRed.dword, 0, true);
		DrawLine(playerPositionToScreen.X, vecPlayerToScreen2.x, playerPositionToScreen.Y, vecPlayerToScreen2.y, ColourBlue.dword, false);
		DrawRectangle(vecPlayerToScreen2.x - 5, vecPlayerToScreen2.x + 5, vecPlayerToScreen2.y - 5, vecPlayerToScreen2.y + 5, true, ColourBlue.dword, 0, true);

	}
}

void ProjectileSolver::RenderMenu()
{
	ImGui::Checkbox("Enabled", &Enabled);
}


ProjectileSolver g_ProjectileSolver;