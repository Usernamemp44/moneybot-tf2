#pragma once
#include "../../Misc/Defines.h"

class CHLClient
{
public:
	CClientClass *GetAllClasses(void);
	bool ShouldDrawEntity(CBaseEntity *pEntity);
	bool GetPlayerView(CViewSetup &view);
	void RenderView(const CViewSetup &view, int flags, int draw);
	static CHLClient * factory();
	static CHLClient factory_noptr();
};