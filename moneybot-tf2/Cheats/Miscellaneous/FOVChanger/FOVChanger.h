#pragma once
#include "../../BaseCheat.h"

class FOVChanger : public BaseCheat {
public:
	FOVChanger();
	void RenderMenu() override;
	void SaveConfig()  override;
	void LoadConfig()  override;
	void OverrideView(CViewSetup*) override;
	float CustomFOV;
	float CustomScopedFOV;
};