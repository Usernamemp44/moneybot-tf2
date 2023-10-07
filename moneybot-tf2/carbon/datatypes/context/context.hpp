#ifndef _CARBON_DATATYPES_CONTEXT_CONTEXT_HPP_
#define _CARBON_DATATYPES_CONTEXT_CONTEXT_HPP_

#include <memory>

#include "input_output.hpp"

namespace carbon {
	struct io_ctx {
		bool initialized;

		io_input_output io;

		int time; // this is always matched with the start of a frame
		int frame_count;

		io_ctx() : initialized(false), time(0), frame_count(0) { }
	};

	// global context
	inline std::unique_ptr<io_ctx> ctx = nullptr;
}

#endif