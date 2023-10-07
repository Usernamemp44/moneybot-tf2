#pragma once
#include "../../Misc/Defines.h"
class CTraceWorldOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(CBaseEntity* pEntityHandle, int contentsMask);

	TraceType_t GetTraceType() const;
};

class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask) override;

	TraceType_t GetTraceType() const;
	void* pSkip = 0;
	char pad[0x100] = { 0 };
};

class IEngineTrace
{
public:
	void ClipRayToEntity(const Ray_t & ray, unsigned int fMask, CBaseEntity * pEntity, CGameTrace * pTrace);
	void TraceRay(const Ray_t &ray, unsigned int nMask, ITraceFilter *pTraceFilter, CGameTrace *pTrace);
	static IEngineTrace * factory();
	static bool IntersectRayWithOBB(const Vector3 &vecRayStart, const Vector3 vecRayDelta,
		const matrix3x4 &matOBBToWorld, const Vector3 &vecOBBMins, const Vector3 &vecOBBMaxs);
};