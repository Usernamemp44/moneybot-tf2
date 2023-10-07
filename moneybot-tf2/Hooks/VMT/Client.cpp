#include "Client.h"
#include "../Hooks.h"
#include "../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"

using FrameStageNotify = void(__thiscall *)(PVOID, ClientFrameStage_t);
void __fastcall FrameStageNotifyHook(PVOID client, void* edx, ClientFrameStage frame)
{
	CBasePlayer *localPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	if (frame == ClientFrameStage_t::FRAME_RENDER_START) {

		for (int i = 0; i <= CEngineClient::factory()->GetMaxClients(); i++)
		{
			CBasePlayer* entity = CEntityList::factory()->GetClientPlayer(i);
			if (entity == NULL || entity->IsDormant() || entity->GetLifeState() == LIFE_DEAD)
				continue;
			//no_interp_eye_angles[i] = entity->GetEyeAngles();
			entity->GetBoneMatrix(no_interp_bone_matrix[i], 256);
			entity->SetAbsOrigin(entity->GetVecOrigin());
		}
	}

	ClientHook->GetMethod<FrameStageNotify>(FRAME_STAGE_NOTIFY_INDEX)(client, frame);
}
