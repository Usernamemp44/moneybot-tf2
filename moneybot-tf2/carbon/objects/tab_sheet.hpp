#ifndef _CARBON_OBJECTS_TAB_SHEET_HPP_
#define _CARBON_OBJECTS_TAB_SHEET_HPP_

#include "base_element.hpp"
#include "page.hpp"

namespace carbon::element {
	class io_tab_sheet : public io_element {
	protected:
		decltype(parent_) selected_tab_{};

	public:
		io_tab_sheet() : io_element("", { 120, 0, 0, 0 }) {
		}
		
		bool hovered() override {
			return false;
		}

		void draw() override {
			if (children_.empty())
				return;

			// this should all be made more dynamic
			auto cur_y = 55;
			for (auto& it : children_) {
				it->set_visible(false);
				if (input::button_pressed(VK_LBUTTON) && is_button_hovered(cur_y)) {
					set_disabled_callbacks(false);
					selected_tab_ = it;
				}

				draw_tab_button(it, cur_y, it == selected_tab_);
				cur_y += 85;
			}

			if (!selected_tab_)
				selected_tab_ = children_.front();

			selected_tab_->set_visible(true);
		}

		void draw_tab_button(decltype(parent_)& button, const int start, const bool active) const {
			auto* const item = dynamic_cast<io_page*>(button.get());

			const auto parent_x = parent_->get_bounds().x + 5;
			const auto parent_y = parent_->get_bounds().y + 5;

			util::fade(item->fade_anim_progress, is_button_hovered(start) && !active, 1.25f, 45, 255);

			if (active) {
				draw::filled_rect({ parent_x + 120 - 8, parent_y + start + 3, 3, 93 }, palette::border.alpha(200));
				item->fade_anim_progress = 255;
			}
			
			draw::u32_text({ parent_x + draw::u32_text_bounds(0, item->icon).x / 2 + 13, parent_y + 30 + start }, palette::light_gray.alpha(item->fade_anim_progress), 0, 0, item->icon);
		}

		[[nodiscard]] bool is_button_hovered(const int start) const {
			constexpr auto item_height = 80;

			return input::is_mouse_over({
				parent_->get_bounds().x + 15,
				parent_->get_bounds().y + 15 + start,
				120 - 8,
				item_height
			});
		}
	};
}

#endif