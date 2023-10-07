#ifndef _CARBON_OBJECTS_HOTKEY_HPP_
#define _CARBON_OBJECTS_HOTKEY_HPP_

#include "base_element.hpp"

namespace carbon::element {
	// todo: modes
	/**
	 * \brief keybind picker
	 */
	class io_hotkey : public io_element {
		enum class e_hotkey_method {
			hotkey_none,
			hotkey_hold,
			hotkey_toggle,
			hotkey_always
		};

		// should do some special shit for config creation without needing these to be pointers
		uint32_t* key_;
		e_hotkey_method* method_;
		
		bool* state_;
		
		bool open_ = false;
		
	public:
		io_hotkey(const std::string_view element_name, const io_vec2 element_bounds, uint32_t* toggle_key, e_hotkey_method* toggle_method, bool* state) : io_element(element_name, { element_bounds.x, element_bounds.y, 28, 14 }), key_(toggle_key), method_(toggle_method), state_(state) {
		}

		bool hovered() override {
			if (disabled_)
				return false;
			
			return input::is_mouse_over({
				get_relative_position(),
				get_size()
			});
		}

		int get_total_height() const override {
			return bounds_.h + 12;
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_relative_position();

			draw::outlined_rect({ pos.x - 1, pos.y - 1, bounds_.w + 2, bounds_.h + 2 }, palette::primary_pink);
			draw::filled_rect({ pos, bounds_.w, bounds_.h }, palette::border);

			if (open_)
				draw::filled_rect({ pos.x, pos.y, bounds_.w, bounds_.h * 2 }, palette::border.alpha(120));
		}


		void input(const bool hovered) {
			if (!open_ && hovered && input::button_pressed(VK_RBUTTON) || open_ && input::button_down(VK_ESCAPE)) {
				open_ = !open_;
				set_disabled_callbacks(open_);
			}

			// handle toggle state
			switch (*method_) {
			case e_hotkey_method::hotkey_none:
				break;
			case e_hotkey_method::hotkey_hold:
				*state_ = input::button_down(*key_);
				break;
			case e_hotkey_method::hotkey_toggle:
				if (input::button_pressed(*key_))
					*state_ = !*state_;
				break;
			case e_hotkey_method::hotkey_always:
				*state_ = true;
				break;
			default:
				break;
			}
		}
	};
}

#endif