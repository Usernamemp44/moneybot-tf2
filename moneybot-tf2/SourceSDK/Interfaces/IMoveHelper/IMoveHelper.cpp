#include "IMoveHelper.h"
#include "../../../Misc/VMT.h"
#include "../../../Misc/PatternFinder.h"

void IMoveHelper::SetHost(CBaseEntity *pHost)
{
	typedef void(__thiscall *FN)(void*, CBaseEntity *);
	GetVFunc<FN>(this, 0)(this, pHost);
}

IMoveHelper* cached_IMoveHelper = NULL;

IMoveHelper*  IMoveHelper::factory() {
	if (cached_IMoveHelper == NULL)
		cached_IMoveHelper = **(IMoveHelper***)(FindPatternInClient("8B 0D ? ? ? ? 8B 46 08 68") + 2);

	return cached_IMoveHelper;
}