#ifndef _CARBON_DATATYPES_VECTOR_VECTOR4D_HPP_
#define _CARBON_DATATYPES_VECTOR_VECTOR4D_HPP_

namespace carbon {
	struct io_vec4 : io_vec2 {
		int w, h;

		io_vec4() : io_vec2(0, 0), w(0), h(0) {
		}

		io_vec4(const int _x, const int _y, const int _w, const int _h) : io_vec2(_x, _y), w(_w), h(_h) {
		}

		io_vec4(const io_vec2 a, const io_vec2 b) : io_vec2(a), w(b.x), h(b.y) {
		}

		io_vec4(const io_vec2 a, const int _w, const int _h) : io_vec2(a), w(_w), h(_h) {
		}
	};
}

#endif