#include "SetupBones.h"
#include <map>
struct vsf {
	bool cached = false;
	matrix3x4 bones[256];
};
std::map<void*, vsf> cached_bones = std::map<void*, vsf>();
using fn = bool(__thiscall *)(void* _this, matrix3x4 * pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);

float curTime = 0;
void* SetupBonesOriginal;
bool __fastcall SetupBonesHook(void* _this, void* edx, matrix3x4 * pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	fn original = (fn)SetupBonesOriginal;
	if (currentTime != curTime)
		cached_bones.clear();
	curTime = currentTime;
	if (!cached_bones[_this].cached) {
		cached_bones[_this] = { 0 };
		bool ret = original(_this, cached_bones[_this].bones, 256, 0x0007FF00 | boneMask, currentTime);
		if (!ret)
			return false;

		cached_bones[_this].cached = true;
	}
	if (pBoneToWorldOut)
		memcpy(pBoneToWorldOut, cached_bones[_this].bones, nMaxBones * sizeof(matrix3x4));
	return true;
}
	