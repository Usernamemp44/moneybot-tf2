#ifndef _CARBON_EFFECTS_BOID_HPP_
#define _CARBON_EFFECTS_BOID_HPP_

#include "../datatypes/datatypes.hpp"

#include "../util/draw.hpp"
#include "../util/util.hpp"

namespace carbon::effect {
	struct io_boid {
		io_vec2 position;
		io_vec2 velocity;
		io_vec2	acceleration;
	};

	class io_boid_network {
	public:
		void update() {
			
		}
		
		void draw() {
			update();
		}
	};
}

#endif