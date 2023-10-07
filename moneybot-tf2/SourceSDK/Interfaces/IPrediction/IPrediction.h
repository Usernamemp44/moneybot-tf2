#pragma once
#include "../../Misc/Defines.h"
#include "../IMoveHelper/IMoveHelper.h"

class IPrediction
{
public:
	void Update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command);
	void SetViewAngles(Vec3 &ang);
	void SetLocalViewAngles(Vec3 &vAng);
	void RunCommand(CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper);
	void SetupMove(CBaseEntity *pEntity, CUserCmd *pCmd, IMoveHelper *pMoveHelper, void* pMoveData);
	void FinishMove(CBaseEntity *pEntity, CUserCmd *pCmd, void* pMoveData);
	void SetIdealPitch(CBaseEntity *player, const Vec3 &origin, const Vec3 &angles, const Vec3 &viewheight);
	static IPrediction * factory();
};