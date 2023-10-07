#ifndef _CARBON_OBJECTS_LABEL_HPP_
#define _CARBON_OBJECTS_LABEL_HPP_

#include "base_element.hpp"

namespace carbon::element {
    class io_label : public io_element {
        std::string label_text_;

    public:
        io_label(const std::string_view element_text, const std::string_view element_name, const io_vec2 element_bounds)
            : io_element(element_name, { element_bounds.x, element_bounds.y, 14, 14 }), label_text_(element_text) {
        }

		bool hovered() override {
			if (disabled_)
				return false;

			return input::is_mouse_over({
				get_relative_position(),
				draw::text_bounds(0, label_text_)
			});
		}

		void draw() override {
			const auto pos = get_relative_position();
			draw::text({ pos.x, pos.y }, palette::white, 0, 0, label_text_);
		}
    };
}

#endif