#ifndef _CARBON_OBJECTS_TOAST_HPP_
#define _CARBON_OBJECTS_TOAST_HPP_

#include "base_element.hpp"

namespace carbon::element {
	/**
	 * \brief toast notifications
	 */
	class io_toast : public io_element {
		float duration_; // time left to display alert

	public:
		io_toast(const float duration) : io_element("example_toast", {0, 0, 0, 0}), duration_(duration) {
		}
	};
}

#endif