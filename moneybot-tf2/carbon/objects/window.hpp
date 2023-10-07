#ifndef _CARBON_OBJECTS_WINDOW_HPP_
#define _CARBON_OBJECTS_WINDOW_HPP_

#include "base_element.hpp"
#include "sub_sheet.hpp"

namespace carbon::element {
	class io_window : public io_element {
	protected:
		std::u32string window_text_;

		int title_bar_height_ = 38;
		int tool_bar_height_ = 28;

		int window_flags_ = window_flags_none;

		bool dragging_ = false;
		bool window_mouse_held_ = false;
		io_vec2 drag_offset_ = { 0, 0 };
		
	public:
		struct javascript {
			bool active = false;
			char* text = {};
		} javascript;

		enum e_window_flags : int {
			window_flags_none = 0,					// none
			window_flags_hide_titlebar = 1 << 0,	// hide window titlebar
			window_flags_javascript = 1 << 1,		// display javascript page option
			window_flags_dedicated_window = 1 << 2, // for desktops, renders a minimize and close button
			window_flags_secondary = 1 << 3,		// secondary window type, has no body
		};

		io_window(const std::u32string_view element_text, const std::string_view element_name, int window_id, const io_vec4 element_bounds, const int flags)
			: io_element(element_name, element_bounds), window_text_(element_text), window_flags_(flags) {
			window_id_ = window_id;

			// focus new window
			ctx->io.focused_window_id = window_id_;

			if (window_flags_ & window_flags_dedicated_window)
				ctx->io.is_dedicated_window = true;

			if (window_flags_ & window_flags_hide_titlebar)
				title_bar_height_ = 0;

			if (!(window_flags_ & window_flags_javascript))
				tool_bar_height_ = 0;

			if (window_flags_ & window_flags_secondary) {
				title_bar_height_ = 29;
				tool_bar_height_ = 0;
			}
		}

		bool hovered() override {
			if (disabled_)
				return false;

			return input::is_mouse_over({ get_absolute_position(), bounds_.w, title_bar_height_ - 10 });
		}

		void draw() override {
			const auto is_hovered = hovered();
			input(is_hovered);

			const auto pos = get_absolute_position();

			// border
			if (!(window_flags_ & window_flags_secondary))
				draw::filled_rect({ pos.x - 1, pos.y - 1, bounds_.w + 2, bounds_.h + 2 }, palette::primary_pink);
			else
				draw::filled_rect({ pos.x, pos.y - 1, bounds_.w, 1 }, palette::primary_pink);

			// title bar
			if (!(window_flags_ & window_flags_hide_titlebar))
				draw::filled_rect({ pos.x, pos.y, bounds_.w, title_bar_height_ }, palette::titlebar);

			// body
			if (!(window_flags_ & window_flags_secondary))
				draw::filled_rect({ pos.x, pos.y, bounds_.w, bounds_.h }, palette::body);

			// title bar shadow
			if (!(window_flags_ & window_flags_hide_titlebar)) {
				draw::filled_gradient_rect({ pos.x, pos.y + tool_bar_height_, bounds_.w, 7 }, palette::titlebar * 0.72f, palette::titlebar * 1.28f);
				draw::filled_rect({ pos.x, pos.y + 7 + tool_bar_height_, bounds_.w, tool_bar_height_ }, palette::titlebar * 1.28f);
			}

			if (!tool_bar_height_ && !(window_flags_ & window_flags_secondary))
				draw::filled_gradient_rect({ pos.x, pos.y + title_bar_height_ - 11, bounds_.w, 28 }, palette::darker_gray, palette::body, false);

			// draw dot pattern
			// todo: use texture instead
			//if (!(window_flags_ & WINDOW_FLAGS_HIDE_TITLEBAR) /*&& !io->is_dedicated_window*/) {
			/*	for (int i = 1; i <= bounds_.w + 1; i += 3) {
					static int vertical_pos = 1;

					// next line
					if (i >= bounds_.w) {
						i = 1;
						vertical_pos += 3;
					}

					const bool should_draw = vertical_pos <= (!(window_flags_ & WINDOW_FLAGS_SECONDARY) ? title_bar_height_ - 12 : 0);

					if (should_draw)
						draw::filled_rect({ pos.x + i, pos.y + vertical_pos, 1, 1 }, { 47, 43, 50, 100 });
					else {
						vertical_pos = 1;
						break;
					}
				}
			}*/

			// window title
			if (!(window_flags_ & window_flags_hide_titlebar))
				draw::u32_text({ pos.x + 10, pos.y + 8 }, palette::white, 0, 0, window_text_);

			// reset any cursor we have back to default, children elements can override this
			// todo: here
			//io->m_cursor = cursors::io_cursor::type::DEFAULT;

			// dedicated window, draw window controls
			//if (ctx->io.is_dedicated_window)
			//	window_controls(pos);

			// handle javascript button and it's functions
			//if (window_flags_ & window_flags_javascript)
			//	javascript_page(pos);
		}

		void window_controls(const io_vec2 pos) {
			const io_vec2 button_size{ 33, title_bar_height_ - 11 };

			auto is_exit_button_hovered = [this, pos, button_size]() {
				if (disabled_)
					return false;
				
				return input::is_mouse_over({ pos.x + bounds_.w - button_size.x, pos.y, bounds_.w, button_size.y });
			};

			// draw exit button
			draw::filled_rect({ pos.x + bounds_.w - button_size.x, pos.y, button_size.x, button_size.y }, { 61, 59, 61, (is_exit_button_hovered() ? 120 : 45) });

			// this is bad way of drawing x
			const auto x_offset = button_size.x / 2 + 3;
			const auto y_offset = button_size.y / 2 - 3;
			for (auto i = 0; i < 6; ++i) {
				draw::filled_rect({ pos.x + bounds_.w - x_offset + i, pos.y + y_offset + i, 2, 1 }, palette::gray_alt);
				draw::filled_rect({ pos.x + bounds_.w - x_offset + 4 - i, pos.y + y_offset + i, 2, 1 }, palette::gray_alt);
				draw::filled_rect({ pos.x + bounds_.w - x_offset + i, pos.y + y_offset + 4 - i, 2, 1 }, palette::gray_alt);
				draw::filled_rect({ pos.x + bounds_.w - x_offset + 4 - i, pos.y + y_offset + 4 - i, 2, 1 }, palette::gray_alt);
			}
		}

		void javascript_page(const io_vec2 pos) {
			/*const auto mouse_clicked = input::button_pressed(0x1);

			auto is_button_hovered = [this]() {
				if (disabled_)
					return false;

				const auto pos = get_absolute_position();

				return input::is_mouse_over({pos.x + 10, (tool_bar_height_ + title_bar_height_) / 2 + 6, 24, (tool_bar_height_ + title_bar_height_) / 2 + 20 });
			};

			const auto button_hovered = is_button_hovered();

			// this is dumb, why not just have an option to detect if a key is being held down
			static auto mouse_held = false;
			static auto clicked_before_hovering = false;

			if (button_hovered && mouse_clicked)
				clicked_before_hovering = true;

			if (button_hovered && !clicked_before_hovering)
				mouse_held = true;

			if (!mouse_clicked)
				mouse_held = button_hovered;

			auto button_color = palette::gray;
			if (button_hovered) {
				// javascript button
				button_color = palette::primary_pink;

				if (mouse_clicked && mouse_held) {
					javascript.active = !javascript.active;
					mouse_held = false;
				}
			}

			// todo: here
			//if (m_javascript.active) {
			//	draw::u32text({ pos.x + 10, pos.y + (m_tool_bar_height + m_title_bar_height) / 2 + 6, 0, 0 }, button_color, 0, 3, U"\uf33a", false);
			//}
			//else {
			//	draw::u32text({ pos.x + 10, pos.y + (m_tool_bar_height + m_title_bar_height) / 2 + 6, 0, 0 }, button_color, 0, 2, U"\uf3b8", false);
			//}

			// todo: this entire block
			static std::shared_ptr<io_textbox> input;
			if (m_javascript.active) {
				// erase tab background (todo: just draw the tab background within the tab control itself)
				draw::filled_rect({ pos.x, pos.y + m_title_bar_height + 25, 120, bounds_.h - m_title_bar_height - 25 }, palette::body);

				// initialize input
				static bool once = true;
				if (once) {
					input = std::make_shared<io_textbox>("", "js_textbox", io_vec4{ 6, bounds_.h - 30, bounds_.w - 12, 0 }, "", 85, false, m_javascript.text);
					this->add_child(input);

					once = false;
				}

				if (input) input->set_visible(true);
			}
			else {
				if (input) input->set_visible(false);
			}*/
		}

		void draw_children(io_element* element, const bool allow_rect = true) const {
			if (!element->get_visible())
				return;

			// reset the y position
			element->reset();

			// draw all parent element
			element->draw();

			const auto should_reset = false;
			const auto should_draw = true;

			// start drawing children
			if (!element->get_children().empty() && should_draw) {
				// ok nave
				auto* const height_offset = static_cast<int*>(_malloca(sizeof(int) * element->get_children().size()));

				auto cur_offset = 0;
				for (auto i = static_cast<int>(element->get_children().size()) - 1; i >= 0; --i) {
					auto& cur_item = element->get_children().at(i);
					height_offset[i] = cur_offset;
					
					// modify offset between controls
					cur_offset += cur_item->get_visible() ? cur_item->get_total_height() + 7 : 0;

					// todo: these are autistic
					// offset controls if in a subtab
					if (dynamic_cast<io_sub_sheet*>(element))
						cur_offset += element->get_total_height() + 10;
				}

				auto i = 0;
				for (auto& it : element->get_children()) {
					if (height_offset != nullptr) {
						if (javascript.active) {
							// only draw the required js controls
							if (it->get_name() != "js_textbox")
								continue;
						}

						// push control down for automatic y positioning
						element->set_y_offset(height_offset[i]);

						draw_children(it.get(), !should_reset && allow_rect);
						++i;
					}
				}
			}
		}

		void input(const bool is_hovered) {
			handle_focused_window();
			
			if (is_hovered && input::button_pressed(VK_LBUTTON)) {
				drag_offset_.x = ctx->io.mouse_position.x - bounds_.x;
				drag_offset_.y = ctx->io.mouse_position.y - bounds_.y;
				dragging_ = true;
			}
			else if (dragging_ && !input::button_down(VK_LBUTTON))
				dragging_ = false;

			if (dragging_) {
				bounds_.x = ctx->io.mouse_position.x - drag_offset_.x;
				bounds_.y = ctx->io.mouse_position.y - drag_offset_.y;

				// clamp window pos to display size
				//bounds_.x = std::clamp(bounds_.x, 0, ctx->io.display_size.x - bounds_.w);
				//bounds_.y = std::clamp(bounds_.y, 0, ctx->io.display_size.y - bounds_.h);
			}
		}

		// yep
		void handle_focused_window() const {
			if (input::is_mouse_over({ get_absolute_position(), get_size() }) && input::button_pressed(VK_LBUTTON))
				ctx->io.focused_window_id = get_window_id();
		}

		int get_toolbar_height() const {
			return tool_bar_height_;
		}

		int get_titlebar_height() const {
			return title_bar_height_;
		}
	};
}

#endif