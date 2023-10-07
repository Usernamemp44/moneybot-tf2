#pragma once
#include "../Cheats/BaseCheat.h"
#include "../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include <vector>

namespace Groups {
	std::vector<CBasePlayer*> AllEntites();
	std::vector<CBasePlayer*> TeamMates(CBasePlayer* pLocalPlayer);
	std::vector<CBasePlayer*> Hostiles(CBasePlayer* pLocalPlayer);
	std::vector<CBasePlayer*> Buildings(CBasePlayer* pLocalPlayer);
	std::vector<CBasePlayer*> Projectiles(CBasePlayer* pLocalPlayer);
}