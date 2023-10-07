#pragma once

#include "../../BaseCheat.h"
#include "../../../SourceSDK/Misc/Vector3.h"
#include "../../../imgui/imgui.h"
#include "../../../Misc/Colour.h" 

class ConfigurationSystem : public BaseCheat {
public:
	ConfigurationSystem();
	void RenderMenu() override;
};