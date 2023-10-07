#pragma once

#include "../IMaterial/IMaterial.h"
#include "../../../Misc/PatternFinder.h"

typedef unsigned int CRC32_t;

class CMatSystemTexture
{
public:
	float m_s0, m_t0, m_s1, m_t1;
private:
	CRC32_t				m_crcFile;
	IMaterial			*m_pMaterial;
	ITexture			*m_pTexture;
	ITexture			*m_pOverrideTexture;

	int					m_iWide;
	int					m_iTall;
	int					m_iInputWide;
	int					m_iInputTall;
	int					m_ID;
	int					m_Flags;
	void	*m_pRegen; // CFontTextureRegen??
};


class CTextureDictionary {
public:
//	CMatSystemTexture* GetTexture(int id);
	IMaterial* GetTextureMaterial(int id);
	static CTextureDictionary* factory();
private:
	//CUtlLinkedList< CMatSystemTexture, unsigned short >	m_Textures;
};