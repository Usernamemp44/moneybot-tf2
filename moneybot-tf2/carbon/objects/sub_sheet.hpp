#ifndef _CARBON_OBJECTS_SUB_SHEET_HPP_
#define _CARBON_OBJECTS_SUB_SHEET_HPP_

#include "base_element.hpp"
#include "page.hpp"

namespace carbon::element {
	class io_sub_sheet : public io_element {
	protected:
		decltype(parent_) selected_tab_{};

	public:
		io_sub_sheet() : io_element("", { 0, 35, 5, 35 }) {
		}

		bool hovered() override {
			return false;
		}

		void draw() override {
			if (children_.empty())
				return;

			auto start = 2;
			const auto width = get_button_width();

			for (auto& it : children_) {
				it->set_visible(false);
				const auto tab_hovered = is_button_hovered(start);

				if (input::button_pressed(VK_LBUTTON) && tab_hovered) {
					set_disabled_callbacks(false);
					selected_tab_ = it;
				}

				draw_tab_button(it, start, tab_hovered || it == selected_tab_);
				start += width;
			}

			if (!selected_tab_)
				selected_tab_ = children_.front();

			selected_tab_->set_visible(true);
		}

		[[nodiscard]] int get_button_width() const {
			// todo: better way to get full width in cases 
			const auto width = get_top_parent()->get_bounds().w - 120;

			if (!children_.empty())
				return static_cast<int>(std::ceilf(static_cast<float>(width) / static_cast<float>(children_.size())));

			return width;
		}

		[[nodiscard]] bool is_button_hovered(const int start) const {
			if (disabled_)
				return false;
			
			const auto pos = get_relative_position();
			return input::is_mouse_over({ pos.x + start - 2, pos.y + 38 - bounds_.y - 3, get_button_width(), 25 });
		}

		void draw_tab_button(decltype(parent_)& button, const int start, const bool hovered) const {
			auto* const item = dynamic_cast<io_page*>(button.get());
			
			const auto item_x = get_relative_x() + start - 2;
			const auto item_y = get_relative_y() + 38 - bounds_.y;
			const auto width = get_button_width();

			util::fade(item->fade_anim_progress, hovered);

			if (item->fade_anim_progress > 0) {
				draw::filled_gradient_rect({ item_x, item_y - 10, width, 34 }, io_color{ 36, 34, 37, 33 }.alpha_less(item->fade_anim_progress), io_color{ 69, 59, 61, 210 }.alpha_less(item->fade_anim_progress), false);
				draw::filled_rect({ item_x, item_y + 25 - 2, width, 2 }, palette::primary_pink.alpha(item->fade_anim_progress));
			}
			
			draw::text({ item_x + width / 2 - draw::text_bounds(0, item->get_name()).x / 2, item_y + 4 }, palette::white, 0, 0, item->get_name());
		}
	};
}

#endif