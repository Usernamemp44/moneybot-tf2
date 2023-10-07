#ifndef _CARBON_DATATYPES_MISC_COLOR_HPP_
#define _CARBON_DATATYPES_MISC_COLOR_HPP_

#include "../context/context.hpp"

#include <algorithm>
#include <cstdint>

namespace carbon {
	struct io_color {
		union {
			struct {
				io_char r;
				io_char g;
				io_char b;
				io_char a;
			};

			io_uint rgba{};
		};

		io_color() { r = g = b = a = 0; }

		// override alpha
		[[nodiscard]] io_color alpha(const int a) const {
			auto temp = *this;
			temp.a = a;
			return temp;
		}

		[[nodiscard]] io_color alpha_less(const int a) const {
			auto temp = *this;
			if (a < temp.a)
				temp.a = a;
			return temp;
		}

		io_color operator+(const int i) const {
			return io_color(
				std::clamp(static_cast<uint8_t>(r + i), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				std::clamp(static_cast<uint8_t>(g + i), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				std::clamp(static_cast<uint8_t>(b + i), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				a
			);
		}

		io_color operator-(const int i) const {
			return io_color(
				std::clamp(static_cast<uint8_t>(r - i), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				std::clamp(static_cast<uint8_t>(g - i), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				std::clamp(static_cast<uint8_t>(b - i), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				a
			);
		}

		io_color operator*(const float f) const {
			return io_color(
				std::clamp(static_cast<uint8_t>(r * f), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				std::clamp(static_cast<uint8_t>(g * f), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				std::clamp(static_cast<uint8_t>(b * f), static_cast<uint8_t>(0), static_cast<uint8_t>(255)),
				a
			);
		}

		io_color(const int _r, const int _g, const int _b, const int _a = 255)
			: r{ static_cast<io_char>(_r) }, g{ static_cast<io_char>(_g) }, b{ static_cast<io_char>(_b) }, a{ static_cast<io_char>(_a) } {
		}

		static int to_hex(const int r, const int g, const int b, const int a) {
			return ((r & 0xFF) << 24) + ((g & 0xFF) << 16) + ((b & 0xFF) << 8) + (a & 0xFF);
		}

		static int to_hex(const io_color col) {
			return ((col.r & 0xFF) << 24) + ((col.g & 0xFF) << 16) + ((col.b & 0xFF) << 8) + (col.a & 0xFF);
		}

		static io_color blend(const io_color first, const io_color second, const float t) {
			return io_color(
				first.r + static_cast<int>(t * (second.r - first.r)),
				first.g + static_cast<int>(t * (second.g - first.g)),
				first.b + static_cast<int>(t * (second.b - first.b)),
				first.a + static_cast<int>(t * (second.a - first.a)));
		}
	};
}

#endif