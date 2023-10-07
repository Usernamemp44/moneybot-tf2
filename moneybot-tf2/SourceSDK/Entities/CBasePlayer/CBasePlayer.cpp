#include "CBasePlayer.h"
#include "../CBaseCombatWeapon/CBaseCombatWeapon.h"
#include "../../../Misc/DynamicNetvars.h"
#include "../../../Misc/VMT.h"
#include "../../Interfaces/CModelInfo/CModelInfo.h"
#include "../../../Misc/Math.h"
#include "../../Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../Interfaces/CEntityList/CEntityList.h"
#include "../../../Misc/PatternFinder.h"
#include "../../Interfaces/IEngineTrace/IEngineTrace.h"

matrix3x4 no_interp_bone_matrix[256][256] = {};

int CBasePlayer::GetMaxHealth()
{
	typedef int(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 107)(this);
}

Vec3 CBasePlayer::GetEyePos()
{
	DYNVAR(balls, Vec3, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	return GetAbsOrigin() + balls.GetValue(this);
}

extern CRITICAL_SECTION absorigin;
void CBasePlayer::GetBoneMatrix(matrix3x4 boneMatrix[256], int v) {
	//EnterCriticalSection(&absorigin);
	//Vec3 original = GetAbsOrigin();
	//SetAbsOrigin(GetVecOrigin());
	this->SetupBones(boneMatrix, 256, v, GlobalVars()->CurTime);
	//SetAbsOrigin(original);
	//LeaveCriticalSection(&absorigin);
}

int CBasePlayer::GetMoveType() {
	DYNVAR_RETURN(int, this, "DT_BaseEntity", "movetype");
}

int CBasePlayer::GetHealth()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iHealth");
}

int CBasePlayer::GetTickBase()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "localdata", "m_nTickBase");
}

int CBasePlayer::GetFlags()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_fFlags");
}

void CBasePlayer::SetFlags(int nFlags)
{
	DYNVAR_SET(int, this, nFlags, "DT_BasePlayer", "m_fFlags");
}

BYTE CBasePlayer::GetLifeState()
{
	DYNVAR_RETURN(BYTE, this, "DT_BasePlayer", "m_lifeState");
}

uint8_t CBasePlayer::GetWaterLevel()
{
	DYNVAR_RETURN(BYTE, this, "DT_BasePlayer", "localdata", "m_nWaterLevel");
}

int CBasePlayer::GetClassNum()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_PlayerClass", "m_iClass");
}

int CBasePlayer::GetCond()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCond");
}

int CBasePlayer::GetCondEx()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx");
}

int CBasePlayer::GetCondEx2()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2");
}

int CBasePlayer::GetCondEx3()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCondEx2");
}

bool CBasePlayer::IsInCond(int eCond)
{
	int m_pnCondVar = 0;
	int m_nCondBit = 0;
	if (eCond >= 96)
	{
		//Assert(eCond < 96 + 32);
		m_pnCondVar = GetCondEx3();
		m_nCondBit = eCond - 96;
	}
	else if (eCond >= 64)
	{
		//Assert(eCond < (64 + 32));
		m_pnCondVar = GetCondEx2();
		m_nCondBit = eCond - 64;
	}
	else if (eCond >= 32)
	{
		//Assert(eCond < (32 + 32));
		m_pnCondVar = GetCondEx();
		m_nCondBit = eCond - 32;
	}
	else
	{
		m_pnCondVar = GetCond();
		m_nCondBit = eCond;
	}
	return (m_pnCondVar & (1 << m_nCondBit)) != 0;
}

void CBasePlayer::RemoveCond(int cond)
{
	static DWORD offset = g_NetVars.GetOffset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");

	if (*reinterpret_cast<int *>(this + offset) & cond)
		*reinterpret_cast<DWORD *>(this + offset) &= ~cond;
}

CUserCmd *CBasePlayer::GetCurrentCommand()
{
	DYNVAR_RETURN(CUserCmd *, (this - sizeof(uintptr_t)), "DT_BasePlayer", "localdata", "m_hConstraintEntity");
}

void CBasePlayer::SetCurrentCmd(CUserCmd *cmd)
{
	DYNVAR_SET(CUserCmd *, (this - sizeof(uintptr_t)), cmd, "DT_BasePlayer", "localdata", "m_hConstraintEntity");
}

Vec3 CBasePlayer::GetVecPunchAngle()
{
	return *reinterpret_cast<Vec3*>(uintptr_t(this) + 0xE48);
}

Vec3 CBasePlayer::GetEyeAngles()
{
	DYNVAR_RETURN(Vec3, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
}

void CBasePlayer::SetEyeAngles(Vec3 &v)
{
	DYNVAR(n, Vec3, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
	n.SetValue(this, v);
}

bool CBasePlayer::SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int nBoneMask, float fCurrentTime)
{
	PVOID pRenderable = (PVOID)(this + 0x4);
	typedef bool(__thiscall *FN)(PVOID, matrix3x4 *, int, int, float);
	return GetVFunc<FN>(pRenderable, 16)(pRenderable, pBoneToWorldOut, nMaxBones, nBoneMask, fCurrentTime);
}

CBoneCache* CBasePlayer::GetBoneCache(void) {
	static auto fn = SearchForStringXref("client.dll", "GetBoneCache");
	typedef CBoneCache*(__thiscall *FN)(void*, void*);
	return ((FN)(fn))(this, 0);
}

Vec3 CBasePlayer::GetHitboxPos(int nHitbox, bool uninterpolated)
{
	DWORD *pModel = GetModel();
	if (!pModel)
		return Vec3();

	studiohdr_t *pHdr = (studiohdr_t *)CModelInfo::factory()->GetStudioModel(pModel);
	if (!pHdr)
		return Vec3();

	mstudiohitboxset_t *pSet = pHdr->GetHitboxSet(this->GetHitboxSet());
	if (!pSet)
		return Vec3();

	mstudiobbox_t *pBox = pSet->pHitbox(nHitbox);
	if (!pBox)
		return Vec3();

	Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f;
	Vec3 vOut = Vec3();

	if (uninterpolated) {
		matrix3x4 BoneMatrix[128];
		if (!this->SetupBones(BoneMatrix, 128, 0x100, IPlayerInfoManager::factory()->GetGlobalVars()->CurTime))
			return Vec3();
		Math::VectorTransform(vPos, BoneMatrix[pBox->bone], vOut);
	}
	else {
		Math::VectorTransform(vPos, no_interp_bone_matrix[this->GetIndex()][pBox->bone], vOut);
	}

	return vOut;
}

Vec3 CBasePlayer::GetVelocity()
{
	DYNVAR_RETURN(Vec3, this, "DT_BasePlayer", "localdata", "m_vecVelocity[0]");
}

Vec3 CBasePlayer::GetBaseVelocity() {
	DYNVAR_RETURN(Vec3, this, "DT_BasePlayer", "localdata", "m_vecBaseVelocity");
}

int CBasePlayer::AllowAutoMovement() {
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "localdata", "m_Local",  "m_bAllowAutoMovement");
}

float CBasePlayer::ModelScale() {
	DYNVAR_RETURN(float, this, "DT_BaseAnimating", "m_flModelScale");
}

float CBasePlayer::GetStepSize() {
	//DYNVAR_RETURN(float, this, "DT_BaseEntity", "m_flStepSize");
	return 18.0f;
}

float CBasePlayer::GetMaxSpeed() {
	DYNVAR_RETURN(float, this, "DT_BasePlayer", "m_flMaxSpeed");
}

Vec3 CBasePlayer::GetHitboxPos(const char* name, bool uninterpolated)
{
	DWORD *pModel = GetModel();
	if (!pModel)
		return this->GetAbsOrigin();

	studiohdr_t *pHdr = (studiohdr_t *)CModelInfo::factory()->GetStudioModel(pModel);
	if (!pHdr)
		return this->GetAbsOrigin();


	mstudiohitboxset_t *pSet = pHdr->GetHitboxSet(this->GetHitboxSet());
	if (!pSet)
		return this->GetAbsOrigin();
	mstudiobbox_t *pBox = 0;
	for (int i = 0; i < pSet->numhitboxes; i++) {
		pBox = pSet->pHitbox(i);
		mstudiobone_t* bone = (mstudiobone_t *)(((byte *)pHdr) + pHdr->boneindex) + pBox->bone;
		//char* fname = _strdup(bone->pszName());
		if (strstr(bone->pszName(), name) != 0) {
			break;
		}
		else {
			pBox = 0;
		}
	}
	if (pBox == 0)
		return this->GetAbsOrigin();

	Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f;
	Vec3 vOut = Vec3();
	if (uninterpolated) {
		matrix3x4 BoneMatrix[128];
		if (!this->SetupBones(BoneMatrix, 128, 0x100, IPlayerInfoManager::factory()->GetGlobalVars()->CurTime))
			return Vec3();
		Math::VectorTransform(vPos, BoneMatrix[pBox->bone], vOut);
	}
	else {
		Math::VectorTransform(vPos, no_interp_bone_matrix[this->GetIndex()][pBox->bone], vOut);
	}
	return vOut;
}

int CBasePlayer::GetNumOfHitboxes()
{
	DWORD *pModel = GetModel();
	if (!pModel)
		return 0;

	studiohdr_t *pHdr = (studiohdr_t *)CModelInfo::factory()->GetStudioModel(pModel);
	if (!pHdr)
		return 0;

	mstudiohitboxset_t *pSet = pHdr->GetHitboxSet(this->GetHitboxSet());
	if (!pSet)
		return 0;

	return pSet->numhitboxes;
}

Vec3 CBasePlayer::GetBonePos(int nBone)
{
	matrix3x4 matrix[128];

	if (this->SetupBones(matrix, 128, 0x100, GetTickCount64()))
		return Vec3(matrix[nBone][0][3], matrix[nBone][1][3], matrix[nBone][2][3]);

	return Vec3(0.0f, 0.0f, 0.0f);
}

CBaseCombatWeapon *CBasePlayer::GetActiveWeapon()
{
	DYNVAR(pHandle, int, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	return reinterpret_cast<CBaseCombatWeapon *>(CEntityList::factory()->GetClientEntityFromHandle(pHandle.GetValue(this)));
}

bool CBasePlayer::IsVisible(CBasePlayer* local, Vec3 start, Vec3& end, unsigned int mask) {

	Ray_t ray;
	CTraceFilter filter;
	CGameTrace tr;
	filter.pSkip = local;
	ray.Init(start, end);
	IEngineTrace::factory()->TraceRay(ray, mask, &filter, &tr);

	return ((tr.m_pEntity == this) || !tr.DidHit());
}

bool CBasePlayer::isScout() {
	return GetClassNum() == TF2_Scout; //////// wtf is thsi?????????
}

bool CBasePlayer::isSoldier() {
	return GetClassNum() == TF2_Soldier;
}

bool CBasePlayer::isPyro() {
	return GetClassNum() == TF2_Pyro;
}

bool CBasePlayer::isDemo() {
	return GetClassNum() == TF2_Demoman;
}

bool CBasePlayer::isHeavy() {
	return GetClassNum() == TF2_Heavy;
}

bool CBasePlayer::isEngineer() {
	return GetClassNum() == TF2_Engineer;
}

bool CBasePlayer::isMedic() {
	return GetClassNum() == TF2_Medic;
}

bool CBasePlayer::isSniper() {
	return GetClassNum() == TF2_Sniper;
}

bool CBasePlayer::isSpy() {
	return GetClassNum() == TF2_Spy;
}