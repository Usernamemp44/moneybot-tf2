#ifndef _CARBON_OBJECTS_TEXTBOX_HPP_
#define _CARBON_OBJECTS_TEXTBOX_HPP_

#include "base_element.hpp"

namespace carbon::element {
	class io_textbox : public io_element {
		std::string textbox_text_;
		std::string placeholder_text_;
		int character_limit_;
		bool hidden_;

		// this isn't a ptr wtf
		std::string string_;

		bool focused_ = false;
		int text_length_ = 0;

		std::array<io_char, 256> key_states_ = {};

	public:
		io_textbox(const std::string_view elm_text, const std::string_view elm_name, const io_vec4 bounds, const std::string_view placeholder, const int max_length, const bool password, char* str)
			: io_element(elm_name, { bounds.x, bounds.y, bounds.w, 22 }), textbox_text_(elm_text), placeholder_text_(placeholder), character_limit_(max_length), hidden_(password) {
		}

		int get_total_height() const override {
			return bounds_.h + 15;
		}

		bool hovered() override {
			if (disabled_)
				return false;

			const auto pos = get_relative_position();
			return input::is_mouse_over({ pos.x - 1, pos.y + 13, bounds_.w + 2, bounds_.h + 2 });
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_relative_position();

			draw::filled_rect({ pos.x - 1, pos.y + 12, bounds_.w + 2, bounds_.h + 2 }, palette::primary_pink);
			draw::filled_rect({ pos.x, pos.y + 13, bounds_.w, bounds_.h }, palette::border);

			// todo: positioning for this and empty check
			draw::text({ pos.x, pos.y - 2 }, palette::white, 0, 0, textbox_text_);

			io_vec4 old_scissor;
			draw::get_scissor_rect(&old_scissor);

			io_vec4 new_scissor = { pos.x, pos.y + 13, pos.x + bounds_.w, pos.y + 13 + bounds_.h };
			draw::set_scissor_rect(&new_scissor);

			// draw placeholder
			if (string_.empty() && !focused_) {
				if (!placeholder_text_.empty())
					draw::text({ pos.x + 3, pos.y + 18 }, palette::gray, 0, 0, placeholder_text_);
			}
			else if (!string_.empty() || focused_)
				draw::text({ pos.x + 3, pos.y + 18 }, palette::white, 0, 0, focused_ ? (hidden_ ? std::string(string_.length(), '*') : string_) + "_" : hidden_ ? std::string(string_.length(), '*') : string_);

			draw::set_scissor_rect(&old_scissor);

			//if (is_hovered)
			//    io->m_cursor = cursors::io_cursor::type::IBEAM;
		}

		void input(const bool is_hovered) {
			if (input::button_pressed(VK_LBUTTON))
				focused_ = is_hovered ? !focused_ : false;
			if (!focused_)
				return;

			text_length_ = string_.length();


			for (io_char key_pressed = 0; key_pressed < 254; ++key_pressed) {
				if (input::button_pressed(key_pressed)) {
					if (key_pressed == VK_ESCAPE || key_pressed == VK_RETURN) {
						focused_ = false;
						break;
					}

					if (key_pressed == VK_BACK) {
						if (!string_.empty())
							string_.pop_back();
						continue;
					}

					wchar_t pressed_char;
					if (text_length_ < character_limit_ && ToAscii(key_pressed, MapVirtualKeyA(key_pressed, 2), static_cast<io_char*>(key_states_.data()), reinterpret_cast<io_ushort*>(&pressed_char), 1) == 1)
						string_ += static_cast<char>(pressed_char);
				}
				else
					key_states_.at(key_pressed) = 0;
			}
		}
	};
}

#endif
