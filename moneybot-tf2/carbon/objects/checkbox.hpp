#ifndef _CARBON_OBJECTS_CHECKBOX_HPP_
#define _CARBON_OBJECTS_CHECKBOX_HPP_

#include "base_element.hpp"

namespace carbon::element {
	class io_checkbox : public io_element {
		std::string checkbox_text_;
		
		bool* setting_;
		
		int hover_anim_progress_ = 0;
		int active_anim_progress_ = 0;

	public:
		io_checkbox(const std::string_view element_text, const std::string_view element_name, const io_vec2 element_bounds, bool* element_setting)
			: io_element(element_name, { element_bounds.x, element_bounds.y, 14, 14 }), checkbox_text_(element_text), setting_(element_setting) {
		}

		bool hovered() override {
			if (disabled_)
				return false;

			return input::is_mouse_over({
				get_relative_position(),
				bounds_.w + draw::text_bounds(0, checkbox_text_).x + 5,
				bounds_.h
			});
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_relative_position();
			util::fade(hover_anim_progress_, is_hovered, 0.55f);
			util::fade(active_anim_progress_, *setting_, 1.0f);

			// all this fading and blending is a bit of a mess but its nicer to do then overlapping
			draw::outlined_rect({ pos.x - 1, pos.y - 1, bounds_.w + 1, bounds_.h + 1 }, palette::border);
			draw::filled_rect({ pos.x, pos.y, bounds_.w, bounds_.h },
				*setting_ ?
				io_color::blend(palette::body, palette::primary_pink, static_cast<float>(active_anim_progress_) / 255.0f) :
				active_anim_progress_ > 0 ?
				io_color::blend(io_color::blend(palette::body, palette::border - 18, static_cast<float>(hover_anim_progress_) / 255.0f), palette::primary_pink, static_cast<float>(active_anim_progress_) / 255.0f) :
				io_color::blend(palette::body, palette::border - 18, static_cast<float>(hover_anim_progress_) / 255.0f));
			
			draw::text({ pos.x + bounds_.w + 6, pos.y + 1 }, palette::white, 0, 0, checkbox_text_);
		}

		void input(const bool is_hovered) {
			if (is_hovered && input::button_pressed(VK_LBUTTON)) {
				hover_anim_progress_ = 0;
				*setting_ = !*setting_;
			}
		}
	};
}

#endif