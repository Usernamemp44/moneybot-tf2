#include "ModelRender.h"
#include "../../SourceSDK/Interfaces/CModelInfo/CModelInfo.h"
#include "../../Cheats/CheatList.h"
#include "../../Cheats/Visuals/Chams.h"
using fn = void(__thiscall *)(CModelRender *, const DrawModelState_t &, const ModelRenderInfo_t &, matrix3x4 *);

void __stdcall DrawModelExecuteHook(DrawModelState_t &state, const ModelRenderInfo_t &info, matrix3x4 *matrix) {


	fn original = ModelRenderHook->GetMethod<fn>(DRAW_MODEL_EXECUTE_INDEX);

	bool render =true;
	Chams* chams = (Chams*)CheatModules["Chams"];

	CBasePlayer* localPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	if (chams->PlayerChams.InvisibleChamsEnabled || chams->PlayerChams.VisibleChamsEnabled )
	{
		if (!chams->DrawingChams && (info.entity_index > 0 && info.entity_index <= CEngineClient::factory()->GetMaxClients()) && info.entity_index != (CEngineClient::factory()->GetLocalPlayer())) {
			render = false;
			if (chams->IgnoreTeammates) {
				if (CEntityList::factory()->GetClientPlayer(info.entity_index)->GetTeamNum() == localPlayer->GetTeamNum())
					render = true;
			}
		}
	}
	if (chams->DrawingChams)
		state.m_decals = 0x0;

	
	
	if (render)
		original(CModelRender::factory(), state, info, matrix);
}