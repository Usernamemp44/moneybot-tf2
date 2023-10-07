#include "Groups.h"
#include "../SourceSDK/Interfaces/CEntityList/CEntityList.h"

int maxPlayers = CEntityList::factory()->GetHighestEntityIndex();

/* Gets all entityes, this includes players, buildings, etc.. (and localplayer)*/
std::vector<CBasePlayer*> Groups::AllEntites() {

	std::vector<CBasePlayer*> playerList;
	for (int i = 0; i <= CEntityList::factory()->GetHighestEntityIndex(); i++) {

		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(i);
		if (!pPlayer || pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->GetHealth() <= 0)
			continue;

		playerList.push_back(pPlayer);

	}	
	return playerList;
}

/* Gets all players on locals team */
std::vector<CBasePlayer*> Groups::TeamMates(CBasePlayer* pLocalPlayer) {

	std::vector<CBasePlayer*> playerList;
	for (int i = 0; i < maxPlayers; i++) {

		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(i);
		if (!pPlayer || pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->GetHealth() <= 0 || pPlayer->GetTeamNum() != pLocalPlayer->GetTeamNum() || pPlayer == pLocalPlayer)
			continue;

		if (pPlayer->GetClientClass()->iClassID != CTFPlayer)
			continue;

		playerList.push_back(pPlayer);
	}

	return playerList;

}

/* Gets all enemies */
std::vector<CBasePlayer*> Groups::Hostiles(CBasePlayer* pLocalPlayer) {
	std::vector<CBasePlayer*> playerList;
	for (int i = 0; i < maxPlayers; i++) {

		CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(i);
		if (!pPlayer || pPlayer->GetLifeState() == LIFE_DEAD || pPlayer->GetHealth() <= 0 || pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum() || pPlayer == pLocalPlayer)
			continue;

		if (pPlayer->GetClientClass()->iClassID != CTFPlayer)
			continue;

		playerList.push_back(pPlayer);
	}

	return playerList;
}

/* Gets all buildings in the game (including teams buildings) */
std::vector<CBasePlayer*> Groups::Buildings(CBasePlayer* pLocalPlayer) {

	std::vector<CBasePlayer*> buldingList;
	for (int i = 0; i < maxPlayers; i++) {

		CBasePlayer* pBuilding = CEntityList::factory()->GetClientPlayer(i);
		if (!pBuilding || pBuilding->GetLifeState() == LIFE_DEAD || pBuilding->GetHealth() <= 0 || pBuilding == pLocalPlayer)
			continue;

		if (pBuilding->GetClientClass()->iClassID != (CObjectTeleporter || CObjectSentrygun || CObjectDispenser))
			continue;

		buldingList.push_back(pBuilding);
	}

	return buldingList;

}


/* Gets all projectiles that are in the air (useful for autoairblast, chams,visuals) */
std::vector<CBasePlayer*> Groups::Projectiles(CBasePlayer* pLocalPlayer) {
	std::vector<CBasePlayer*> projectileList;
	for (int i = 0; i < maxPlayers; i++) {

		CBasePlayer* pProjectile = CEntityList::factory()->GetClientPlayer(i);
		if (!pProjectile || pProjectile == pLocalPlayer || pProjectile->IsDormant())
			continue;

		if (pProjectile->GetClientClass()->iClassID != (
			CTFProjectile_SentryRocket ||
			CTFProjectile_Arrow ||
			CTFProjectile_HealingBolt ||
			CTFProjectile_Jar ||
			CTFProjectile_JarMilk ||
			CTFProjectile_Throwable ||
			CTFProjectile_Rocket
			)) 
			continue;	
		

		projectileList.push_back(pProjectile);
	}

	return projectileList;
}

