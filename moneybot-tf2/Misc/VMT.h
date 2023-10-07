#pragma once
#include <Windows.h>
class VMTHook
{
public:
	void** m_pOriginalVTable;
	void** m_pNewVTable;
	void*** m_pInstance;

	int m_iNumIndices;

	//void* origFunctions[256];
	//funcs

	VMTHook(void* instance);
	~VMTHook();

	int tellCount();

	void* HookFunction(int iIndex, void* pfnHook);
	void* UnhookFunction(int iIndex);
	void* GetMethod(int iIndex);
	void* GetHookedMethod(int iIndex);
	void SetPadding(int pad);

	int padding;


	template <typename Fn>
	const Fn& GetMethod(unsigned int index) const {
		return *(const Fn*)&m_pOriginalVTable[index];
	}

};
inline void**& GetVTable(void* inst, size_t offset = 0)
{
	return *reinterpret_cast<void***>((size_t)inst + offset);
}

inline const void** GetVTable(const void* inst, size_t offset = 0)
{
	return *reinterpret_cast<const void***>((size_t)inst + offset);
}

template<typename T>
inline T GetVFunc(const void* inst, size_t index, size_t offset = 0)
{
	return reinterpret_cast<T>(GetVTable(inst, offset)[index]);
}