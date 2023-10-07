#include "IPlayerInfoManager.h"
#include "..\..\Misc\GetInterface.h"


IPlayerInfoManager * cached_pinfomanager;
IPlayerInfoManager * IPlayerInfoManager::factory()
{
	if (cached_pinfomanager == nullptr) {
		cached_pinfomanager = (IPlayerInfoManager*)GetInterface("server.dll", "PlayerInfoManager002");
	}
	return cached_pinfomanager;
}
