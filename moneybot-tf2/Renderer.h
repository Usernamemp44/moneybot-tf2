#pragma once
#include <stdint.h>

enum {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_TOP,
	TEXT_ALIGN_BOTTOM
};

void DrawLine(int startX, int endX, int startY, int endY, uint32_t color, bool dotted = false);

void DrawRectangle(float startX, float endX, float startY, float endY, bool filled, uint32_t color, float rounding, bool outline);

void DrawTextCache(const char * text, int x, int y, float size, uint32_t color, bool outlined, uint8_t xalign, uint8_t yalign);

void DrawCircle(int x, int y, uint32_t color, float radius, bool filled);

void DrawGradientRectangle(float startX, float endX, float startY, float endY, uint32_t colorStart, uint32_t colorEnd, bool outline);