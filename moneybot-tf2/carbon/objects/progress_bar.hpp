#ifndef _CARBON_OBJECTS_PROGRESS_BAR_HPP_
#define _CARBON_OBJECTS_PROGRESS_BAR_HPP_

#include "base_element.hpp"

namespace carbon::element {
	class io_progress_bar : public io_element {
		float* progress_;

	public:
		io_progress_bar(const io_vec4 element_bounds, float* progress) : io_element("progress_bar", { element_bounds.x, element_bounds.y, element_bounds.w, 4}), progress_(progress) {
		}
		
		void draw() override {
			const auto pos = get_relative_position();

			// im not overlapping so i can easily alpha override everything smoothly
			const auto fill = static_cast<int>(*progress_ * bounds_.w);
			draw::filled_rect({ pos.x + fill, pos.y, bounds_.w - fill, bounds_.h }, palette::border);
			
			if (*progress_ > 0.001f)
				draw::filled_gradient_rect({ pos.x, pos.y, fill, bounds_.h }, palette::primary_pink, io_color::blend(palette::primary_pink - 18, palette::primary_pink + 22, *progress_), true);
		}
	};
}

#endif