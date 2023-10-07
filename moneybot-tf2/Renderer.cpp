#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <stdint.h>
#include <d3d9.h>
//#include <d3dx9.h>
#include <vector>
#include "Renderer.h"
void DrawLine(int startX, int endX, int startY, int endY, uint32_t color, bool dotted) {

	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(startX, startY), ImVec2(endX, endY), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)));
}

void DrawRectangle(float startX, float endX, float startY, float endY, bool filled, uint32_t color, float rounding, bool outline )
{
	//CandaceRenderer.DrawRectangle(round(x), round(x + w), round(y), round(y + h), False, ARGBToColor(alpha, healthColorR, healthColorG, healthColorB), 1, True)
	//(startX: float, endX: float, startY: float, endY: float, filled: bool, color: int, arg6: bool) -> None
	//
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	if (filled) {
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(startX, startY), ImVec2(endX, endY), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), rounding);
	}
	else {
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(startX, startY), ImVec2(endX, endY), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), rounding);
	}
	if (outline)
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(startX - 1, startY - 1), ImVec2(endX + 1, endY + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, a / 255)), rounding);
	}
}

void DrawGradientRectangle(float startX, float endX, float startY, float endY, uint32_t colorStart, uint32_t colorEnd, bool outline) {
	float aS = (colorStart >> 24) & 0xff;
	float rS = (colorStart >> 16) & 0xff;
	float gS = (colorStart >> 8) & 0xff;
	float bS = (colorStart) & 0xff;

	float aE = (colorEnd >> 24) & 0xff;
	float rE = (colorEnd >> 16) & 0xff;
	float gE = (colorEnd >> 8) & 0xff;
	float bE = (colorEnd) & 0xff;


	ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(startX, startY), ImVec2(endX, endY), ImGui::GetColorU32(ImVec4(rS / 255, gS / 255, bS / 255, aS / 255)), ImGui::GetColorU32(ImVec4(rS / 255, gS / 255, bS / 255, aS / 255)), ImGui::GetColorU32(ImVec4(rE / 255, gE / 255, bE / 255, aE / 255)), ImGui::GetColorU32(ImVec4(rE / 255, gE / 255, bE / 255, aE / 255)));

	if (outline)
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(startX - 1, startY - 1), ImVec2(endX + 1, endY + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, aS / 255)));
	}
}
//void DrawTextCache(char * text, int x, int y, int orientation, bool bold, float size, char* font, uint32_t color, bool outlined, int endx, int endy)
void DrawTextCache(const char * text, int x, int y, float size, uint32_t color, bool outlined, uint8_t xalign, uint8_t yalign)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;

	bool oFontShadow = ImGui::custom_UseFontShadow;
	unsigned int oFontShadowColor = ImGui::GetColorU32(ImVec4(0, 0, 0, a / 255));

	ImGui::custom_UseFontShadow = outlined;
	ImGui::custom_FontShadowColor = ImGui::GetColorU32(ImVec4(0, 0, 0, a / 255));
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 siz = io.Fonts->Fonts[0 + size ]->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
	
	if (xalign == TEXT_ALIGN_CENTER) {
		x = x - siz.x / 2;
	}
	else if (xalign == TEXT_ALIGN_RIGHT) {
		x = x - siz.x;
	}

	if (yalign == TEXT_ALIGN_CENTER) {
		y = y - siz.y / 2;
	}
	else if (yalign == TEXT_ALIGN_BOTTOM) {
		y = y - siz.y;
	}

	ImGui::GetBackgroundDrawList()->AddText(io.Fonts->Fonts[0 + size ], size, ImVec2(x, y), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), text);
	
	ImGui::custom_UseFontShadow = oFontShadow;
	ImGui::custom_FontShadowColor = oFontShadowColor;
}

void DrawCircle(int x, int y, uint32_t color, float radius, bool filled)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), 200);
}
struct image_data {
	const char* path;
	ImTextureID fuck;
};
//std::map<const char*, ImTextureID> images = std::map<const char*, ImTextureID>();
std::vector<image_data*> images = std::vector<image_data*>();
/*ImTextureID GetImage(const char* path) {

	for (int i = 0; i < images.size(); i++) {
		if (strcmp(images[i]->path, path) == 0) {
			return images[i]->fuck;
		}
	}
	image_data* image = new image_data();
	image->path = _strdup(path);

	HRESULT hr = D3DXCreateTextureFromFileA(PresentDevice, path, (LPDIRECT3DTEXTURE9*)&image->fuck);
	if (hr != S_OK)
		return NULL;

	images.push_back(image);
	return image->fuck;
}
void DrawImage(const char* path, int startX, int endX, int startY, int endY) {
	ImGui::GetForegroundDrawList()->AddImage(GetImage(path), ImVec2(startX, startY), ImVec2(endX, endY));
}*/