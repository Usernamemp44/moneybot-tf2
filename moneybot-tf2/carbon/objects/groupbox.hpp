#ifndef _CARBON_OBJECTS_GROUPBOX_HPP_
#define _CARBON_OBJECTS_GROUPBOX_HPP_

#include "base_element.hpp"

// todo: scrolling, minimizing, animations, spacing

namespace carbon::element {
	/**
	 * \brief groupbox containers
	 */
	class io_groupbox : public io_element {
		std::string groupbox_text_;

		int max_height_ = 0;
		bool dynamic_ = false;

		bool dragging_ = false;
		io_vec2 drag_offset_ = { 0, 0 };
		
	public:
		io_groupbox(const std::string_view element_text, const std::string_view element_name, const io_vec4 element_bounds)
			: io_element(element_name, element_bounds), groupbox_text_(element_text) {
		}

		bool hovered() override {
			if (disabled_)
				return false;

			// this is only for the grip but we should handle this all differently
			const auto pos = get_relative_position();
			return input::is_mouse_over({
				pos.x + bounds_.w - 6,
				pos.y + bounds_.h - 5,
				6,
				6
			});
		}

		std::shared_ptr<carbon::element::io_element> add_child(std::shared_ptr<io_element> item) override {
			item->parent_ = shared_from_this();
			children_.emplace(children_.begin(), item);
			return item;
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_relative_position();

			calculate_max_size();

			draw::filled_rect({ pos.x, pos.y, bounds_.w, bounds_.h }, palette::border);
			draw::filled_rect({ pos.x + 1, pos.y + 1, bounds_.w - 2, bounds_.h - 2 }, palette::body);

			// title bar
			draw::filled_rect({ pos.x + 1, pos.y + 1, bounds_.w - 2, 25 }, palette::titlebar);
			draw::text({ pos.x + 7, pos.y + 7 }, palette::white, 0, 0, groupbox_text_);

			// resize grip
			// todo: this way of drawing the grip is ghetto
			const io_vec2 grip_pos = { pos.x + bounds_.w - 6, pos.y + bounds_.h - 5 };

			draw::filled_rect({ grip_pos.x + 4, grip_pos.y, 1, 1 }, palette::border);

			for (auto i = 3; i <= 4; i += 2)
				draw::filled_rect({ grip_pos.x + i, grip_pos.y + 1, 1, 1 }, palette::border);

			for (auto i = 2; i <= 4; i += 2)
				draw::filled_rect({ grip_pos.x + i, grip_pos.y + 2, 1, 1 }, palette::border);

			for (auto i = 1; i <= 4; i += 2)
				draw::filled_rect({ grip_pos.x + i, grip_pos.y + 3, 1, 1 }, palette::border);
		}

		void input(const bool is_hovered) {
			const auto mouse_pos = ctx->io.mouse_position;
			const auto mouse_down = input::button_down(VK_LBUTTON);

			if (is_hovered && !dragging_ && input::button_pressed(VK_LBUTTON)) {
				drag_offset_.y = mouse_pos.y - bounds_.h;
				dragging_ = true;
			}
			else if (dragging_ && !mouse_down)
				dragging_ = false;

			if (dragging_)
				bounds_.h = std::clamp(mouse_pos.y - drag_offset_.y, get_min_size(), get_top_parent()->get_bounds().h - bounds_.h);
		}

		void calculate_max_size() const {
			if (!dynamic_)
				return;
		}

		static int get_min_size() {
			return 50;
		}
	};
}

#endif