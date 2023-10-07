#include "BunnyHop.h"


#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"

#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/IGameMovement/IGameMovement.h"
#include "../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../../SourceSDK/Interfaces/IEngineTrace/IEngineTrace.h"
#include "../../CheatList.h"

#include "../../../imgui/imgui.h"

BunnyHop::BunnyHop() {
	CheatModules["BunnyHop"] = this;
	CanRender = true;
}

void BunnyHop::RenderMenu()
{
	ImGui::Checkbox("Enabled", &Enabled);
//		ImGui::SliderFloat("Aimbot FOV", &AimbotFOV, 0.01f, 180);
//	}
}
void BunnyHop::CreateMove(CUserCmd_t*& cmd) {
	if (!Enabled) return;
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	//if (!AimbotEnabled || !(cmd->Buttons & IN_ATTACK)) { return; }
	if (cmd->CommandNumber == 0) return;

	if (pLocalPlayer->GetMoveType() == MOVETYPE_LADDER ||
		pLocalPlayer->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	static bool first_jump = false, fake_jump;
	if (cmd->Buttons & IN_JUMP) {
		if (!first_jump) {
			first_jump = fake_jump = true;
		}
		else if (!(pLocalPlayer->GetFlags() & FL_ONGROUND)) {
			if (fake_jump) {
				fake_jump = false;
			}
			else {
				cmd->Buttons &= ~IN_JUMP;
			}
		}
		else {
			fake_jump = true;
		}
	}
	else {
		first_jump = false;
	}
}

BunnyHop g_BunnyHop;