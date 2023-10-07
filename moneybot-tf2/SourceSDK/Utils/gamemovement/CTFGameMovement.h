#pragma once
#include "../../../Cheats/BaseCheat.h"
#include "../../Entities/CBaseEntity/CBaseEntity.h"
#include "../../Entities/CBasePlayer/CBasePlayer.h"

namespace CTFGameMovement
{

	bool Init(CBasePlayer *pl);

	bool CheckWater();
	void CheckWaterJump();
	void WaterMove();
	bool InWater();
	void FullWalkMoveUnderwater();
	void PreventBunnyJumping();
	bool CheckJumpButton();
	void FullWalkMove();
	void FinishGravity();
	void StepMove(Vec3 &vecDestination, CGameTrace &trace);
	int  TryPlayerMove(Vec3 *pFirstDest, CGameTrace *pFirstTrace);
	bool CanAccelerate();
	void Accelerate(Vec3 &wishdir, float wishspeed, float accel);
	void Friction();
	void AirAccelerate(Vec3 &wishdir, float wishspeed, float accel);
	void AirMove();
	void WalkMove();
	void CheckVelocity();
	void StartGravity();

	void  TracePlayerBBox(Vec3, Vec3, unsigned fMask, int collisionGroup, CGameTrace &pm);
	int   ClipVelocity(Vec3 &in, Vec3 &normal, Vec3 &out, float overbounce);
	float GetAirSpeedCap();
	void  WaterJump();
	void  CategorizePosition();
	void  SetGroundEntity(CGameTrace *pm);

	struct MoveInfo
	{
		Vec3 m_vecViewAngles{};
		Vec3  m_vecVelocity{};
		Vec3  m_vecOrigin{};
		float         m_flForwardMove{};
		float         m_flSideMove{};
		float         m_flMaxSpeed{};
		float         m_surfaceFriction{1.f};
		int     flags{};
		float         degrees_per_tick{0.f};
		bool          is_decelerating{false};

		void Reset()
		{
			m_vecViewAngles   = {};
			m_vecVelocity     = {};
			m_vecOrigin       = {};
			m_flForwardMove   = 0.f;
			m_flSideMove      = 0.f;
			m_flMaxSpeed      = 0.f;
			m_surfaceFriction = 1.f;
			flags             = 0;
			degrees_per_tick  = 0.f;
			is_decelerating   = false;
		}
	};

	inline MoveInfo    move{};
	inline CBasePlayer *player{nullptr};

	static MoveInfo *GetMove()
	{
		return &move;
	}

	static void SetPlayer(CBasePlayer *pl)
	{
		player = pl;
	}

	static CBasePlayer *GetPlayer()
	{
		return player;
	}
};
