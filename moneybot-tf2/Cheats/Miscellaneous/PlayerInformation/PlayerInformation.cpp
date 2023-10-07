#include "PlayerInformation.h"
#include "../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"

#include "../../CheatList.h"

PlayerInformation::PlayerInformation()
{
	CheatModules["PlayerInformation"] = this;
}

void PlayerInformation::CreateMove(CUserCmd *&)
{

}
void PlayerInformation::SceneEnd() {
	for (int i = 1; i <= CEngineClient::factory()->GetMaxClients(); i++) {

		CBasePlayer* entity = CEntityList::factory()->GetClientPlayer(i);
		if (!entity) {
			continue;
		}
		PlayerData oldPlayerData = players[entity->GetIndex()];
		players[entity->GetIndex()] = PlayerData();
		PlayerData& balls = players[entity->GetIndex()];
		balls.flags = entity->GetFlags();
		balls.origin = entity->GetAbsOrigin();
		balls.previousFlags = oldPlayerData.flags;
		balls.previousOrigin = oldPlayerData.origin;
		balls.eyepos = entity->GetEyePos();
	}
}
void PlayerInformation::RenderMenu()
{
}

PlayerInformation g_PlayerInformation;