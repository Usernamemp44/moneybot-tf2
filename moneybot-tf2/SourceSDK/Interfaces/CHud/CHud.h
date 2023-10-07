#pragma once

#include "../../Misc/Defines.h"
#include "../../../Misc/PatternFinder.h"

typedef unsigned long HFont;

typedef struct wrect_s
{
	int	left;
	int right;
	int top;
	int bottom;
} wrect_t;

class CHudTexture {
public:
	int Width() const
	{
		return rc.right - rc.left;
	}

	int Height() const
	{
		return rc.bottom - rc.top;
	}

	char		szShortName[64];
	char		szTextureFile[64];

	bool		bRenderUsingFont;
	bool		bPrecached;
	char		cCharacterInFont;

	HFont hFont;

	// vgui texture Id assigned to this item
	int			textureId;

	// s0, t0, s1, t1
	float		texCoords[4];

	// Original bounds
	wrect_t		rc;
};

class CHud {
public:
	CHudTexture* GetIcon(const char *szIcon);
	static CHud* factory();
};