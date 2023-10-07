#ifndef _CARBON_OBJECTS_COLOR_PICKER_HPP_
#define _CARBON_OBJECTS_COLOR_PICKER_HPP_

#include "base_element.hpp"

namespace carbon::element {
	class io_color_picker : public io_element {
		io_color* color_;

		bool open_ = false;
		io_vec2 picker_pos_ = { 0, 0 };
		
		bool dragging_ = false;
		io_vec2 drag_offset_ = { 0, 0 };

		int hover_anim_progress_ = 0;

	public:
		io_color_picker(const std::string_view element_name, const io_vec2 element_bounds, io_color* color) : io_element(element_name, { element_bounds.x, element_bounds.y, 16, 8 }), color_(color) {
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

			util::fade(hover_anim_progress_, is_hovered, 1.0f);

			const auto pos = get_relative_position();

			draw::outlined_rect({ pos.x - 1, pos.y - 1, bounds_.w + 1, bounds_.h + 1 }, io_color::blend(palette::border - 8, palette::border + 16, static_cast<float>(hover_anim_progress_) / 255.0f));

			// draw alpha background todo: texture
			if (color_->a < 255) {
				draw::filled_rect({ pos, get_size() }, { 204, 204, 204 });

				auto n = 0;
				for (auto i = pos.y; i < pos.y + bounds_.h; i += 4, n++)
					for (auto j = pos.x + (n & 1) * 4; j < pos.x + bounds_.w; j += 4 * 2)
						draw::filled_rect({ j, i, 4, 4 }, { 128, 128, 128 });
			}

			draw::filled_rect({ pos, bounds_.w, bounds_.h }, *color_);
			
			if (open_) {
				draw::filled_rect({ picker_pos_.x, picker_pos_.y, 128, 128 }, palette::border);
				draw::filled_rect({ picker_pos_.x + 1, picker_pos_.y + 1, 128 - 2, 128 - 2 }, palette::body);

				draw::filled_rect({ picker_pos_.x + 1, picker_pos_.y + 1, 128 - 2, 25 }, palette::titlebar);
				draw::text({ picker_pos_.x + 7, picker_pos_.y + 7 }, palette::white, 0, 0, "color picker");
			}
		}

		void input(const bool hovered) {
			if (!open_ && hovered && input::button_pressed(VK_RBUTTON) || open_ && input::button_down(VK_ESCAPE)) {
				open_ = !open_;
				set_disabled_callbacks(open_);
				picker_pos_ = get_relative_position();
			}

			if (open_) {
				if (input::is_mouse_over({ picker_pos_, 128, 128 }) && input::button_pressed(VK_LBUTTON)) {
					drag_offset_.x = ctx->io.mouse_position.x - picker_pos_.x;
					drag_offset_.y = ctx->io.mouse_position.y - picker_pos_.y;
					dragging_ = true;
				}
				else if (dragging_ && !input::button_down(VK_LBUTTON))
					dragging_ = false;

				if (dragging_) {
					picker_pos_.x = ctx->io.mouse_position.x - drag_offset_.x;
					picker_pos_.y = ctx->io.mouse_position.y - drag_offset_.y;
				}
			}
		}
	};
}

#endif