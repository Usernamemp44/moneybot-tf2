#ifndef _CARBON_OBJECTS_SLIDER_HPP_
#define _CARBON_OBJECTS_SLIDER_HPP_

#include "base_element.hpp"

namespace carbon::element {
	template <typename T>
	class io_slider : public io_element {
		std::string_view slider_text_;
		std::u32string append_text_;

		T* setting_;
		T full_;

		float min_;
		float max_;

		bool dragging_ = false;
		int hover_anim_progress_ = 0;
		int drag_anim_progress_ = 0;

	public:
		io_slider(const std::string_view element_text, const std::string_view element_name, const io_vec4 element_bounds, T* element_setting, T element_min, T element_max, std::u32string element_append_text)
			: io_element(element_name, { element_bounds.x, element_bounds.y, element_bounds.w, 6 }), slider_text_(element_text), append_text_(std::move(element_append_text)), setting_(element_setting), full_(element_max), min_(element_min), max_(element_max) {
		}

		int get_total_height() const override {
			return bounds_.h + (!slider_text_.empty() ? 16 : 6);
		}

		bool hovered() override {
			if (disabled_)
				return false;

			return input::is_mouse_over({
				get_relative_position(),
				slider_text_.empty() ?
				io_vec2{bounds_.w - 9, bounds_.h + 2} :
				io_vec2{bounds_.w + 1, bounds_.h + 20}
			});
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_relative_position();
			const auto offset_y = slider_text_.empty() ? 0 : 18;

			util::fade(hover_anim_progress_, is_hovered, 0.55f);
			util::fade(drag_anim_progress_, dragging_, 0.55f);

			// we only care about whats most recent i guess but this is kinda retarded
			const auto anim_progress = hover_anim_progress_ < drag_anim_progress_ ? drag_anim_progress_ : hover_anim_progress_;
			const auto fill = static_cast<int>(bounds_.w * (static_cast<float>(*setting_) - min_) / (max_ - min_) + 1);

			// background
			draw::filled_rect({ pos.x - 1, pos.y + offset_y, bounds_.w + 2, bounds_.h }, io_color::blend(palette::border, { palette::border - 18 }, static_cast<float>(anim_progress) / 255.0f));

			// slider value
			draw::filled_rect({ pos.x - 1, pos.y + offset_y, fill - 2, bounds_.h }, palette::primary_pink);

			// knob
			draw::filled_rect({ pos.x - 1 + fill - 2, pos.y + offset_y, 3, bounds_.h }, io_color::blend(palette::primary_pink * 0.7f, palette::white, static_cast<float>(anim_progress) / 255.0f)); //{ is_hovered ? palette::white : palette::primary_pink * 0.7f });

			// name
			draw::text({ pos.x - 2, pos.y + 2 }, palette::white, 0, 0, slider_text_);

			// todo: format
			//const std::u32string u32_formatted_value = util::to_utf32(fmt::format(static_cast<T>(0.1f) == static_cast<T>(0) ? "{}" : "{0:.2f}", *m_setting));
			//const auto formatted_text_size = draw::u32_text_bounds(0, u32_formatted_value + append_text_);

			// value
			//draw::u32_text({ pos.x + bounds_.w - formatted_text_size.x, pos.y + 2 }, palette::white, 0, 0, u32_formatted_value + append_text_);
		}

		void input(const bool is_hovered) {
			// process dragging
			const auto mouse_down = input::button_down(VK_LBUTTON);
			
			if (is_hovered && !dragging_ && input::button_pressed(VK_LBUTTON))
				dragging_ = true;
			else if (dragging_ && !mouse_down)
				dragging_ = false;

			// process arrow keys
			if (is_hovered && max_ > 1) {
				if (input::button_down(VK_LEFT))
					*setting_ -= static_cast<T>(1);
				else if (input::button_down(VK_RIGHT))
					*setting_ += static_cast<T>(1);
				else if (input::button_down(VK_DOWN))
					*setting_ -= static_cast<T>(10);
				else if (input::button_down(VK_UP))
					*setting_ += static_cast<T>(10);
			}

			// process sliding
			if (dragging_) {
				const auto pos = get_relative_position();
				const auto progress = std::clamp<float>(!slider_text_.empty() ? static_cast<float>(ctx->io.mouse_position.x - pos.x) / (bounds_.w - 3) : static_cast<float>(ctx->io.mouse_position.x - pos.x - 4) / (bounds_.w - 10), 0.0f, 1.0f);
				*setting_ = progress == 1.0f ? full_ : static_cast<T>((max_ - min_) * progress + min_);
			}

			*setting_ = static_cast<T>(std::clamp<float>(*setting_, min_, max_));
		}
	};
}

#endif