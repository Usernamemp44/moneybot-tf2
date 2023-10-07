#include "CBaseEntity.h"
#include "../../../Misc/DynamicNetvars.h"
#include "../../../Misc/VMT.h"
#include "../../../Misc/PatternFinder.h"
#include "../../Interfaces/CModelInfo/CModelInfo.h"
extern CRITICAL_SECTION absorigin;
Vec3 CBaseEntity::GetAbsOrigin()
{
	//EnterCriticalSection(&absorigin);
	typedef Vec3 &(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 9)(this);
	//LeaveCriticalSection(&absorigin);
}

Vec3 CBaseEntity::GetAbsAngles()
{
	typedef Vec3 &(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 10)(this);
}

int CBaseEntity::DrawModel(int flags)
{
	void *renderable = (PVOID)(this + sizeof(uintptr_t));
	typedef int(__thiscall *FN)(PVOID, int);
	return GetVFunc<FN>(renderable, 10)(renderable, flags);
}

void CBaseEntity::SetVecOrigin(Vec3 & vOrigin)
{
	DYNVAR_SET(Vec3, this, vOrigin, "DT_BaseEntity", "m_vecOrigin");
}

Vec3 CBaseEntity::GetVecOrigin()
{
	DYNVAR_RETURN(Vec3, this, "DT_BaseEntity", "m_vecOrigin");
}

Vec3 CBaseEntity::GetWorldSpaceCenter()
{
	Vec3 vMin, vMax;
	this->GetRenderBounds(vMin, vMax);
	Vec3 vWorldSpaceCenter = this->GetAbsOrigin();
	vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2.0f;

	return vWorldSpaceCenter;
}

int CBaseEntity::GetHealth() {
	//GHETTO VIRTUAL FUNCTION
	if (IsPlayer()) {
		DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iHealth");
	}
	if (IsBuilding()) {
		DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iHealth");
	}
	return 0;
}

int CBaseEntity::GetMaxHealth() {
	//GHETTO VIRTUAL FUNCTION
	if (IsPlayer()) {
		typedef int(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 107)(this);
	}
	if (IsBuilding()) {
		DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iMaxHealth");
	}
	return 0;
}


bool CBaseEntity::IsWeapon()
{
	typedef bool(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 137)(this);
}

bool CBaseEntity::IsPlayer()
{
	return (this->GetClientClass()->iClassID == CTFPlayer);
}

bool CBaseEntity::IsBuilding()
{
	return (this->GetClientClass()->iClassID == CObjectDispenser
		|| this->GetClientClass()->iClassID == CObjectSentrygun
		|| this->GetClientClass()->iClassID == CObjectTeleporter);
}

bool CBaseEntity::IsPickup()
{
	return (this->GetClientClass()->iClassID == CBaseAnimating && CModelInfo::factory()->GetModelName(GetModel())[24] != 'h' || this->GetClientClass()->iClassID == CTFAmmoPack);
}
bool CBaseEntity::IsWearable()
{
	typedef bool(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 138)(this);
}

bool CBaseEntity::IsDormant()
{
	PVOID pNetworkable = (PVOID)(this + 0x8);
	typedef bool(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(pNetworkable, 8)(pNetworkable);
}

void CBaseEntity::GetRenderBounds(Vec3 & vMins, Vec3 & vMaxs)
{
	PVOID pRenderable = (PVOID)(this + 0x4);
	typedef void(__thiscall *FN)(PVOID, Vec3 &, Vec3 &);
	GetVFunc<FN>(pRenderable, 20)(pRenderable, vMins, vMaxs);
}

int CBaseEntity::GetTeamNum()
{
	DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_iTeamNum");
}

int CBaseEntity::GetClassId()
{
	return this->GetClientClass()->iClassID;
}

CClientClass * CBaseEntity::GetClientClass()
{
	PVOID pNetworkable = (PVOID)(this + 0x8);
	typedef CClientClass *(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(pNetworkable, 2)(pNetworkable);
}

int CBaseEntity::GetIndex()
{
	PVOID pNetworkable = (PVOID)(this + 0x8);
	typedef int(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(pNetworkable, 9)(pNetworkable);
}
void CBaseEntity::InvalidateBoneCache() {
	//A1 ? ? ? ? 48 C7 81
	typedef void(__thiscall *InvalidateBoneCache)(void*);
	static auto fn = reinterpret_cast<InvalidateBoneCache>(FindPatternInClient("A1 ?? ?? ?? ?? 48 C7 81"));
	fn(this);

}
void CBaseEntity::SetAbsOrigin(Vec3 orig) {
	//EnterCriticalSection(&absorigin);
	typedef void(__thiscall *SetAbsOriginFn)(void*, const Vec3&);
	static auto fn = reinterpret_cast<SetAbsOriginFn>(FindPatternInClient("55 8B EC 56 57 8B F1 E8 ?? ?? ?? ?? 8B 7D 08 F3 0F 10 07 0F 2E 86 ?? ?? ?? ??"));
	fn(this, orig);
	//LeaveCriticalSection(&absorigin);
}
void CBaseEntity::SetAbsAngles(Vec3 orig) {
	//55 8B EC 81 EC ? ? ? ? 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3 0F 10 07
	typedef void(__thiscall *SetAbsAnglesFn)(void*, const Vec3&);
	static auto fn = reinterpret_cast<SetAbsAnglesFn>(FindPatternInClient("55 8B EC 83 EC 60 56 57 8B F1"));
	fn(this, orig);

}
Vec3 CBaseEntity::GetCollideableMins()
{
	DYNVAR_RETURN(Vec3, this, "DT_BaseEntity", "m_Collision", "m_vecMins");
}

Vec3 CBaseEntity::GetCollideableMaxs()
{
	DYNVAR_RETURN(Vec3, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs");
}	

DWORD *CBaseEntity::GetModel()
{
	PVOID pRenderable = (PVOID)(this + 0x4);
	typedef DWORD *(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(pRenderable, 9)(pRenderable);
}

int CBaseEntity::GetHitboxSet()
{
	DYNVAR_RETURN(int, this, "DT_BaseAnimating", "m_nHitboxSet");
}

int CBaseEntity::GetOwner()
{
	DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_hOwnerEntity");
}
float CBaseEntity::GetSimulationTime() {
	DYNVAR_RETURN(float, this, "DT_BaseEntity", "m_flSimulationTime");
}