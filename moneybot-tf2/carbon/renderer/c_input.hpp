#ifndef _UTIL_INPUT_INPUT_HPP_
#define _UTIL_INPUT_INPUT_HPP_


#include <cstdint>
#include <Windows.h>

#include "../carbon/datatypes/vector/vector2d.hpp"

namespace input {
	void capture_mouse_move(unsigned long lparam);
	carbon::io_vec2 get_mouse_position();

	int get_scroll_state();
	void clear_scroll();

	bool register_key_press(UINT message, WPARAM wparam);

	bool key_down(uint32_t key);
	bool key_pressed(uint32_t key);

	void update();
}

#endif