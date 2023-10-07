#include "Colour.h"

Colour GenerateGradient(Colour from, Colour to, float percentage)
{
	Colour result; 
	result.A = 255;
	result.R = from.r + percentage * (to.r - from.r);
	result.G = from.g + percentage * (to.g - from.g);
	result.B = from.b + percentage * (to.b - from.b);
	return result;
}

void to_json(json& j, const Colour& p) {
	j = json{ {"dword", p.dword} };
}

void from_json(const json& j, Colour& p) {
	j.at("dword").get_to(p.dword);
}