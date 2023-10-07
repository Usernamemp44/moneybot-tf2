#include "Console.h"
#include "../../../Renderer.h"
#include <vector>
#include <string>
#include <stddef.h>
#include <stdarg.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "../../../imgui/imgui.h"
#include "../../../Misc/Colour.h"
struct console_txt {
	std::string* str;
	float lifetime;
	uint64_t timestamp;
	Colour colour;
};
std::vector<console_txt*> ConsoleLog = std::vector<console_txt*>();

void Console::printf(const char * format, float lifetime, Colour colour ...)
{
	va_list ap;
	va_start(ap, format);
	char buffer[16384];
	vsprintf(buffer, format, ap);
	puts(buffer, lifetime, colour);
	va_end(ap);
}

void Console::puts(const char * str, float lifetime, Colour colour )
{
	console_txt* txt = new console_txt();
	txt->str = new std::string(str);
	txt->lifetime = lifetime;
	txt->timestamp = time(NULL); // should move to higher precision
	txt->colour = colour;
	ConsoleLog.push_back(txt);
}

void Console::Render()
{
	int cursorX = 100;
	int cursorY = 100;
	const float textSize = 24;

	for (unsigned int i = 0; i < ConsoleLog.size(); i++){
		console_txt* x = ConsoleLog[i];
		if (time(NULL) > (uint64_t)(x->timestamp + (uint64_t)(x->lifetime)) || ConsoleLog.size() >= 6) {
			x->colour.A--;

			if (x->colour.a <= 0) {
				free(x->str);
				free(x);
				ConsoleLog.erase(ConsoleLog.begin() + i);
				continue;
			}
		}
		; 
		std::ostringstream os;
		Colour cls = Colour(ColourWhite.dword);
		cls.a = x->colour.a;
		os << std::put_time(std::localtime((const time_t*)&x->timestamp), "[%H:%M:%S] ");
		DrawTextCache(os.str().c_str(), cursorX, cursorY, textSize, cls.dword, true, TEXT_ALIGN_LEFT, TEXT_ALIGN_TOP);	
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 siz = io.Fonts->Fonts[0 + textSize]->CalcTextSizeA(textSize, FLT_MAX, 0.0f, os.str().c_str());
		DrawTextCache(x->str->c_str(), cursorX + siz.x, cursorY, textSize, x->colour.dword, true, TEXT_ALIGN_LEFT, TEXT_ALIGN_TOP);
		cursorY += textSize;
	}

}
