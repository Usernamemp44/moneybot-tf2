#ifndef _CARBON_OBJECTS_PAGE_HPP_
#define _CARBON_OBJECTS_PAGE_HPP_

#include "base_element.hpp"

namespace carbon::element {
	class io_page : public io_element {
	public:
		int fade_anim_progress = 0;
		std::u32string icon;

		explicit io_page(const std::string_view page_name)
			: io_element(page_name, {}) {
		}

		io_page(const std::string_view page_name, const std::u32string_view page_icon)
			: io_element(page_name, {}), icon(page_icon) {
		}

		void draw() override {
		}

		bool hovered() override {
			return false;
		}
	};
}

#endif