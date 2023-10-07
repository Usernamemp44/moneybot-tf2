#include "CHud.h"

template <typename T> static constexpr auto RelativeToAbsolute(uintptr_t address) noexcept
{
	return (T)(address + sizeof(uintptr_t) + *reinterpret_cast<uintptr_t *>(address));
}

CHudTexture* CHud::GetIcon(const char *pBlah) {
	using FN = CHudTexture* (__thiscall *)(void*, const char*);
	static FN oGetIcon = RelativeToAbsolute<FN>(FindPatternInClient("E8 ? ? ? ? 80 7F 3C 00") + 1); // untested

	return oGetIcon(this, pBlah);
}

CHud* gHud = nullptr;
CHud* CHud::factory() {
	if (!gHud) {
		gHud = *RelativeToAbsolute<CHud**>(FindPatternInClient("E8 ? ? ? ? 8B 76 14 85 F6 75 98") + 0x1) + 0xFC + 0x1;
	}

	return gHud; // somebody fix this ): find the class pointer!!
}