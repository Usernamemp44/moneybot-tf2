#include "CBaseCombatWeapon.h"
#include "../../../Misc/DynamicNetvars.h"
#include "../../../Misc/VMT.h"
#include "../../Interfaces/CEntityList/CEntityList.h"
#include "../CBasePlayer/CBasePlayer.h"
#include "../../Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../Interfaces/CEngineClient/CEngineClient.h"
char* CBaseCombatWeapon::GetName()
{
	typedef char *(__thiscall* FN)(PVOID);
	return GetVFunc<FN>(this, 329)(this);
}

int CBaseCombatWeapon::GetSlot()
{
	typedef int(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 327)(this);
}

float CBaseCombatWeapon::GetChargeTime()
{
	DYNVAR_RETURN(float, this, "DT_WeaponPipebombLauncher", "PipebombLauncherLocalData", "m_flChargeBeginTime");
}

float CBaseCombatWeapon::GetChargeDamage()
{
	DYNVAR_RETURN(float, this, "DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage");
}

float CBaseCombatWeapon::GetNextPrimaryAttack()
{
	DYNVAR_RETURN(float, this, "DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
}

int CBaseCombatWeapon::GetItemDefinitionIndex()
{
	DYNVAR_RETURN(int, this, "DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
}

float CBaseCombatWeapon::GetLastFireTime()
{
	DYNVAR_RETURN(float, this, "DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
}

float CBaseCombatWeapon::GetSwingRange(CBaseEntity *pLocal)
{
	typedef int(__thiscall *FN)(CBaseEntity *);
	return (static_cast<float>(GetVFunc<FN>(this, 451)(pLocal)));
}

bool CBaseCombatWeapon::DoSwingTrace(CGameTrace &Trace)
{
	typedef int(__thiscall *FN)(CGameTrace &);
	return GetVFunc<FN>(this, 453)(Trace);
}

Vec3 &CBaseCombatWeapon::GetBulletSpread()
{
	typedef Vec3 &(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 286)(this);
}

int CBaseCombatWeapon::GetDamageType()
{
	typedef int(__thiscall * FN)(void *);
	return GetVFunc<FN>(this, 378)(this);
}

bool CBaseCombatWeapon::CanFireCriticalShot(bool bHeadShot)
{
	typedef bool(__thiscall * FN)(void *, bool, CBaseEntity *);
	return GetVFunc<FN>(this, 421)(this, bHeadShot, nullptr);
}



bool CBaseCombatWeapon::IsReadyToFire() {


	/* Need to implement more stuff for crossbow and shit but good enough for now ig :shrug: */
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	bool canShootWeapon;

	static float nextAttack = 0.0f;
	static float lastAttack = 0.0f;
	float serverTime = pLocalPlayer->GetTickBase() * GlobalVars()->TickInterval;
	float nextTime = this->GetNextPrimaryAttack();
	float lastTime = this->GetLastFireTime();

	if (nextTime != lastAttack) {
		nextAttack = nextTime;
		lastAttack = lastTime;
	}
	canShootWeapon = nextAttack <= serverTime;

	return canShootWeapon;
}
