#pragma once
#include "../BaseCheat.h"
struct CUserCmd_t;
class CBasePlayer;
class Aimbot : public BaseCheat {
	public:
		Aimbot();
		enum TargetType {
			HOSTILE,
			FRIENDLY,
			BULDING
		};
		struct AimbotTarget {
			CBasePlayer* pPlayer = nullptr;
			Vec3 vecHitbox{};
			Vec3 vecPos{};
			float flFov{};
			float flDistance{};
			int iTargetType{};
			int iHp;
			bool isMoving;
		};
		Vec3 RunMultipoint(CBasePlayer* pPlayer, int curHitbox);
		void CreateMove(CUserCmd_t*&) override;
		void AutoBackStab(CUserCmd_t* cmd);
		void RenderMenu() override;
		float GetFieldOfView(Vec3 angle, Vec3 source, Vec3 distance);
		void AddTargets(CUserCmd_t* cmd);
		void GetTargets(CUserCmd* cmd);
		int SelectTarget(CUserCmd_t* cmd);
		int SelectHitbox(CUserCmd_t* cmd, CBasePlayer* pPlayer);
		bool AimbotEnabled;
		float AimbotFOV;
		bool AutoShoot;
		bool BulletNiggers;		
};