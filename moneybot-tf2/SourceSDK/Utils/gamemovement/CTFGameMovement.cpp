#define NOMINMAX

#include "../../Misc/Defines.h"
#include "CTFGameMovement.h"
#include "../../Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../Interfaces/ConVar/ConVar.h"
#include "../Misc/Math.h"
#include "../../Interfaces/IEngineTrace/IEngineTrace.h"
#include "../../../Cheats/Aimbot/Backtracking/LagRecords/LagRecords.h"
#include "../../../Cheats/CheatList.h"

#include <mutex>
#include "../../Interfaces/IVDebugOverlay/IVDebugOverlay.h"

bool IsNaN(float x)
{
	constexpr int nanmask = 255 << 23;
	return (*reinterpret_cast<int*>(&x) & nanmask) == nanmask;
};

inline void VectorMAInline(const float *start, float scale, const float *direction, float *dest)
{
	dest[0] = start[0] + direction[0] * scale;
	dest[1] = start[1] + direction[1] * scale;
	dest[2] = start[2] + direction[2] * scale;
}

inline void VectorMAInline(const Vec3 &start, float scale, const Vec3 &direction, Vec3 &dest)
{
	dest.x = start.x + direction.x * scale;
	dest.y = start.y + direction.y * scale;
	dest.z = start.z + direction.z * scale;
}

inline void VectorMA(const Vec3 &start, float scale, const Vec3 &direction, Vec3 &dest)
{
	VectorMAInline(start, scale, direction, dest);
}

inline void VectorMA(const float *start, float scale, const float *direction, float *dest)
{
	VectorMAInline(start, scale, direction, dest);
}

inline void VectorCopy(Vec3& first, Vec3& second) {
	second.x = first.x;
	second.y = first.y;
	second.z = first.z;
}

float GetCurrentGravity() {
	return ICvar::factory()->FindVar("sv_gravity")->GetFloat();
}

// this is limited by the network fractional bits used for coords
// because net coords will be only be accurate to 5 bits fractional
// Standard collision test epsilon
// 1/32nd inch collision epsilon
constexpr float DIST_EPSILON{0.03125f};

Vec3 vec3_origin{};

// this is limited by the network fractional bits used for coords
// because net coords will be only be accurate to 5 bits fractional
// Standard collision test epsilon
// 1/32nd inch collision epsilon
#define TF_MAX_SPEED   (400 * 1.3)
constexpr int MAX_CLIP_PLANES = 5;

constexpr float TIME_TO_UNDUCK    = 0.2f;
constexpr float TIME_TO_UNDUCK_MS = 200.0f;

constexpr float GAMEMOVEMENT_DUCK_TIME          = 1000.0f;                  // ms
constexpr float GAMEMOVEMENT_JUMP_TIME          = 510.0f;                   // ms approx - based on the 21 unit height jump
constexpr float GAMEMOVEMENT_JUMP_HEIGHT        = 21.0f;                    // units
constexpr float GAMEMOVEMENT_TIME_TO_UNDUCK     = TIME_TO_UNDUCK * 1000.0f; // ms
constexpr float GAMEMOVEMENT_TIME_TO_UNDUCK_INV = GAMEMOVEMENT_DUCK_TIME - GAMEMOVEMENT_TIME_TO_UNDUCK;


// Only allow bunny jumping up to 1.2x server / player maxspeed setting
constexpr float BUNNYJUMP_MAX_SPEED_FACTOR = 1.2f;

constexpr int COLLISION_GROUP_PLAYER_MOVEMENT = 8;

enum
{
	SPEED_CROPPED_RESET = 0,
	SPEED_CROPPED_DUCK = 1,
	SPEED_CROPPED_WEAPON = 2,
};


enum
{
	WL_NOT_IN_WATER = 0,
	WL_FEET,
	WL_WAIST,
	WL_EYES
};

bool g_bMovementOptimizations = true;

struct mins_and_maxs
{
	Vec3 mins;
	Vec3 maxs;
};

const auto GetScales = [](CBasePlayer *player)-> mins_and_maxs {
	const Vec3 m_vHullMin{Vec3(-23.9f, -23.9f, 0.f)};
	const Vec3 m_vHullMax{Vec3(23.9f, 23.9f, 81.9f)};

	return {m_vHullMin * player->ModelScale(), m_vHullMax * player->ModelScale()};
};

#define PRINT_FUNCTION \
	//Game::ConsolePrint({}, __FUNCTION__); \
	Game::ConsolePrint({0,255,255}, fmt::format(" m_vecVelocity {:.2f} {:.2f} {:.2f}", CTFGameMovement::GetMove()->m_vecVelocity.x, CTFGameMovement::GetMove()->m_vecVelocity.y, CTFGameMovement::GetMove()->m_vecVelocity.z).c_str());


int CTFGameMovement::ClipVelocity(Vec3& in, Vec3& normal, Vec3& out, float overbounce) {

	const float angle = normal[2];

	int blocked = 0x00; // Assume unblocked

	if (angle > 0)
		blocked |= 0x01; // Posotive z component, assume its a floor.

	if (angle <= 0)
		blocked |= 0x02; // No z, it is a wall / step.

	const float backoff = in.Dot(normal) * overbounce;

	for (int i = 0; i < 3; i++) {
		const float change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	const float adjust = out.Dot(normal);
	if (adjust < 0.0f) {
		out -= normal * adjust;
	}

	return blocked;

}

int CTFGameMovement::TryPlayerMove(Vec3* pFirstDest, CGameTrace* pFirstTrace) {

	Vec3 dir{};
	float d;
	Vec3 planes[MAX_CLIP_PLANES];
	Vec3 primal_velocity{}, original_velocity{};
	Vec3 new_velocity{};
	int i, j;
	CGameTrace pm;
	Vec3 end{};

	int numbumps = 4;
	int blocked = 0;
	int numplanes = 0;

	VectorCopy(GetMove()->m_vecVelocity, original_velocity);
	VectorCopy(GetMove()->m_vecVelocity, primal_velocity);

	float allFraction = 0;
	float time_left = GlobalVars()->TickInterval;

	for (int bumpcount = 0; bumpcount < numbumps; bumpcount++) {

		if (GetMove()->m_vecVelocity.Length() == 0.0)
			break;

		VectorMA(GetMove()->m_vecVelocity, time_left, GetMove()->m_vecVelocity, end);

		if (pFirstDest && end == *pFirstDest) {
			pm = *pFirstTrace;
		}
		else {
			TracePlayerBBox(GetMove()->m_vecOrigin, end, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm);
		}

		allFraction += pm.fraction;

		if (pm.allsolid) {

			VectorCopy(vec3_origin, GetMove()->m_vecVelocity);
			return 4;
		}

		if (pm.fraction > 0) {
			if (numbumps > 0 && pm.fraction == 1) {

				CGameTrace stuck;
				TracePlayerBBox(pm.endpos, pm.endpos, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, stuck);
				if (stuck.startsolid || stuck.fraction != 1.0f) {
					VectorCopy(vec3_origin, GetMove()->m_vecVelocity);
					break;
				}

			}

			GetMove()->m_vecOrigin = pm.endpos;
			VectorCopy(GetMove()->m_vecVelocity, original_velocity);
			numplanes = 0;
		}

		if (pm.fraction == 1)
			break;

		if (pm.plane.normal[2] > 0.7) {
			blocked |= 1;
		}

		if (!pm.plane.normal[2]) {
			blocked |= 2;
		}

		time_left -= time_left * pm.fraction;

		if (numplanes >= MAX_CLIP_PLANES) {
			VectorCopy(vec3_origin, GetMove()->m_vecVelocity);

			break;
		}

		VectorCopy(pm.plane.normal, planes[numplanes]);
		numplanes++;

		if (numplanes == 1 && GetPlayer()->GetMoveType() == MOVETYPE_WALK && !(GetMove()->flags & FL_ONGROUND)) {

			for (i = 0; i < numplanes; i++) {
				if (planes[i][2] > 0.7f) {
					ClipVelocity(original_velocity, planes[i], new_velocity, 1);
					VectorCopy(new_velocity, original_velocity);
				}
				else {
					static ConVar* sv_bounce = ICvar::factory()->FindVar("sv_bounce");

					ClipVelocity(original_velocity, planes[i], new_velocity, 1.0f + sv_bounce->GetFloat() * (1.0f - GetMove()->m_surfaceFriction));
				}
			}

			VectorCopy(new_velocity, GetMove()->m_vecVelocity);
			VectorCopy(new_velocity, original_velocity);

		}

		else {
			for (i = 0; i < numplanes; i++) {
				ClipVelocity(original_velocity, planes[i], GetMove()->m_vecVelocity, 1);

				for (j = 0; j < numplanes; j++) {
					if (j != i) {
						if (GetMove()->m_vecVelocity.Dot(planes[j]) < 0)
							break;
					}
				}

				if (j == numplanes)
					break;
			}

			if (i != numplanes) {

			}
			else {
				if (numplanes != 2) {
					VectorCopy(vec3_origin, GetMove()->m_vecVelocity);
					break;
				}

				dir = planes[0].Cross(planes[1]);
				dir.Normalize();
				d = dir.Dot(GetMove()->m_vecVelocity);

				GetMove()->m_vecVelocity = dir * d;
			}

			d = GetMove()->m_vecVelocity.Dot(primal_velocity);
			if (d <= 0) {
				VectorCopy(vec3_origin, GetMove()->m_vecVelocity);
				break;
			}
		}
	}

	if (allFraction == 0) {
		VectorCopy(vec3_origin, GetMove()->m_vecVelocity);
	}

	return blocked;

}

bool CTFGameMovement::CanAccelerate() {
	return true;
}

void CTFGameMovement::Accelerate(Vec3& wishdir, float wishspeed, float accel) {

	if (!CanAccelerate())
		return;

	const float currentspeed = GetMove()->m_vecVelocity.Dot(wishdir);

	const float addspeed = wishspeed - currentspeed;

	if (addspeed <= 0)
		return;

	float accelspeed = accel * GlobalVars()->TickInterval * wishspeed * GetMove()->m_surfaceFriction;

	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (int i = 0; i < 3; i++) {
		GetMove()->m_vecVelocity[i] += accelspeed * wishdir[i];
	}

}

void CTFGameMovement::AirAccelerate(Vec3& wishdir, float wishspeed, float accel) {

	float wishspd = wishspeed;

	if (GetPlayer()->GetLifeState() == LIFE_DEAD)
		return;


	if (wishspd > GetAirSpeedCap())
		wishspd = GetAirSpeedCap();

	const float currentspeed = GetMove()->m_vecVelocity.Dot(wishdir);

	const float addspeed = wishspd - currentspeed;

	if (addspeed <= 0)
		return;

	float accellspeed = accel * wishspeed * GlobalVars()->TickInterval * GetMove()->m_surfaceFriction;

	if (accellspeed > addspeed)
		accellspeed = addspeed;

	
	for (int i = 0; i < 3; i++) {
		GetMove()->m_vecVelocity[i] += accellspeed * wishdir[i];
	}

}

void CTFGameMovement::AirMove() {

	Vec3 wishvel{};
	Vec3 wishdir{};
	Vec3 forward{}, right{}, up{};

	Math::AngleVectors(GetMove()->m_vecViewAngles, &forward, &right, &up);

	const float fmove = GetMove()->m_flForwardMove;
	const float smove = GetMove()->m_flSideMove;

	forward[2] = 0;
	right[2] = 0;
	forward.Normalize();
	right.Normalize();

	for (int i = 0; i < 2; i++)
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);

	float wishspeed = wishdir.Normalize();

	if (wishspeed != 0.f && wishspeed > GetMove()->m_flMaxSpeed) {
		wishvel *= GetMove()->m_flMaxSpeed / wishspeed;
		wishspeed = GetMove()->m_flMaxSpeed;
	}

	static ConVar* sv_airaccelerate = ICvar::factory()->FindVar("sv_airaccelerate");

	float flAcceleration = sv_airaccelerate->GetFloat();

	if (GetPlayer()->IsInCond(128) /* TF_COND_AIR_CURRENT */) {

		static ConVar* tf_movement_aircurrent_aircontrol_mult = ICvar::factory()->FindVar("tf_movement_aircurrent_aircontrol_mult");

		flAcceleration *= tf_movement_aircurrent_aircontrol_mult->GetFloat();


	}

	AirAccelerate(wishdir, wishspeed, flAcceleration);

	GetMove()->m_vecVelocity -= GetPlayer()->GetBaseVelocity();

	const int iBlocked = TryPlayerMove(nullptr, nullptr);

	GetMove()->m_vecVelocity -= GetPlayer()->GetBaseVelocity();
}

void CTFGameMovement::StepMove(Vec3& vecDestination, CGameTrace& trace) {

	CGameTrace saveTrace;
	saveTrace = trace;

	Vec3 vecEndPos{};
	VectorCopy(vecDestination, vecEndPos);
	
	Vec3 vecPos{}, vecVel{};
	VectorCopy(GetMove()->m_vecOrigin, vecPos);
	VectorCopy(GetMove()->m_vecVelocity, vecVel);

	bool bLowRoad = false;
	bool bUpRoad = true;

	if (GetPlayer()->AllowAutoMovement()) {

		VectorCopy(GetMove()->m_vecOrigin, vecEndPos);
		vecEndPos.z += GetPlayer()->GetStepSize();
		TracePlayerBBox(GetMove()->m_vecOrigin, vecEndPos, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, trace);

		if (!trace.startsolid && !trace.allsolid) {
			GetMove()->m_vecOrigin = trace.endpos;
		}

		TryPlayerMove(nullptr, nullptr);

		VectorCopy(GetMove()->m_vecOrigin, vecEndPos);
		vecEndPos.z -= GetPlayer()->GetStepSize();
		TracePlayerBBox(GetMove()->m_vecOrigin, vecEndPos, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, trace);

		if (!trace.startsolid && !trace.allsolid) {
			GetMove()->m_vecOrigin = trace.endpos;
		}

		if (trace.fraction != 1.0f &&
			trace.plane.normal[2] < 0.7f || GetMove()->m_vecOrigin == vecPos) {
			bLowRoad = true;
			bUpRoad = false;
		}

	}
	else {
		bLowRoad = true;
		bUpRoad = false;
	}

	if (bLowRoad) {
		Vec3 vecUpPos{}, vecUpVel{};
		if (bUpRoad) {
			VectorCopy(GetMove()->m_vecOrigin, vecUpPos);
			VectorCopy(GetMove()->m_vecVelocity, vecUpVel);
		}

		// Take the "low" road
		GetMove()->m_vecOrigin = vecPos;
		VectorCopy(vecVel, GetMove()->m_vecVelocity);
		VectorCopy(vecDestination, vecEndPos);
		TryPlayerMove(&vecEndPos, &saveTrace);

		Vec3 vecDownPos{}, vecDownVel{};
		VectorCopy(GetMove()->m_vecOrigin, vecDownPos);
		VectorCopy(GetMove()->m_vecVelocity, vecDownVel);

		if (bUpRoad) {
			const float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
			const float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);

			// decide which one went farther
			if (flUpDist >= flDownDist)
			{
				GetMove()->m_vecOrigin = vecUpPos;
				VectorCopy(vecUpVel, GetMove()->m_vecVelocity);

				// copy z value from the Low Road move
				GetMove()->m_vecVelocity.z = vecDownVel.z;
			}
		}
	}
}

void CTFGameMovement::TracePlayerBBox(Vec3 start, Vec3 end, unsigned fMask, int collisionGroup, CGameTrace& pm) {

	const Vec3 raise{ 0, 0, DIST_EPSILON * 2 };

	Ray_t ray;
	ray.Init(start + raise, end + raise, GetScales(GetPlayer()).mins, GetScales(GetPlayer()).maxs);

	CTraceFilter filter;
	filter.pSkip = GetPlayer();
	IEngineTrace::factory()->TraceRay(ray, fMask, &filter, &pm);
}

void CTFGameMovement::WalkMove() {

	Vec3 vecForward{}, vecRight{}, vecUp{};
	Math::AngleVectors(GetMove()->m_vecViewAngles, &vecForward, &vecRight, &vecUp);
	vecForward.z = 0.0f;
	vecRight.z = 0.0f;
	vecForward.Normalize();
	vecRight.Normalize();

	const float flForwardMove = GetMove()->m_flForwardMove;
	const float flSideMove = GetMove()->m_flSideMove;

	Vec3 vecWishDirection((vecForward.x * flForwardMove + vecRight.x * flSideMove),
		(vecForward.y * flForwardMove + vecRight.y * flSideMove),
		0.0f);

	
	float flWishSpeed = vecWishDirection.Normalize();

	static ConVar* sv_accelerate = ICvar::factory()->FindVar("sv_accelerate");
	static ConVar* sv_friction = ICvar::factory()->FindVar("sv_friction");
	static ConVar* sv_stopspeed = ICvar::factory()->FindVar("sv_stopspeed");
	static ConVar* tf_clamp_back_speed = ICvar::factory()->FindVar("tf_clamp_back_speed");
	static ConVar* tf_clamp_back_speed_min = ICvar::factory()->FindVar("tf_clamp_back_speed_min");

	float flAcceleartion = sv_accelerate->GetFloat();

	if (flWishSpeed > 0) {
		const float newFriction = GetMove()->m_surfaceFriction * sv_friction->GetFloat();

		if (flWishSpeed < 100.0f * newFriction / flAcceleartion && GetPlayer()->IsInCond(127)  /*TF_COND_LOST_FOOTING*/) {
			const float flSpeed = GetMove()->m_vecVelocity.Length();

			flAcceleartion = fmaxf(flSpeed, sv_stopspeed->GetFloat() * newFriction / flWishSpeed) + 1.0f;
		}
	}

	GetMove()->m_vecVelocity.z = 0;
	Accelerate(vecWishDirection, flWishSpeed, flAcceleartion);

	float flNewSpeed = GetMove()->m_vecVelocity.Length();
	if (flNewSpeed > GetMove()->m_flMaxSpeed) {
		const float flScale = GetMove()->m_flMaxSpeed / flNewSpeed;
		GetMove()->m_vecVelocity.x *= flScale;
		GetMove()->m_vecVelocity.y *= flScale;
	}


	if (tf_clamp_back_speed->GetFloat() < 1.0f && GetMove()->m_vecVelocity.Length() > tf_clamp_back_speed_min->GetFloat())
	{
		const float flDot = vecForward.Dot(GetMove()->m_vecVelocity);

		// are we moving backwards at all?
		if (flDot < 0)
		{
			Vec3       vecBackMove = vecForward * flDot;
			const Vec3 vecRightMove = vecRight * vecRight.Dot(GetMove()->m_vecVelocity);

			// clamp the back move Vec3 if it is faster than max
			const float flBackSpeed = vecBackMove.Length();
			const float flMaxBackSpeed = GetMove()->m_flMaxSpeed * tf_clamp_back_speed->GetFloat();

			if (flBackSpeed > flMaxBackSpeed)
			{
				vecBackMove *= flMaxBackSpeed / flBackSpeed;
			}

			// reassemble velocity	
			GetMove()->m_vecVelocity = vecBackMove + vecRightMove;

			// Re-run this to prevent crazy values (clients can induce this via usercmd viewangles hacking)
			flNewSpeed = GetMove()->m_vecVelocity.Length();
			if (flNewSpeed > GetMove()->m_flMaxSpeed)
			{
				const float flScale = GetMove()->m_flMaxSpeed / flNewSpeed;
				GetMove()->m_vecVelocity.x *= flScale;
				GetMove()->m_vecVelocity.y *= flScale;
			}
		}
	}
	// Add base velocity to the player's current velocity - base velocity = velocity from conveyors, etc.
	GetMove()->m_vecVelocity += GetPlayer()->GetBaseVelocity();

	// Calculate the current speed and return if we are not really moving.
	const float flSpeed = GetMove()->m_vecVelocity.Length();
	if (flSpeed < 1.0f)
	{
		// I didn't remove the base velocity here since it wasn't moving us in the first place.
		GetMove()->m_vecVelocity = vec3_origin;
		return;
	}

	// Calculate the destination.
	Vec3 vecDestination{
		GetMove()->m_vecOrigin.x + GetMove()->m_vecVelocity.x * GlobalVars()->TickInterval,
		GetMove()->m_vecOrigin.y + GetMove()->m_vecVelocity.y * GlobalVars()->TickInterval,
		GetMove()->m_vecOrigin.z
	};

	// Try moving to the destination.
	CGameTrace trace;
	TracePlayerBBox(GetMove()->m_vecOrigin, vecDestination, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, trace);
	if (trace.fraction == 1.0f)
	{
		// Made it to the destination (remove the base velocity).
		GetMove()->m_vecOrigin = trace.endpos;
		GetMove()->m_vecVelocity -= GetPlayer()->GetBaseVelocity();

		return;
	}

	// Now try and do a step move.
	StepMove(vecDestination, trace);

	// Remove base velocity.
	const Vec3 baseVelocity = GetPlayer()->GetBaseVelocity();
	GetMove()->m_vecVelocity -= baseVelocity;

}

void CTFGameMovement::FinishGravity() {
	GetMove()->m_vecVelocity[2] -= GetCurrentGravity() * GlobalVars()->TickInterval * 0.5f;
	CheckVelocity();
}


bool CTFGameMovement::Init(CBasePlayer* pl) {

	SetPlayer(pl);

	GetMove()->Reset();

	CUserCmd pCurCmd{};
	pCurCmd.ViewAngles = pl->GetEyeAngles();
	Math::ClampAngles(pCurCmd.ViewAngles);

	static LagRecords* lagRecords = (LagRecords*)CheatModules["LagRecords"];

	std::unique_lock<std::mutex> guard(LagRecordsCriticalSection);

	if (lagRecords->LagRecordsDeck[pl->GetIndex()].empty() || lagRecords->LagRecordsDeck[pl->GetIndex()].size() < 3)
		return false;

	auto current_record = lagRecords->LagRecordsDeck[pl->GetIndex()].at(0);
	auto second_record = lagRecords->LagRecordsDeck[pl->GetIndex()].at(1);
	auto third_record = lagRecords->LagRecordsDeck[pl->GetIndex()].at(2);

	guard.unlock();

	bool is_strafing = false;

	bool on_ground = pl->GetFlags() & FL_ONGROUND;

	if (!on_ground /* strafe pred checkbox check */)
	{
		float degrees_per_tick{
			(current_record.eyeangles.y - second_record.eyeangles.y
				+ (second_record.eyeangles.y - third_record.eyeangles.y)) / (2.f - GlobalVars()->TickInterval)
		};

		//Too much, they propably just panicking
		if (std::abs(degrees_per_tick) > 6.f)
			degrees_per_tick = 0.f;

		//y += positive = LEFT
		const auto turning_left{ third_record.eyeangles.y < current_record.eyeangles.y };
		const auto turning_right{ third_record.eyeangles.y > current_record.eyeangles.y };

		if (turning_right)
		{
			//pCurCmd.sidemove = 450.f;
			//pCurCmd.forwardmove = 0.f;

			is_strafing = true;

			GetMove()->degrees_per_tick = -std::abs(degrees_per_tick);
		}
		else if (turning_left)	//towards positive y
		{
			//pCurCmd.sidemove = -450.f;
			//pCurCmd.forwardmove = 0.f;

			is_strafing = true;

			GetMove()->degrees_per_tick = std::abs(degrees_per_tick);
		}

		//Game::ConsolePrint({255, 127, 0}, fmt::format("average_turn_rate {} | turning_left {} | turning_right {}", GetMove()->degrees_per_tick, turning_left, turning_right).c_str());
		//Game::ConsolePrint({255, 127, 0}, fmt::format("last_record vel {}", second_record.velocity.Length2D()).c_str());
		//Game::ConsolePrint({255, 127, 0}, fmt::format("last_record vel {}", third_record->m_vecVelocity.Length2D()).c_str());
	}
	//else
	{
		/*const float first_vel{current_record->anim_state_info.m_DebugAnimData.m_vecMoveYaw.y};
		const float second_vel{second_record->anim_state_info.m_DebugAnimData.m_vecMoveYaw.y};
		const float third_vel{third_record->anim_state_info.m_DebugAnimData.m_vecMoveYaw.y};*/

		//GetMove()->is_decelerating = first_vel < second_vel < third_vel;

		//Game::ConsolePrint({}, fmt::format("first_vel {} second_vel {} third_vel {} is_de-celerating {}", first_vel, second_vel, third_vel, GetMove()->is_decelerating).c_str());

		// convert velocity to angular momentum.
		Vec3 angle{};
		Math::VectorAngles(pl->GetVelocity(), angle);

		// get our current speed of travel.
		const float speed{ pl->GetVelocity().Length() };

		// fix direction by factoring in where we are looking.
		angle.y = pCurCmd.ViewAngles.y - angle.y;

		// convert corrected angle back to a direction.
		Vec3 direction{};
		Math::AngleVectors(angle, &direction);

		const Vec3 move_from_vel{ direction * speed };

		//If we aren't decelerating and on ground, move
		//if (!GetMove()->is_decelerating)
		{
			if (!is_strafing || pl->GetFlags() & FL_ONGROUND)
				pCurCmd.ForwardMove = move_from_vel.x;

			pCurCmd.SideMove = move_from_vel.y;

			//if (std::abs(move.x) > 450.f / 2.f)
			//pCurCmd.forwardmove = std::clamp(move.x * 450.f, -450.f, 450.f);

			//if (std::abs(move.y) > 450.f / 2.f)
			//pCurCmd.sidemove = std::clamp(move.y * 450.f, -450.f, 450.f);
		}
	}

	//Set these first so we can predict.
	GetMove()->m_vecViewAngles = player->GetEyeAngles();
	GetMove()->m_vecVelocity = player->GetVelocity();
	GetMove()->m_vecOrigin = player->GetVecOrigin();
	GetMove()->m_flForwardMove = pCurCmd.ForwardMove;
	GetMove()->m_flSideMove = pCurCmd.SideMove;
	GetMove()->m_flMaxSpeed = TF_MAX_SPEED;
	GetMove()->flags = pl->GetFlags();

	//Correct velocity for lag
	const int ticks_to_correct{ current_record.chokedticks - 1 };
	if (ticks_to_correct > 0)
	{
		//const Vec3 old_vel{GetMove()->m_vecVelocity};

		//Updates velocity for lag
		for (int i{ 0 }; i < ticks_to_correct; ++i)
		{
			FullWalkMove();
		}

		//Reset
		GetMove()->m_vecViewAngles = player->GetEyeAngles();
		GetMove()->m_vecOrigin = player->GetVecOrigin();
		GetMove()->m_flForwardMove = pCurCmd.ForwardMove;
		GetMove()->m_flSideMove = pCurCmd.SideMove;
		GetMove()->m_flMaxSpeed = TF_MAX_SPEED;
		GetMove()->flags = pl->GetFlags();

		// const char *format{("Corrected velocity over {:d} ticks ({:2f} {:2f} {:2f})")};
		// Game::ConsolePrint({}, fmt::format(format,
		// 								   ticks_to_correct,
		// 								   GetMove()->m_vecVelocity.x - old_vel.x,
		// 								   GetMove()->m_vecVelocity.y - old_vel.y,
		// 								   GetMove()->m_vecVelocity.z - old_vel.z));
	}

	//Check if they are trying to move
	if (GetMove()->flags & FL_ONGROUND)
	{
		//See if velocity went down by friction.
		const Vec3 current_velocity_backup{ GetMove()->m_vecVelocity };
		{
			//Set velocity to last known velocity
			GetMove()->m_vecVelocity = second_record.velocity;

			//Apply 1 tick of friction.
			Friction();

			const Vec3 positive_one(fabs(current_velocity_backup.x), fabs(current_velocity_backup.y), 0.f);
			const Vec3 positive_two(fabs(GetMove()->m_vecVelocity.x), fabs(GetMove()->m_vecVelocity.y), 0.f);

			const float tolerance{ positive_one.Length() - positive_two.Length() };

			// convert velocity to angular momentum.
			Vec3 angle{};
			Math::VectorAngles(current_record.velocity, angle);
			// get our current speed of travel.
			float speed{ current_record.velocity.Length() };
			// fix direction by factoring in where we are looking.
			angle.y = current_record.eyeangles.y - angle.y;
			// convert corrected angle back to a direction.
			Vec3 direction{};
			Math::VectorAngles(angle, direction);
			const Vec3 current_record_move{ direction * speed };

			// convert velocity to angular momentum.
			Math::VectorAngles(second_record.velocity, angle);
			// get our current speed of travel.
			speed = second_record.velocity.Length();
			// fix direction by factoring in where we are looking.
			angle.y = second_record.eyeangles.y - angle.y;
			// convert corrected angle back to a direction.
			Math::AngleVectors(angle, &direction);
			const Vec3 last_record_move{ direction * speed };

			// Game::ConsolePrint({}, fmt::format("current x {:.2f} y {:.2f} | last record x {:.2f} y {:.2f} | diff {:.2f} {:.2f} | tolerance {}",
			// 								   current_record_move.x, current_record_move.y,
			// 								   last_record_move.x, last_record_move.y,
			// 								   current_record_move.x - last_record_move.x,
			// 								   current_record_move.y - last_record_move.y,
			// 								   tolerance));

			const bool is_accelerating{
				fabs(current_record_move.Length() - last_record_move.Length()) <= tolerance * 1.25f
			};

			//There being no velocity difference between current velocity 
			if (!is_accelerating)
			{
				GetMove()->is_decelerating = true;

				GetMove()->m_flSideMove = GetMove()->m_flForwardMove = 0.f;
				//GetMove()->m_flSideMove    = GetMove()->m_flForwardMove = 0.f;

				//Game::ConsolePrint({255, 127, 0}, fmt::format("velocity_diff {:2f} {:2f} [{:2f}]", velocity_diff.x, velocity_diff.y, velocity_diff.length()));
			}
			else
			{
				//Game::ConsolePrint({127, 255, 0}, fmt::format("velocity_diff {:2f} {:2f} [{:2f}]", velocity_diff.x, velocity_diff.y, velocity_diff.length()));
			}

			//Game::ConsolePrint({}, fmt::format("is_accelerating {}", is_accelerating));
		}
		//Restore
		GetMove()->m_vecVelocity = current_velocity_backup;
	}

	//Print move data
	//Game::ConsolePrint({}, fmt::format("::Init is_decelerating{}", ).c_str());
	//GetMove()->Print();

	return true;

}


bool CTFGameMovement::CheckWater() {
	return false;
}

auto TracePlayerBBoxForGround = []
(
	const Vec3 &start,
	const Vec3 &end, 
	const Vec3 &mins_src,
	const Vec3 &maxs_src,
	CBasePlayer* player,
	const unsigned int mask,
	CGameTrace& pm
	)
{

	//IVDebugOverlay::factory()->AddBoxOverlay(start, mins_src, maxs_src, {}, 255, 0, 255, 20, 2.0f);

	Ray_t ray;

	float fraction = pm.fraction;
	Vec3 endpos = pm.endpos;

	Vec3 mins = mins_src;
	Vec3 maxs = Vec3(std::min(0.f, maxs_src.x), std::min(0.f, maxs_src.y), maxs_src.z);
	ray.Init(start, end, mins, maxs);

	CTraceFilter filter;
	IEngineTrace::factory()->TraceRay(ray, mask, &filter, &pm);

	if (pm.m_pEntity && pm.plane.normal[2] >= 0.7) {
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	// Check the +x, +y quadrant
	mins = Vec3(std::max(0.f, mins_src.x), std::max(0.f, mins_src.y), mins_src.z);
	maxs = maxs_src;
	ray.Init(start, end, mins, maxs);

	IEngineTrace::factory()->TraceRay(ray, mask, &filter, &pm);

	if (pm.m_pEntity && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	// Check the -x, +y quadrant
	mins = Vec3(mins_src.x, std::max(0.f, mins_src.y), mins_src.z);
	maxs = Vec3(std::min(0.f, maxs_src.x), maxs_src.y, maxs_src.z);
	ray.Init(start, end, mins, maxs);

	IEngineTrace::factory()->TraceRay(ray, mask, &filter, &pm);

	if (pm.m_pEntity && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	// Check the +x, -y quadrant
	mins = Vec3(std::max(0.f, mins_src.x), mins_src.y, mins_src.z);
	maxs = Vec3(maxs_src.x, std::min(0.f, maxs_src.y), maxs_src.z);

	ray.Init(start, end, mins, maxs);

	IEngineTrace::factory()->TraceRay(ray, mask, &filter, &pm);

	if (pm.m_pEntity && pm.plane.normal[2] >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos = endpos;
		return;
	}

	pm.fraction = fraction;
	pm.endpos = endpos;
};


void CTFGameMovement::CategorizePosition() {

	GetMove()->m_surfaceFriction = 1.f;

	if (GetMove()->m_vecVelocity.z > 250.0f) {
		SetGroundEntity(nullptr);
		return;
	}

	const Vec3 vecStartPos = GetMove()->m_vecOrigin;
	Vec3 vecEndPos{ vecStartPos - Vec3{0, 0, 2.0f} };

	const bool bUnderwater = GetPlayer()->GetWaterLevel() >= WL_EYES;
	bool bMoveToEndPos = false;

	if (GetPlayer()->GetMoveType() == MOVETYPE_WALK && GetMove()->flags & FL_ONGROUND) {
		vecEndPos.z -= GetPlayer()->GetStepSize();
		bMoveToEndPos = true;
	}

	CGameTrace trace;
	TracePlayerBBox(vecStartPos, vecEndPos, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, trace);

	//IVDebugOverlay::factory()->AddBoxOverlay(vecStartPos, Vec3(-2, -2, -2), Vec3(2, 2, 2), Vec3(0, 0, 0), 255, 255, 255, 255, 5.f);
	//IVDebugOverlay::factory()->AddBoxOverlay(vecEndPos, Vec3(-2, -2, -2), Vec3(2, 2, 2), Vec3(0, 0, 0), 0, 255, 0, 255, 5.f);

	if (trace.plane.normal.z < 0.7f) {

		TracePlayerBBoxForGround(vecStartPos, vecEndPos, GetScales(GetPlayer()).maxs, GetScales(GetPlayer()).mins, GetPlayer(), MASK_PLAYERSOLID, trace);

		if (trace.plane.normal[2] < 0.7f) {

			SetGroundEntity(nullptr);

			if (GetMove()->m_vecVelocity.z > 0.0f && GetPlayer()->GetMoveType() != MOVETYPE_NOCLIP) {
				GetMove()->m_surfaceFriction = 0.25f;
			}
		}
		else {
			SetGroundEntity(&trace);
		}
	}
	else {
		// YWB:  This logic block essentially lifted from StayOnGround implementation
		if (bMoveToEndPos &&
			!trace.startsolid &&     // not sure we need this check as fraction would == 0.0f?
			trace.fraction > 0.0f && // must go somewhere
			trace.fraction < 1.0f)   // must hit something
		{
			Vec3 org = GetMove()->m_vecOrigin;
			org.z = trace.endpos.z;
			GetMove()->m_vecOrigin = org;
		}
		else
		{
			//Game::ConsolePrint({255, 0, 0}, "CategorizePosition StayOnground failed");
		}
		SetGroundEntity(&trace);
	}
}

void CTFGameMovement::SetGroundEntity(CGameTrace* pm) {

	CBaseEntity* newGround = pm ? pm->m_pEntity : nullptr;

	if (newGround) {
		GetMove()->m_vecVelocity.z = 0.0f;
		GetMove()->flags |= FL_ONGROUND;
	}
	else {
		GetMove()->flags &= ~FL_ONGROUND;
	}
}

void CTFGameMovement::CheckWaterJump() {

}


void CTFGameMovement::WaterMove() {
	int     i;
	Vec3  wishvel{};
	Vec3  wishdir{};
	Vec3  start{}, dest{};
	Vec3  temp{};
	CGameTrace pm;
	float   newspeed;
	Vec3  forward{}, right{}, up{};

	static ConVar* sv_friction = ICvar::factory()->FindVar("sv_friction");
	static ConVar* sv_accelerate = ICvar::factory()->FindVar("sv_accelerate");

	Math::AngleVectors(GetMove()->m_vecViewAngles, &forward, &right, &up); // Determine movement angles

	//
	// user intentions
	//
	for (i = 0; i < 3; i++)
	{
		wishvel[i] = forward[i] * GetMove()->m_flForwardMove + right[i] * GetMove()->m_flSideMove;
	}


	// if we have the jump key down, move us up as well
	/*if (GetMove()->m_nButtons & IN_JUMP)
	{
		wishvel[2] += GetMove()->m_flClientMaxSpeed;
	}
	else */
	if (GetMove()->m_flForwardMove == 0.f && GetMove()->m_flSideMove == 0.f) // Sinking after no other movement occurs
	{
		wishvel[2] -= 60; // drift towards bottom
	}
	//else // Go straight up by upmove amount.
	//{
	//	// exaggerate upward movement along forward as well
	//	float upwardMovememnt = GetMove()->m_flForwardMove * forward.z * 2;
	//	upwardMovememnt       = std::clamp(upwardMovememnt, 0.f, GetMove()->m_flClientMaxSpeed);
	//	wishvel[2] += GetMove()->m_flUpMove + upwardMovememnt;
	//}
	

	// Copy it over and determine speed
	VectorCopy(wishvel, wishdir);
	wishdir.Normalize();
	float wishspeed = wishdir.Length();

	// Cap speed.
	if (wishspeed > GetMove()->m_flMaxSpeed)
	{
		wishvel *= GetMove()->m_flMaxSpeed / wishspeed;
		wishspeed = GetMove()->m_flMaxSpeed;
	}


	// Water friction
	VectorCopy(GetMove()->m_vecVelocity, temp);
	temp.Normalize();
	const float speed = temp.Length();
	if (speed != 0.f)
	{
		newspeed = speed - GlobalVars()->TickInterval * speed * sv_friction->GetFloat() * GetMove()->m_surfaceFriction;
		if (newspeed < 0.1f)
		{
			newspeed = 0;
		}

		GetMove()->m_vecVelocity *= newspeed / speed;
	}
	else
	{
		newspeed = 0;
	}

	if (GetPlayer()->IsInCond(77) /*TF_COND_HALLOWEEN_GHOST_MODE*/)
	{
		wishvel.Normalize();
		const float accelspeed = sv_accelerate->GetFloat() * wishspeed * GlobalVars()->TickInterval * GetMove()->m_surfaceFriction;

		for (i = 0; i < 3; i++)
		{
			const float deltaSpeed = accelspeed * wishvel[i];
			GetMove()->m_vecVelocity[i] += deltaSpeed;
		}

		const float speedSqrt = sqrtf(GetMove()->m_vecVelocity.x * GetMove()->m_vecVelocity.x + GetMove()->m_vecVelocity.y * GetMove()->m_vecVelocity.y);

		static ConVar* tf_ghost_xy_speed = ICvar::factory()->FindVar("tf_ghost_xy_speed");

		if (speedSqrt > tf_ghost_xy_speed->GetFloat())
		{
			const float normal = tf_ghost_xy_speed->GetFloat() / speedSqrt;
			GetMove()->m_vecVelocity.x = GetMove()->m_vecVelocity.x * normal;
			GetMove()->m_vecVelocity.y = GetMove()->m_vecVelocity.y * normal;
		}
	}
	else if (wishspeed >= 0.1f) // water acceleration
	{
		const float addspeed = wishspeed - newspeed;
		if (addspeed > 0)
		{
			wishvel.Normalize();

			float accelspeed = sv_accelerate->GetFloat() * wishspeed * GlobalVars()->TickInterval * GetMove()->m_surfaceFriction;
			if (accelspeed > addspeed)
			{
				accelspeed = addspeed;
			}

			for (i = 0; i < 3; i++)
			{
				const float deltaSpeed = accelspeed * wishvel[i];
				GetMove()->m_vecVelocity[i] += deltaSpeed;
			}
		}
	}

	GetMove()->m_vecVelocity += GetPlayer()->GetBaseVelocity();

	// Now move
	// assume it is a stair or a slope, so press down from stepheight above
	VectorMA(GetMove()->m_vecOrigin, GlobalVars()->TickInterval, GetMove()->m_vecVelocity, dest);

	TracePlayerBBox(GetMove()->m_vecOrigin, dest, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm);
	if (pm.fraction == 1.0f)
	{
		VectorCopy(dest, start);
		if (true)
		{
			start[2] += GetPlayer()->GetStepSize() + 1;
		}

		TracePlayerBBox(start, dest, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm);

		if (!pm.startsolid && !pm.allsolid)
		{
			// walked up the step, so just keep result and exit
			GetMove()->m_vecOrigin = pm.endpos;
			GetMove()->m_vecVelocity -= GetPlayer()->GetBaseVelocity();
			return;
		}

		// Try moving straight along out normal path.
		TryPlayerMove(nullptr, nullptr);
	}
	else
	{
		if (!(GetMove()->flags & FL_ONGROUND))
		{
			TryPlayerMove(nullptr, nullptr);

			GetMove()->m_vecVelocity -= GetPlayer()->GetBaseVelocity();

			return;
		}

		StepMove(dest, pm);
	}

	GetMove()->m_vecVelocity -= GetPlayer()->GetBaseVelocity();
}

void CTFGameMovement::WaterJump() {

}

bool CTFGameMovement::InWater() {
	return false;
}

void CTFGameMovement::FullWalkMoveUnderwater() {

	WaterMove();

	CategorizePosition();

	if (GetMove()->flags & FL_ONGROUND) {
		GetMove()->m_vecVelocity[2] = 0;
	}
}

void CTFGameMovement::PreventBunnyJumping() {

	if (GetPlayer()->IsInCond(82) /*TF_COND_HALLOWEEN_KART*/)
		return;

	const float maxscalesspeed = BUNNYJUMP_MAX_SPEED_FACTOR * GetPlayer()->GetMaxSpeed();
	if (maxscalesspeed <= 0.0f)
		return;

	const float spd = GetMove()->m_vecVelocity.Length();
	if (spd <= maxscalesspeed)
		return;

	const float fraction = maxscalesspeed / spd;

	GetMove()->m_vecVelocity *= fraction;

}

bool CTFGameMovement::CheckJumpButton() {
	return false;
}


void CTFGameMovement::FullWalkMove() {

	if (!InWater())
	{
		if (GetPlayer()->IsInCond(80) /*TF_COND_PARACHUTE_ACTIVE*/ && GetMove()->m_vecVelocity.z < 0)
		{
			static ConVar* tf_parachute_maxspeed_z = ICvar::factory()->FindVar("tf_parachute_maxspeed_z");
			static ConVar* tf_parachute_maxspeed_xy = ICvar::factory()->FindVar("tf_parachute_maxspeed_xy");

			GetMove()->m_vecVelocity[2] = std::max(GetMove()->m_vecVelocity[2], tf_parachute_maxspeed_z->GetFloat());

			const float flDrag = tf_parachute_maxspeed_xy->GetFloat();

			// Instead of clamping, we'll dampen
			const float flSpeedX = abs(GetMove()->m_vecVelocity.x);
			const float flSpeedY = abs(GetMove()->m_vecVelocity.y);
			const float flReductionX = flSpeedX > flDrag ? (flSpeedX - flDrag) / 3.0f - 10.0f : 0;
			const float flReductionY = flSpeedY > flDrag ? (flSpeedY - flDrag) / 3.0f - 10.0f : 0;

			GetMove()->m_vecVelocity.x = std::clamp(GetMove()->m_vecVelocity.x, -flDrag - flReductionX, flDrag + flReductionX);
			GetMove()->m_vecVelocity.y = std::clamp(GetMove()->m_vecVelocity.y, -flDrag - flReductionY, flDrag + flReductionY);
		}

		StartGravity();
	}

	// If we are swimming in the water, see if we are nudging against a place we can jump up out
	//  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0.
	//  Also run the swim code if we're a ghost or have the TF_COND_SWIMMING_NO_EFFECTS condition
	if (InWater() || GetPlayer()->IsInCond(77) /*TF_COND_HALLOWEEN_GHOST_MODE*/ || GetPlayer()->IsInCond(107) /*TF_COND_SWIMMING_NO_EFFECTS*/)
	{
		FullWalkMoveUnderwater();
		return;
	}

	// Make sure velocity is valid.
	CheckVelocity();

	if (GetMove()->flags & FL_ONGROUND)
	{
		GetMove()->m_vecVelocity[2] = 0.0;
	    Friction();
		WalkMove();
	}
	else
	{
		AirMove();
	}

	// Set final flags.
	CategorizePosition();

	// Add any remaining gravitational component if we are not in water.
	if (!InWater())
	{
		FinishGravity();
	}

	// If we are on ground, no downward velocity.
	if (GetMove()->flags & FL_ONGROUND)
	{
		GetMove()->m_vecVelocity[2] = 0;
	}

	//CTFGameMovement::CheckFalling()

	// Make sure velocity is valid.
	CheckVelocity();
}

void CTFGameMovement::Friction()
{
	PRINT_FUNCTION

		// Calculate speed
		const float speed = GetMove()->m_vecVelocity.Length();

	// If too slow, return
	if (speed < 0.1f)
	{
		return;
	}

	static ConVar* sv_friction = ICvar::factory()->FindVar("sv_friction");
	static ConVar* sv_stopspeed = ICvar::factory()->FindVar("sv_stopspeed");

	float drop = 0;

	// apply ground friction
	if (GetMove()->flags & FL_ONGROUND) // On an entity that is the ground
	{
		const float friction = sv_friction->GetFloat() * GetMove()->m_surfaceFriction;

		// Bleed off some speed, but if we have less than the bleed
		//  threshold, bleed the threshold amount.
		const float control = speed < sv_stopspeed->GetFloat() ? sv_stopspeed->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * GlobalVars()->TickInterval;
	}

	// scale the velocity
	float newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;

		// Adjust velocity according to proportion.
		GetMove()->m_vecVelocity = GetMove()->m_vecVelocity * newspeed;
	}
}

float CTFGameMovement::GetAirSpeedCap()
{

	static ConVar* tf_max_charge_speed{ ICvar::factory()->FindVar("tf_max_charge_speed") };
	static ConVar* tf_parachute_aircontrol{ ICvar::factory()->FindVar("tf_parachute_aircontrol")};
	static ConVar* tf_halloween_kart_dash_speed{ ICvar::factory()->FindVar("tf_halloween_kart_dash_speed") };
	static ConVar* tf_halloween_kart_aircontrol{ ICvar::factory()->FindVar("tf_halloween_kart_aircontrol") };

	if (GetPlayer()->IsInCond( 17) /*TF_COND_SHIELD_CHARGE*/)
	{
		return tf_max_charge_speed->GetFloat();
	}

	float flCap = 30.f;

	if (GetPlayer()->IsInCond( 122) /*TF_COND_PARACHUTE_DEPLOYED*/)
	{
		flCap *= tf_parachute_aircontrol->GetFloat();
	}

	if (GetPlayer()->IsInCond(82) /*TF_COND_HALLOWEEN_KART*/)
	{
		if (GetPlayer()->IsInCond(83) /*TF_COND_HALLOWEEN_KART_DASH*/)
		{
			return tf_halloween_kart_dash_speed->GetFloat();
		}

		flCap *= tf_halloween_kart_aircontrol->GetFloat();
	}

	const float flIncreasedAirControl = 1.f;

	return flCap * flIncreasedAirControl;
}

void CTFGameMovement::CheckVelocity()
{
	PRINT_FUNCTION

		const auto DescribeAxis = [](const int axis) -> char*
	{
		static char sz[32];

		switch (axis)
		{
		case 0:
			strncpy_s(sz, "X", sizeof sz);
			break;
		case 1:
			strncpy_s(sz, "Y", sizeof sz);
			break;
		case 2:
		default:
			strncpy_s(sz, "Z", sizeof sz);
			break;
		}

		return sz;
	};

	static ConVar* sv_maxvelocity = ICvar::factory()->FindVar("sv_maxvelocity");

	Vec3 org = GetMove()->m_vecOrigin;

	for (int i = 0; i < 3; i++)
	{
		// See if it's bogus.
		if (IsNaN(GetMove()->m_vecVelocity[i]))
		{
			GetMove()->m_vecVelocity[i] = 0;
		}

		if (IsNaN(org[i]))
		{
			org[i] = 0;
			GetMove()->m_vecOrigin = org;
		}

		// Bound it.
		if (GetMove()->m_vecVelocity[i] > sv_maxvelocity->GetFloat())
		{

			GetMove()->m_vecVelocity[i] = sv_maxvelocity->GetFloat();
		}
		else if (GetMove()->m_vecVelocity[i] < -sv_maxvelocity->GetFloat())
		{
			GetMove()->m_vecVelocity[i] = -sv_maxvelocity->GetFloat();
		}
	}
}

void CTFGameMovement::StartGravity()
{

		const float ent_gravity = GetCurrentGravity();

	// Add gravity so they'll be in the correct position during movement
	// yes, this 0.5 looks wrong, but it's not.  
	GetMove()->m_vecVelocity.z -= ent_gravity * 0.5f * GlobalVars()->TickInterval;
	GetMove()->m_vecVelocity.z += GetPlayer()->GetBaseVelocity().z * GlobalVars()->TickInterval;

	Vec3 temp = GetPlayer()->GetBaseVelocity();
	temp.z = 0;
	GetPlayer()->GetBaseVelocity() = temp;

	CheckVelocity();
}
