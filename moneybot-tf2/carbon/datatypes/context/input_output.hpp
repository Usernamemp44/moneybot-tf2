#ifndef _CARBON_DATATYPES_CONTEXT_INPUT_OUTPUT_HPP_
#define _CARBON_DATATYPES_CONTEXT_INPUT_OUTPUT_HPP_

#include <Windows.h>

#include "../definitions.hpp"

#include "../vector/vector2d.hpp"

namespace carbon {
	struct io_input_output {
		bool is_dedicated_window; // if set to true, we treat this gui as a windows application
		int focused_window_id;

		io_vec2 display_size;

		io_uint start_time;

		int frame_start_time;
		float frame_delta; // how long it took to render the frame, in milliseconds
		int frame_end_time;

		float frame_rate;

		int draw_calls; // reset on each new frame

		// input
		io_vec2 mouse_position;
		int scroll_state;

		// todo: more cursor shit
		uint8_t cursor;

		io_input_output() : is_dedicated_window(false), focused_window_id(-1), display_size(-1, -1), start_time(GetTickCount()), frame_start_time(-1), frame_delta(-1.0f), frame_end_time(-1), frame_rate(0.0f), draw_calls(0), mouse_position(-1, -1), scroll_state(-1) {
			//m_cursor = cursors::io_cursor::type::DEFAULT;
		}
	};
}

#endif