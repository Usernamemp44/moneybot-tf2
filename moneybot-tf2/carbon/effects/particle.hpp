#ifndef _CARBON_EFFECTS_PARTICLE_HPP_
#define _CARBON_EFFECTS_PARTICLE_HPP_

#include <vector>

#include "../datatypes/datatypes.hpp"

#include "../util/draw.hpp"
#include "../util/util.hpp"

namespace carbon::effect {
	struct io_particle {
		io_vec2 position;
		io_vec2 velocity;
		
		io_int radius;
	};

	/**
	 * \brief particle network similar to particle.js
	 */
	class io_particle_network {
		std::vector<io_particle> particle_list_;

	public:
		void add(const io_uint count) {
			for (io_uint i = 0; i < count; i++) {
				// set random position within screen size and velocity
				particle_list_.push_back({
					{ util::random<int>(0, ctx->io.display_size.x), util::random<int>(0, ctx->io.display_size.y) },
					{ 0, 0 },
					1
				});
			}
		}

		void remove(const io_uint count) {
			particle_list_.erase(particle_list_.begin(), particle_list_.begin() + count);
		}

		void clear() {
			particle_list_.clear();
		}

		/**
		 * \brief update all particles with new positions and data on each new frame
		 */
		void update() {
			for (auto& particle : particle_list_) {
				// bounce off the walls
				if (particle.position.x + particle.radius > ctx->io.display_size.x || particle.position.x - particle.radius < 0)
					particle.position.x = -particle.position.x;

				if (particle.position.y + particle.radius > ctx->io.display_size.y || particle.position.y - particle.radius < 0)
					particle.position.y = -particle.position.y;

				// todo: update position + velocity based on frame time
				particle.position += particle.velocity;
				
				// cursor push
				
				// move position using velocity

				// offscreen bounce effect or remove when offscreen
			}
		}

		void draw() {
			// might actually make a update option for each
			// particle so we don't need to loop the list twice
			update();

			// draw
			for (auto& particle : particle_list_) {
				// how should we draw connections, iterate all the particles for each particle?
				// or could we do some direct x mesh magic
				draw::line(particle.position, ctx->io.mouse_position, 1, { 255, 255, 255 });
				
				draw::filled_circle(particle.position, particle.radius, { 255, 255, 255 });
			}
		}
	};
}

#endif