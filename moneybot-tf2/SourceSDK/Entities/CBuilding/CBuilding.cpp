#include "CBuilding.h"
CBaseEntity *CBuilding::GetOwner()
{
	DYNVAR_RETURN(CBaseEntity *, this, "DT_BaseObject", "m_hBuilder");
}

int CBuilding::GetLevel()
{
	DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iUpgradeLevel");
}

bool CBuilding::IsSapped()
{
	DYNVAR_RETURN(bool, this, "DT_BaseObject", "m_bHasSapper");
}

int CBuilding::GetHealth()
{
	DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iHealth");
}

int CBuilding::GetMaxHealth()
{
	DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iMaxHealth");
}

bool CBuilding::IsCarried()
{
	DYNVAR_RETURN(int, this, "DT_BaseObject", "m_bCarried");
}