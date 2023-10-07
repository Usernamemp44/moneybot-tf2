#pragma once
#include "../../../Misc/Colour.h"
class Console {
public:
	static void printf(const char* format, float lifetime = 5.0, Colour colour = ColourWhite, ...);
	static void puts(const char* str, float lifetime = 5.0, Colour color = ColourWhite);
	static void Render();
};