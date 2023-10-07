#include "carbon.hpp"

#include <cassert>

#include "datatypes/context/context.hpp"

namespace carbon {
	std::vector<std::shared_ptr<element::io_window>> windows{};
}

void carbon::initialize() {
	ctx = std::make_unique<io_ctx>();
	ctx->initialized = true;
}

void carbon::shutdown() {
	ctx.reset();
}

void carbon::new_frame() {
	assert(ctx != nullptr);
//	assert(panel != nullptr);

	ctx->io.draw_calls = 0;
	ctx->time = ctx->io.frame_start_time;
	ctx->frame_count++;
	
	ctx->io.mouse_position = input::get_mouse_position();
	ctx->io.scroll_state = input::get_scroll_state();

	// this wont be needed when doing panels instead
	windows.clear();
}

// todo: maybe add a panel object to the context to add all windows/notifications to
void carbon::end_frame() {
	// todo: draw cursor if carbon is not in a dedicated window
	std::sort(windows.begin(), windows.end(), [&](const std::shared_ptr<element::io_window>& lhs, const std::shared_ptr<element::io_window>& rhs) {
		return lhs->is_on_top() < rhs->is_on_top();
	});

	for (auto& it : windows)
		it->draw_children(it.get(), true);

	// draw cursor
	/*const auto x = ctx->io.mouse_position.x;
	const auto y = ctx->io.mouse_position.y;

	draw::filled_rect({ x, y, 1, 12 }, palette::cursor1);
	draw::filled_rect({ x + 1, y, 1, 11 }, palette::cursor1);
	draw::filled_rect({ x + 1, y + 11, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 2, y + 1, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 2, y + 10, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 3, y + 2, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 3, y + 9, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 4, y + 3, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 5, y + 4, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 6, y + 5, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 7, y + 6, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 8, y + 7, 1, 1 }, palette::cursor1);
	draw::filled_rect({ x + 4, y + 8, 5, 1 }, palette::cursor1);

	draw::filled_rect({ x + 1, y + 1, 1, 10 }, palette::cursor2);
	draw::filled_rect({ x + 2, y + 2, 1, 8 }, palette::cursor2);
	draw::filled_rect({ x + 3, y + 3, 1, 6 }, palette::cursor2);
	draw::filled_rect({ x + 4, y + 4, 1, 4 }, palette::cursor2);
	draw::filled_rect({ x + 5, y + 5, 1, 3 }, palette::cursor2);
	draw::filled_rect({ x + 6, y + 6, 1, 2 }, palette::cursor2);
	draw::filled_rect({ x + 7, y + 7, 1, 1 }, palette::cursor2);

	const io_vec2 offset = { x - 4, y - 7 };

	draw::filled_rect({ offset.x - 1, offset.y - 1, 9, 3 }, palette::cursor1);
	draw::filled_rect({ offset.x - 1, offset.y + 12, 9, 3 }, palette::cursor1);
	draw::filled_rect({ offset.x + 2, offset.y + 1, 3, 12 }, palette::cursor1);

	draw::filled_rect({ offset.x, offset.y, 7, 1 }, palette::cursor2);
	draw::filled_rect({ offset.x, offset.y + 13, 7, 1 }, palette::cursor2);
	draw::filled_rect({ offset.x + 3, offset.y + 1, 1, 12 }, palette::cursor2);*/
}

void carbon::add_window(const std::shared_ptr<element::io_window>& window) {
	windows.push_back(window);
}
