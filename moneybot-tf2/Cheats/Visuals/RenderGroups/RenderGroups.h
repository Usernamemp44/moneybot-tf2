#pragma once

#include "../../BaseCheat.h"
#include "../../../imgui/imgui.h"
#include "../../../Misc/Colour.h"
#include "../../../json.h"
#include "RenderGroup.h"
#include <vector>
class RenderGroups : public BaseCheat {
public:
	bool Enabled;
	RenderGroups();
	void RenderMenu() override;
	void LoadConfig() override;
	void SaveConfig() override;
	int CurrentlySelectedRenderGroup = -1;
	RenderGroup* FindMatchingRenderGroup(CBaseEntity*);
	
private:
	std::vector<RenderGroup*> renderGroups;
};
