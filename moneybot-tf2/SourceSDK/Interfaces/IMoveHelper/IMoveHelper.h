#pragma once
#include "../../Misc/Defines.h"

class IMoveHelper
{
public:
	void SetHost(CBaseEntity *pHost);
	static IMoveHelper* factory();
};
extern IMoveHelper* cached_IMoveHelper;