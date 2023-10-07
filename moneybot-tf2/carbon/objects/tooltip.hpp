#ifndef _CARBON_OBJECTS_TOOLTIP_HPP_
#define _CARBON_OBJECTS_TOOLTIP_HPP_

#include "base_element.hpp"

namespace carbon::element {
    class io_tooltip : public io_element {
		int hover_anim_progress_ = 0;
    	
    	// todo: just add this to a item and then if the parent is hovered offset this by the bounds and add a nice fade in and out
    	
    public:
		explicit io_tooltip(const std::string_view element_name) : io_element(element_name) {
		}
    	
		void draw() override {
			util::fade(hover_anim_progress_, parent_->hovered(), 2.0f, -765, 200);
			
			if (hover_anim_progress_ <= 0)
				return;

			const auto pos = ctx->io.mouse_position + io_vec2{ 35, 0 };
			const auto text_bounds = draw::text_bounds(0, name_);
			
			draw::outlined_rect({ pos.x, pos.y, text_bounds.x + 8, text_bounds.y + 8 }, palette::border.alpha(hover_anim_progress_));
			draw::filled_rect({ pos.x + 1, pos.y + 1, text_bounds.x + 7, text_bounds.y + 7 }, palette::body.alpha(hover_anim_progress_));
			draw::text({ pos.x + 5, pos.y + 4 }, palette::white.alpha(hover_anim_progress_), 0, 0, name_);
		}
    };
}

#endif