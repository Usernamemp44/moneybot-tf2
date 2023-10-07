#ifndef _CARBON_OBJECTS_PANEL_HPP_
#define _CARBON_OBJECTS_PANEL_HPP_

#include "base_element.hpp"
#include "window.hpp"

namespace carbon::element {
	// todo: make this proper
	/**
	 * \brief can be used to just hold everything to render
	 */
	class io_panel {
		std::vector<std::shared_ptr<io_element>> children_;
		
	public:
		void draw() {
			for (auto& child : children_) {
				if (auto* window = dynamic_cast<io_window*>(child.get()))
					window->draw_children(child.get(), true);
				else
					child->draw();
			}
		}
	};
}

#endif