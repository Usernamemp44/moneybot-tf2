#ifndef _CARBON_UTIL_INPUT_HPP_
#define _CARBON_UTIL_INPUT_HPP_

#include <cstdint>
#include <type_traits>

namespace carbon {
	using io_get_mouse_position = std::add_pointer_t<io_vec2()>;
	using io_get_scroll_state = std::add_pointer_t<int()>;
	using io_get_button_down = std::add_pointer_t<bool(uint32_t key)>;
	using io_get_button_pressed = std::add_pointer_t<bool(uint32_t key)>;

	struct io_input_alias {
		io_get_mouse_position get_mouse_position;
		io_get_scroll_state get_scroll_state;
		io_get_button_down get_button_down;
		io_get_button_pressed get_button_pressed;
	};

	inline io_input_alias input_alias;
	namespace input {
		inline io_vec2 get_mouse_position() {
			assert(carbon::input_alias.get_mouse_position != nullptr);
			return input_alias.get_mouse_position();
		}
		
		inline int get_scroll_state() {
			assert(carbon::input_alias.get_scroll_state != nullptr);
			return input_alias.get_scroll_state();
		}

		inline bool button_down(const uint32_t key) {
			assert(carbon::input_alias.get_button_down != nullptr);
			return input_alias.get_button_down(key);
		}
		
		inline bool button_pressed(const uint32_t key) {
			assert(carbon::input_alias.get_button_pressed != nullptr);
			return input_alias.get_button_pressed(key);
		}

		inline bool is_mouse_over(const io_vec4 bounds) {
			return ctx->io.mouse_position.x >= bounds.x && ctx->io.mouse_position.x <= bounds.x + bounds.w &&
				ctx->io.mouse_position.y >= bounds.y && ctx->io.mouse_position.y <= bounds.y + bounds.h;
		}
	}
}

#endif