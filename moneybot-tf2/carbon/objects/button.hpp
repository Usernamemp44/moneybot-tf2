#ifndef _CARBON_OBJECTS_BUTTON_HPP_
#define _CARBON_OBJECTS_BUTTON_HPP_

#include "base_element.hpp"

namespace carbon::element {
	class io_button : public io_element {
		std::string button_text_;
		
		std::function<void()> function_callback_;

		int hover_anim_progress_ = 0;

	public:
		io_button(const std::string_view element_text, const std::string_view element_name, const io_vec4 element_bounds, std::function<void()> callback)
			: io_element(element_name, element_bounds), function_callback_(std::move(callback)), button_text_(element_text) {
		}

		bool hovered() override {
			if (disabled_)
				return false;

			return input::is_mouse_over({
				get_relative_position(),
				get_size()
			});
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_relative_position();
			util::fade(hover_anim_progress_, is_hovered, 0.55f);

			draw::outlined_rect({ pos.x - 1, pos.y - 1, bounds_.w + 1, bounds_.h + 1 }, palette::primary_pink);
			draw::filled_rect({ pos.x, pos.y, bounds_.w, bounds_.h }, io_color::blend(palette::border, palette::border - 18, static_cast<float>(hover_anim_progress_) / 255.0f));
			draw::text({ pos.x + bounds_.w / 2, pos.y + bounds_.h / 2 - 1 }, palette::white, 0x1000 | 0x10000, 0, button_text_);
		}

		void input(const bool is_hovered) {
			if (is_hovered && input::button_pressed(VK_LBUTTON)) {
				hover_anim_progress_ = 0;
				function_callback_();
			}
		}
	};
}

#endif