#pragma once
#include <stdint.h>
#include "../json.h"
using json = nlohmann::json;

struct Colour {

	union {
		union {
			union {
				struct {
					union {
						uint8_t b;
						uint8_t B;
					};
					union {
						uint8_t g;
						uint8_t G;
					};
					union {
						uint8_t r;
						uint8_t R;
					};
					union {
						uint8_t a;
						uint8_t A;
					};
				};
			};
			uint32_t dword;
		};
	};

	Colour(uint32_t dw) {
		dword = dw;
	}
	Colour& operator=(Colour clr) {
		this->dword = clr.dword;
		return *this;
	}
	Colour(const Colour &clr) {
		this->dword = clr.dword;
	}
	Colour(uint8_t r, uint8_t g, uint8_t b) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Colour() {
		dword = 0xFF000000;
	}

};

Colour GenerateGradient(Colour from, Colour to, float percentage);

const Colour ColourBlack = Colour(0, 0, 0);
const Colour ColourWhite = Colour(255, 255, 255);
const Colour ColourMaroon = Colour(255, 80, 80);
const Colour ColourRed = Colour(255, 0, 0);
const Colour ColourGreen = Colour(0, 255, 0);
const Colour ColourBlue = Colour(0, 0, 255);
const Colour ColourPurple = Colour(255, 200, 0);
const Colour ColourCyan = Colour(0, 255, 255);

void to_json(json& j, const Colour& p);

void from_json(const json& j, Colour& p);