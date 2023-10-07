#pragma once

class CBasePlayer;
struct CMoveData_t;

class IGameMovement {
public:
	void ProcessMovement(CBasePlayer*, CMoveData_t*); // 1
	void StartTrackPredictionErrors(CBasePlayer*); // 3
	void FinishTrackPredictionErrors(CBasePlayer*); // 4
	static IGameMovement* factory();
	static IGameMovement factory_noptr();
};