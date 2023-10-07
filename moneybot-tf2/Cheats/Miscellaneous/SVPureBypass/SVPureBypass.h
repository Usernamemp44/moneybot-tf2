#pragma once

#include "../../BaseCheat.h"
#include "../../../SourceSDK/Misc/Vector3.h"
#include "../../../imgui/imgui.h"

class CBasePlayer;

class SVPureBypass : public BaseCheat {
public:
	bool bypassed = false;
	void RenderMenu() override;

	SVPureBypass();

};
