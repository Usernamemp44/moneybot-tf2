#ifndef _CARBON_CARBON_HPP_
#define _CARBON_CARBON_HPP_

// ReSharper disable CppUnusedIncludeDirective

// todo: make everything start at relative position
// aka all bounds being equal and things not doing pos.x - 1 etc

// todo: hash element name shit used for javascript to increase performance
// thinking of moving some stuff around

#include <memory>

#include "datatypes/datatypes.hpp"

#include "effects/effects.hpp"

#include "objects/objects.hpp"

#include "util/draw.hpp"
#include "util/input.hpp"
#include "util/util.hpp"

namespace carbon {
	void initialize();
	void shutdown();

	void new_frame();
	void end_frame();

	void add_window(const std::shared_ptr<element::io_window>& window);
}

#endif