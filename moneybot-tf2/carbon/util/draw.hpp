#ifndef _CARBON_UTIL_DRAW_HPP_
#define _CARBON_UTIL_DRAW_HPP_

#include <cassert>
#include <string_view>
#include <type_traits>

namespace carbon {
	namespace palette {
		inline io_color body = { 28, 26, 28 };
		inline io_color body_transparent = { 28, 26, 28, 125 };
		inline io_color primary_pink = { 233, 109, 109 };
		inline io_color border = { 61, 59, 61 };
		inline io_color titlebar = { 36, 34, 37 };

		inline io_color cursor1 = { 27, 28, 34 };
		inline io_color cursor2 = { 233, 109, 109 };

		inline io_color darker_gray = { 20, 20, 20 };
		inline io_color darker_gray_transparent = { 20, 20, 20, 125 };
		inline io_color dark_gray = { 61, 61, 61 };
		inline io_color light_gray = { 161, 161, 161 };
		inline io_color gray = { 145, 145, 145 };
		inline io_color gray_alt = { 125, 125, 125 };
		inline io_color white = { 234, 234, 234 };
	}
	
	using io_draw_line = std::add_pointer_t<void(io_vec2 start, io_vec2 end, int width, io_color color)>;
	using io_draw_filled_rect = std::add_pointer_t<void(io_vec4 rect, io_color color)>;
	using io_draw_outlined_rect = std::add_pointer_t<void(io_vec4 rect, io_color color)>;
	using io_draw_filled_gradient_rect = std::add_pointer_t<void(io_vec4 rect, io_color start_color, io_color end_color, bool horizontal)>;
	using io_draw_outlined_gradient_rect = std::add_pointer_t<void(io_vec4 rect, io_color start_color, io_color end_color, bool horizontal)>;
	using io_draw_filled_circle = std::add_pointer_t<void(io_vec2 pos, int radius, io_color color, int resolution, int split, float rotation)>;
	using io_draw_outlined_circle = std::add_pointer_t<void(io_vec2 pos, int radius, io_color color, int resolution, int split, float rotation)>;
	using io_draw_text = std::add_pointer_t<void(io_vec2 pos, io_color color, io_int font_flags, int font, std::string_view text, bool ignore_color, float scale)>;
	using io_draw_u32_text = std::add_pointer_t<void(io_vec2 pos, io_color color, io_int font_flags, int font, std::u32string_view text, bool ignore_color, float scale)>;
	using io_get_text_bounds = std::add_pointer_t<io_vec2(int font, std::string_view text)>;
	using io_get_u32_text_bounds = std::add_pointer_t<io_vec2(int font, std::u32string_view text)>;

	using io_get_scissor_rect = std::add_pointer_t<HRESULT(io_vec4* rect)>;
	using io_set_scissor_rect = std::add_pointer_t<HRESULT(io_vec4* rect)>;

	struct io_render_alias {
		io_draw_line draw_line;
		io_draw_filled_rect draw_filled_rect;
		io_draw_outlined_rect draw_outlined_rect;
		io_draw_filled_gradient_rect draw_filled_gradient_rect;
		io_draw_outlined_gradient_rect draw_outlined_gradient_rect;
		io_draw_filled_circle draw_filled_circle;
		io_draw_outlined_circle draw_outlined_circle;
		io_draw_text draw_text;
		io_draw_u32_text draw_u32_text;
		io_get_text_bounds get_text_bounds;
		io_get_u32_text_bounds get_u32_text_bounds;

		io_get_scissor_rect get_scissor_rect;
		io_set_scissor_rect set_scissor_rect;
	};

	inline io_render_alias render_alias;

	namespace draw {
		inline void line(const io_vec2 start, const io_vec2 end, const int width, const io_color color) {
			assert(carbon::render_alias.draw_line != nullptr);
			render_alias.draw_line(start, end, width, color);
			ctx->io.draw_calls += 1;
		}
		
		inline void filled_rect(const io_vec4 rect, const io_color color) {
			assert(carbon::render_alias.draw_filled_rect != nullptr);
			render_alias.draw_filled_rect(rect, color);
			ctx->io.draw_calls += 1;
		}

		inline void outlined_rect(const io_vec4 rect, const io_color color) {
			assert(carbon::render_alias.draw_outlined_rect != nullptr);
			render_alias.draw_outlined_rect(rect, color);
			ctx->io.draw_calls += 1;
		}

		inline void filled_gradient_rect(const io_vec4 rect, const io_color start_color, const io_color end_color, const bool horizontal = false) {
			assert(carbon::render_alias.draw_filled_gradient_rect != nullptr);
			render_alias.draw_filled_gradient_rect(rect, start_color, end_color, horizontal);
			ctx->io.draw_calls += 1;
		}

		inline void outlined_gradient_rect(const io_vec4 rect, const io_color start_color, const io_color end_color, const bool horizontal = false) {
			assert(carbon::render_alias.draw_outlined_gradient_rect != nullptr);
			render_alias.draw_outlined_gradient_rect(rect, start_color, end_color, horizontal);
			ctx->io.draw_calls += 1;
		}

		inline void filled_circle(const io_vec2 pos, const int radius, const io_color color, const int resolution = 12, const int split = 1, const float rotation = 0.0f) {
			assert(carbon::render_alias.draw_filled_circle != nullptr);
			render_alias.draw_filled_circle(pos, radius, color, resolution, split, rotation);
			ctx->io.draw_calls += 1;
		}

		inline void outlined_circle(const io_vec2 pos, const int radius, const io_color color, const int resolution = 12, const int split = 1, const float rotation = 0.0f) {
			assert(carbon::render_alias.draw_outlined_circle != nullptr);
			render_alias.draw_outlined_circle(pos, radius, color, resolution, split, rotation);
			ctx->io.draw_calls += 1;
		}

		inline void text(const io_vec2 pos, const io_color color, const io_int font_flags, const int font, const std::string_view text, const bool ignore_color = false, const float scale = 1.0f) {
			assert(carbon::render_alias.draw_text != nullptr);
			render_alias.draw_text(pos, color, font_flags, font, text, ignore_color, scale);
			ctx->io.draw_calls += 1;
		}
		
		inline void u32_text(const io_vec2 pos, const io_color color, const io_int font_flags, const int font, const std::u32string_view text, const bool ignore_color = false, const float scale = 1.0f) {
			assert(carbon::render_alias.draw_u32_text != nullptr);
			render_alias.draw_u32_text(pos, color, font_flags, font, text, ignore_color, scale);
			ctx->io.draw_calls += 1;
		}

		inline io_vec2 text_bounds(const int font, const std::string_view text) {
			assert(carbon::render_alias.get_text_bounds != nullptr);
			return render_alias.get_text_bounds(font, text);
		}
		
		inline io_vec2 u32_text_bounds(const int font, const std::u32string_view text) {
			assert(carbon::render_alias.get_u32_text_bounds != nullptr);
			return render_alias.get_u32_text_bounds(font, text);
		}

		inline HRESULT get_scissor_rect(io_vec4* rect) {
			assert(carbon::render_alias.get_scissor_rect != nullptr);
			return render_alias.get_scissor_rect(rect);
		}

		inline HRESULT set_scissor_rect(io_vec4* rect) {
			assert(carbon::render_alias.set_scissor_rect != nullptr);
			return render_alias.set_scissor_rect(rect);
		}
	}
}

#endif