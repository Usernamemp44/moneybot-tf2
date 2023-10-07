#pragma once
#include "../CBaseEntity/CBaseEntity.h"
#include "../CBaseCombatWeapon/CBaseCombatWeapon.h"
extern matrix3x4 no_interp_bone_matrix[256][256];
class CBasePlayer : public CBaseEntity {
public:

	int GetMaxHealth();
	Vec3 GetEyePos();
	void GetBoneMatrix(matrix3x4 boneMatrix[256], int v);
	int GetMoveType();
	int GetHealth();

	int GetFlags();
	void SetFlags(int nFlags);
	BYTE GetLifeState();
	uint8_t GetWaterLevel();
	int GetTickBase();
	int GetClassNum();
	int GetCond();
	int GetCondEx();
	int GetCondEx2();
	int GetCondEx3();
	bool IsInCond(int eCond);
	void SetCurrentCmd(CUserCmd * cmd);
	Vec3 GetVecPunchAngle();
	void RemoveCond(int cond);
	CUserCmd * GetCurrentCommand();
	Vec3 GetEyeAngles();
	void SetEyeAngles(Vec3 & v);
	bool SetupBones(matrix3x4 * pBoneToWorldOut, int nMaxBones, int nBoneMask, float fCurrentTime);
	CBoneCache * GetBoneCache(void);
	Vec3 GetHitboxPos(int nHitbox, bool uninterpolated = false);
	Vec3 GetVelocity();
	Vec3 GetBaseVelocity();
	int AllowAutoMovement();
	float ModelScale();
	Vec3 GetHitboxPos(const char * name, bool uninterpolated = false);
	int GetNumOfHitboxes();
	Vec3 GetBonePos(int nBone);
	CBaseCombatWeapon * GetActiveWeapon();
	bool IsVisible(CBasePlayer* local, Vec3 start, Vec3& end, unsigned int mask = MASK_SHOT);

	float GetStepSize();

	float GetMaxSpeed();


	bool isScout();
	bool isSoldier();
	bool isPyro();
	bool isDemo();
	bool isHeavy();
	bool isEngineer();
	bool isMedic();
	bool isSniper();
	bool isSpy();
};