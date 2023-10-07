#include "CEntityList.h"
#include "../../Misc/GetInterface.h"
#include "../../../Misc/VMT.h"



CBaseEntity *CEntityList::GetClientEntity(int nEntityIndex)
{
	typedef CBaseEntity *(__thiscall *FN)(PVOID, int);
	return GetVFunc<FN>(this, 3)(this, nEntityIndex);
}

CBasePlayer * CEntityList::GetClientPlayer(int nEntityIndex)
{
	return (CBasePlayer*)GetClientEntity(nEntityIndex);
}

CBaseEntity *CEntityList::GetClientEntityFromHandle(uintptr_t hEntity)
{
	typedef CBaseEntity *(__thiscall *FN)(PVOID, uintptr_t);
	return GetVFunc<FN>(this, 4)(this, hEntity);
}
CBaseEntity *CEntityList::GetClientEntityFromHandle(int hEntity)
{
	typedef CBaseEntity *(__thiscall *FN)(PVOID, int);
	return GetVFunc<FN>(this, 4)(this, hEntity);
}
int CEntityList::GetHighestEntityIndex(void)
{
	typedef int(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 6)(this);
}
CEntityList* cached = nullptr;
CEntityList * CEntityList::factory()
{
	if (cached == nullptr) {
		cached = (CEntityList*)(GetInterface("client.dll", "VClientEntityList003"));
	}
	return cached;
}
CEntityList  CEntityList::factory_noptr()
{
	if (cached == nullptr) {
		cached = (CEntityList*)(GetInterface("client.dll", "VClientEntityList003"));
	}
	return *cached;
}

