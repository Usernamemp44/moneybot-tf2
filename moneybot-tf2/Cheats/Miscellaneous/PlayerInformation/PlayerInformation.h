#pragma once

#include "../../BaseCheat.h"
#include "../../../SourceSDK/Misc/Vector3.h"
#include "../../../imgui/imgui.h"
#include "../../../SourceSDK/Interfaces/ConVar/ConVar.h"

struct PlayerData {
	Vec3 origin;
	Vec3 previousOrigin;
	Vec3 eyepos;
	int flags;
	int previousFlags;
};
class PlayerInformation : public BaseCheat {
public:
	bool Enabled = true;
	PlayerInformation();
	void CreateMove(CUserCmd*&) override;
	void SceneEnd() override;
	void RenderMenu() override;
	PlayerData players[256];
};

