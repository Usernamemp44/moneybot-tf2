#include "CTextureDictionary.h"
#include "../../../Misc/VMT.h"

IMaterial* CTextureDictionary::GetTextureMaterial(int id) {
    typedef IMaterial*(__thiscall *FN)(void*, int);
    return GetVFunc<FN>(this, 8)(this, id);
}

template <typename T> static constexpr auto RelativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + sizeof(uintptr_t) + *reinterpret_cast<uintptr_t *>(address));
}

CTextureDictionary* cacheDict = NULL;
CTextureDictionary* CTextureDictionary::factory() {
    if (cacheDict == nullptr) {
        using FN = CTextureDictionary* (__cdecl *)();
        static FN oGetDict = RelativeToAbsolute<FN>(FindPatternInModule("vguimatsurface.dll", "E8 ? ? ? ? 57 8B C8") + 1); // untested
        cacheDict = oGetDict();
    }

    return cacheDict;
}