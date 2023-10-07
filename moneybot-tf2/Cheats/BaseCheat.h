#pragma once
#include "../SourceSDK/Misc/Defines.h"
#include "../Misc/Groups.h"
#include "../carbon/carbon.hpp"


class BaseCheat {
public:
	virtual void SceneEnd() {};
	virtual void CreateMove(CUserCmd_t*&) {};
	virtual void OverrideView(CViewSetup *) {};
	virtual void Render() {};
	virtual void RenderMenu() {};
	virtual void SaveConfig() {};
	virtual void LoadConfig() {};
	bool CanRender = false;
	bool Callable = true;
	bool Priority = false;
	bool ShouldPredict = false;

	const char* Group = NULL;
};