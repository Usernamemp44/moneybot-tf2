#include "MoveFix.h"

void MovementFix::FixMove(CUserCmd* cmd, Vec3 vecOldAngles, float oForwardMove, float oSideMove) {
	Vec3 vecDesiredAngle = Vec3(oForwardMove, oSideMove, 0).ToAngle();
	float fDesiredSpeed = sqrtf(oForwardMove * oForwardMove + oSideMove * oSideMove);

	// Fix movement direction
	float fDeltaYaw = DEG2RAD(cmd->ViewAngles.y - vecOldAngles.y + vecDesiredAngle.y);

	// Normalize fDeltaYaw
	fDeltaYaw = fmod(fDeltaYaw + 180.f, 360.f) - 180.f;

	// Ensure that the desired speed is retained in the corrected direction
	float fForwardMove = cosf(fDeltaYaw) * fDesiredSpeed;
	float fSideMove = sinf(fDeltaYaw) * fDesiredSpeed;

	// Set the calculated fMove and sMove to the CUserCmd
	cmd->ForwardMove = fForwardMove;
	cmd->SideMove = fSideMove;
}