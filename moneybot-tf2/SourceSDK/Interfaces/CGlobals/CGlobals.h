#pragma once

class CGlobals
{
public:
	float RealTime;
	int FrameCount;
	float AbsoluteFrameTime;
	float CurTime;
	float FrameTime;
	int MaxClients;
	int TickCount;
	float TickInterval;
	float InterpolationAmount;
};
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / GlobalVars()->TickInterval ) )
#define TICKS_TO_TIME( t )		( GlobalVars()->TickInterval *( t ) )