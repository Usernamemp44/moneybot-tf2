#pragma once
#include "../../Misc/Defines.h"

class CClientModeShared
{
public:
	bool IsChatPanelOutOfFocus(void);
	static CClientModeShared* factory();
	static CClientModeShared factory_noptr();
};