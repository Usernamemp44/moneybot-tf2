#pragma once
#include "../IMaterial/IMaterial.h"
#include "../IRenderContext/IRenderContext.h"

struct KeyValues; //forward

class IMaterialSystem
{
public:
	virtual ITexture* CreateRenderTargetFF(const char* name) = 0;
	IMaterial *FindMaterial(char const *pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = 0);
	IMaterial *CreateMaterial(char const *pMaterialName, KeyValues *kv);
	IMaterial *GetMaterial(MaterialHandle_t h);
	IMatRenderContext *GetRenderContext();
	MaterialHandle_t FirstMaterial();
	MaterialHandle_t InvalidMaterial();
	MaterialHandle_t NextMaterial(MaterialHandle_t h);
	static IMaterialSystem * factory();
};