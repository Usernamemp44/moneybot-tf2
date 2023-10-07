#include "IMaterialSystem.h"
#include "../../../Misc/VMT.h"
#include "../../Misc/GetInterface.h"

IMaterial *IMaterialSystem::FindMaterial(char const *pMaterialName, const char *pTextureGroupName, bool complain, const char *pComplainPrefix)
{
	typedef IMaterial *(__thiscall *FN)(void*, const char *, const char *, bool, const char *);
	return GetVFunc<FN>(this, 73)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
}

IMaterial *IMaterialSystem::CreateMaterial(char const *pMaterialName, KeyValues *kv)
{
	typedef IMaterial *(__thiscall *FN)(PVOID, const char *, KeyValues *);
	return GetVFunc<FN>(this, 72)(this, pMaterialName, kv);
}

IMaterial *IMaterialSystem::GetMaterial(MaterialHandle_t h)
{
	typedef IMaterial *(__thiscall *FN)(void*, MaterialHandle_t);
	return GetVFunc<FN>(this, 76)(this, h);
}

IMatRenderContext * IMaterialSystem::GetRenderContext()
{
	typedef IMatRenderContext*(__thiscall *FN)(void*); 
	return GetVFunc<FN>(this, 100)(this); //98? 96?
}

MaterialHandle_t IMaterialSystem::FirstMaterial()
{
	typedef MaterialHandle_t(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 75)(this);
}

MaterialHandle_t IMaterialSystem::InvalidMaterial()
{
	typedef MaterialHandle_t(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 77)(this);
}

MaterialHandle_t IMaterialSystem::NextMaterial(MaterialHandle_t h)
{
	typedef MaterialHandle_t(__thiscall *FN)(void*, MaterialHandle_t);
	return GetVFunc<FN>(this, 76)(this, h);
}

IMaterialSystem* cached_materialsystem = NULL;
IMaterialSystem* IMaterialSystem::factory() {
	if (cached_materialsystem == nullptr) {
		cached_materialsystem = (IMaterialSystem*)(GetInterface("MaterialSystem.dll", "VMaterialSystem081"));
	}
	return cached_materialsystem;
}