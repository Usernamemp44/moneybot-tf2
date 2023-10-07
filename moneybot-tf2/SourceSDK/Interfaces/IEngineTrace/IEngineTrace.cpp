#include "IEngineTrace.h"
#include "../../../Misc/VMT.h"
#include "../../Misc/GetInterface.h"
#include "../../../Misc/Math.h"
#include "../../Entities/CBasePlayer/CBasePlayer.h"
void IEngineTrace::ClipRayToEntity(const Ray_t &ray, unsigned int fMask, CBaseEntity *pEntity, CGameTrace *pTrace) {
	typedef void(__thiscall *FN)(void*, const Ray_t &, unsigned int, CBaseEntity *, CGameTrace *);
	return GetVFunc<FN>(this, 3)(this, ray, fMask, pEntity, pTrace);
}

void IEngineTrace::TraceRay(const Ray_t &ray, unsigned int nMask, ITraceFilter *pTraceFilter, CGameTrace *pTrace)
{
	typedef void(__thiscall *FN)(void*, const Ray_t &, unsigned int, ITraceFilter *, CGameTrace *);
	return GetVFunc<FN>(this, 4)(this, ray, nMask, pTraceFilter, pTrace);
}

IEngineTrace* cached_trace = NULL;
IEngineTrace * IEngineTrace::factory()
{
	if (!cached_trace) {
		cached_trace = reinterpret_cast<IEngineTrace *>(GetInterface("engine.dll", "EngineTraceClient003"));
	}
	return cached_trace;
}
bool IEngineTrace::IntersectRayWithOBB(const Vector3 &vecRayStart, const Vector3 vecRayDelta,
	const matrix3x4 &matOBBToWorld, const Vector3 &vecOBBMins, const Vector3 &vecOBBMaxs)
{


	// OPTIMIZE: Store this in the box instead of computing it here
	// compute center in local space
	Vector3 vecBoxExtents = (vecOBBMins + vecOBBMaxs) * 0.5;
	Vector3 vecBoxCenter;


	// transform to world space
	Math::VectorTransform(vecBoxExtents, matOBBToWorld, vecBoxCenter);


	// calc extents from local center
	vecBoxExtents = vecOBBMaxs - vecBoxExtents;


	// OPTIMIZE: This is optimized for world space.  If the transform is fast enough, it may make more
	// sense to just xform and call UTIL_ClipToBox() instead.  MEASURE THIS.


	// save the extents of the ray along 
	Vector3 extent, uextent;
	Vector3 segmentCenter = vecRayStart + vecRayDelta - vecBoxCenter;


	extent.x = extent.y = extent.z = 0.0f;


	// check box axes for separation
	for (int j = 0; j < 3; j++)
	{
		extent[j] = vecRayDelta.x * matOBBToWorld[0][j] + vecRayDelta.y * matOBBToWorld[1][j] + vecRayDelta.z * matOBBToWorld[2][j];
		uextent[j] = fabsf(extent[j]);
		float coord = segmentCenter.x * matOBBToWorld[0][j] + segmentCenter.y * matOBBToWorld[1][j] + segmentCenter.z * matOBBToWorld[2][j];
		coord = fabsf(coord);

		if (coord > (vecBoxExtents[j] + uextent[j]))
			return false;
	}


	// now check cross axes for separation
	float tmp, cextent;
	Vector3 cross;
	cross = vecRayDelta.Cross(segmentCenter);
	

	cextent = cross.x * matOBBToWorld[0][0] + cross.y * matOBBToWorld[1][0] + cross.z * matOBBToWorld[2][0];
	cextent = fabsf(cextent);
	tmp = vecBoxExtents[1] * uextent[2] + vecBoxExtents[2] * uextent[1];
	if (cextent > tmp)
		return false;


	cextent = cross.x * matOBBToWorld[0][1] + cross.y * matOBBToWorld[1][1] + cross.z * matOBBToWorld[2][1];
	cextent = fabsf(cextent);
	tmp = vecBoxExtents[0] * uextent[2] + vecBoxExtents[2] * uextent[0];
	if (cextent > tmp)
		return false;


	cextent = cross.x * matOBBToWorld[0][2] + cross.y * matOBBToWorld[1][2] + cross.z * matOBBToWorld[2][2];
	cextent = fabsf(cextent);
	tmp = vecBoxExtents[0] * uextent[1] + vecBoxExtents[1] * uextent[0];
	if (cextent > tmp)
		return false;
	return true;
}
bool CTraceWorldOnly::ShouldHitEntity(CBaseEntity * pEntityHandle, int contentsMask)
{
	return false;
}

TraceType_t CTraceWorldOnly::GetTraceType() const
{
	return TRACE_WORLD_ONLY;
}

bool CTraceFilter::ShouldHitEntity(void * pEntityHandle, int contentsMask)
{
	CBasePlayer* entity = reinterpret_cast<CBasePlayer*>(pEntityHandle);

	switch (entity->GetClientClass()->iClassID) {
		case 55:
		case 64:
		case 117:
		case 225: {
			return false;
		}
	}

	return !(pEntityHandle == pSkip);
}

TraceType_t CTraceFilter::GetTraceType() const
{
	return (TraceType_t)TRACE_EVERYTHING;
}

