#include "d3d9.hpp"

#include <corecrt_math_defines.h>
#include <ctime>

#include "../carbon/carbon.hpp"
#include "c_input.hpp"

#include "../../Renderer.h"


namespace d3d9 {
	IDirect3DDevice9* _device;
	IDirect3DVertexBuffer9* _vertex_buffer;
	IDirect3DStateBlock9* _prev_state_block;
	IDirect3DStateBlock9* _render_state_block;

	clock_t _frame_start;

	// define
	c_font_manager font_manager;
}

void d3d9::initialize(IDirect3DDevice9* device) {
	_device = device;
	_device->AddRef();

	draw_list = std::make_shared<c_draw_list>(max_vertices);

	create_objects();

	// https://www.fileformat.info/info/unicode/font/segoe_ui_emoji/list.htm
	// https://github.com/fzany/Font-Awesome-Cheat-Charp/blob/master/Output.cs

	// setup aliases for carbon
	carbon::render_alias.draw_line = draw_line;
	carbon::render_alias.draw_filled_rect = draw_filled_rect;
	carbon::render_alias.draw_outlined_rect = draw_outlined_rect;
	carbon::render_alias.draw_filled_gradient_rect = draw_filled_gradient_rect;
	carbon::render_alias.draw_outlined_gradient_rect = draw_outlined_gradient_rect;
	carbon::render_alias.draw_filled_circle = draw_filled_circle;
	carbon::render_alias.draw_outlined_circle = draw_outlined_circle;
	carbon::render_alias.draw_text = draw_text;
	carbon::render_alias.draw_u32_text = draw_u32_text;
	carbon::render_alias.get_text_bounds = get_text_bounds;
	carbon::render_alias.get_u32_text_bounds = get_u32_text_bounds;
	carbon::render_alias.get_scissor_rect = get_scissor_rect;
	carbon::render_alias.set_scissor_rect = set_scissor_rect;

	carbon::input_alias.get_mouse_position = input::get_mouse_position;
	carbon::input_alias.get_scroll_state = input::get_scroll_state;
	carbon::input_alias.get_button_down = input::key_down;
	carbon::input_alias.get_button_pressed = input::key_pressed;

	font_manager.push(std::make_unique<c_font>("Segoe UI Emoji", 10, true, _device, 100, false));

}

void d3d9::create_objects() {
	if (!_device)
		return;

	_device->CreateVertexBuffer(max_vertices * sizeof(vertex_t), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &_vertex_buffer, nullptr);

	for (auto i = 0; i < 2; ++i) {
		_device->BeginStateBlock();

		_device->SetRenderState(D3DRS_ZENABLE, FALSE);

		_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		_device->SetRenderState(D3DRS_ALPHAREF, 0x08);
		_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

		_device->SetRenderState(D3DRS_LIGHTING, FALSE);

		_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		_device->SetRenderState(D3DRS_CLIPPING, TRUE);
		_device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
		_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		_device->SetRenderState(D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

		_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		_device->SetTexture(0, nullptr);
		_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));
		_device->SetPixelShader(nullptr);

		if (i != 0)
			_device->EndStateBlock(&_prev_state_block);
		else
			_device->EndStateBlock(&_render_state_block);
	}
}

void d3d9::invalidate_objects() {
	if (_vertex_buffer) {
		_vertex_buffer->Release();
		_vertex_buffer = nullptr;
	}

	if (_prev_state_block) {
		_prev_state_block->Release();
		_prev_state_block = nullptr;
	}

	if (_render_state_block) {
		_render_state_block->Release();
		_render_state_block = nullptr;
	}
}

bool d3d9::run_frame() {
	D3DVIEWPORT9 viewport;
	if (_device->GetViewport(&viewport) < 0)
		return false;

	carbon::ctx->io.display_size = { static_cast<int>(viewport.Width), static_cast<int>(viewport.Height) };
	input::update();
	return true;
}

void d3d9::begin() {
	if (!_device)
		return;

	_frame_start = clock();

	//D3DVIEWPORT9 vp{ 0, 0, static_cast<DWORD>(carbon::ctx->io.display_size.x), static_cast<DWORD>(carbon::ctx->io.display_size.y), 0.0f, 1.0f };
	//_device->SetViewport(&vp);

	_prev_state_block->Capture();
	_render_state_block->Apply();
}

void d3d9::end() {
	const auto delta_ticks = clock() - _frame_start;

	carbon::ctx->io.frame_delta = delta_ticks;
	carbon::ctx->io.frame_rate = 1000.0f / delta_ticks;

	_prev_state_block->Apply();
}

namespace {
	bool is_toplogy_list(const D3DPRIMITIVETYPE topology) {
		return topology == D3DPT_POINTLIST || topology == D3DPT_LINELIST || topology == D3DPT_TRIANGLELIST;
	}

	int topology_order(const D3DPRIMITIVETYPE topology) {
		switch (topology) {
		case D3DPT_POINTLIST:
			return 1;
		case D3DPT_LINELIST:
		case D3DPT_LINESTRIP:
			return 2;
		case D3DPT_TRIANGLELIST:
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			return 3;
		default:
			return 0;
		}
	}
};

void d3d9::draw() {
	const auto num_vertices = std::size(draw_list->vertices);

	if (num_vertices > 0) {
		if (num_vertices > max_vertices) {
			max_vertices = num_vertices;
			invalidate_objects();
			create_objects();
		}

		void* data;

		if (FAILED(_vertex_buffer->Lock(0, 0, &data, D3DLOCK_DISCARD))) throw std::exception("Crucial Direct3D 9 operation failed!");
		{
			std::memcpy(data, std::data(draw_list->vertices), sizeof(vertex_t) * num_vertices);
		}
		_vertex_buffer->Unlock();
	}

	std::size_t pos = 0;
	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));
	_device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	_device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);


	for (const auto& batch : draw_list->batches) {
		const auto order = topology_order(batch.topology);
		if (batch.count && order > 0) {
			auto primitive_count = batch.count;

			if (is_toplogy_list(batch.topology))
				primitive_count /= order;
			else
				primitive_count -= order - 1;

			_device->SetTexture(0, batch.texture);
			_device->DrawPrimitive(batch.topology, pos, primitive_count);
			pos += batch.count;
		}
	}

	draw_list->clear();
}

void d3d9::shutdown() {
	invalidate_objects();

	_device->Release();
	_device = nullptr;
}

// this is ghetto but will be fixed when renderer improved
HRESULT d3d9::get_scissor_rect(carbon::io_vec4* out) {
	RECT rect;
	const auto ret = _device->GetScissorRect(&rect);

	*out = {
		rect.left,
		rect.top,
		rect.right,
		rect.bottom
	};

	return ret;
}

HRESULT d3d9::set_scissor_rect(carbon::io_vec4* rect) {
	RECT new_rect = {
		rect->x,
		rect->y,
		rect->w,
		rect->h
	};

	return _device->SetScissorRect(&new_rect);
}

void d3d9::draw_line(const carbon::io_vec2 start, const carbon::io_vec2 end, const int width, const carbon::io_color color) {
	vertex_t* vertices;
	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		*vertices++ = { static_cast<float>(start.x), static_cast<float>(start.y), 1.0f, color };
		*vertices++ = { static_cast<float>(end.x), static_cast<float>(end.y), 1.0f, color };
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->DrawPrimitive(D3DPT_LINESTRIP, 0, 2);

	//add_vertices(v, D3DPT_LINESTRIP);
}

void d3d9::draw_filled_rect(const carbon::io_vec4 rect, const carbon::io_color color) {
	vertex_t* vertices;
	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y + rect.h), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y + rect.h), 1.0f, color };
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void d3d9::draw_outlined_rect(const carbon::io_vec4 rect, const carbon::io_color color) {
	vertex_t* vertices;
	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y + rect.h), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y + rect.h), 1.0f, color };
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y), 1.0f, color };
	}
	_vertex_buffer->Unlock();


	_device->SetTexture(0, nullptr);
	_device->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
}

void d3d9::draw_filled_rounded_rect(const carbon::io_vec4 rect, int radius, const carbon::io_color color, const int resolution) {
	radius = std::clamp(radius, 0, rect.w / 2);
	radius = std::clamp(radius, 0, rect.h / 2);

	vertex_t* vertices;

	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		for (auto i = 0; i < 4; i++) {
			const auto x = rect.x + (i < 2 ? rect.w - radius : radius);
			const auto y = rect.x + (i % 3 ? rect.h - radius : radius);

			const auto rotation = 90.0f * i;

			for (auto j = 0; j < resolution; j++) {
				const auto angle = (rotation + j * 6.0f) * M_PI / 180.0f;
				vertices[i * resolution + j] = { static_cast<float>(x + radius * std::sin(angle)), static_cast<float>(y - radius * std::cos(angle)), 1.0f, color };
			}
		}

	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
	_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 4 * resolution - 2);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
}

void d3d9::draw_outlined_rounded_rect(const carbon::io_vec4 rect, int radius, const carbon::io_color color, const int resolution) {
	radius = std::clamp(radius, 0, rect.w / 2);
	radius = std::clamp(radius, 0, rect.h / 2);

	vertex_t* vertices;

	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		for (auto i = 0; i < 4; i++) {
			const auto x = rect.x + (i < 2 ? rect.w - radius : radius);
			const auto y = rect.x + (i % 3 ? rect.h - radius : radius);

			const auto rotation = 90.0f * i;

			for (auto j = 0; j < resolution; j++) {
				const auto angle = (rotation + j * 6.0f) * M_PI / 180.0f;
				vertices[i * resolution + j] = { static_cast<float>(x + radius * std::sin(angle)), static_cast<float>(y - radius * std::cos(angle)), 1.0f, color };
			}
		}
		vertices[4 * resolution] = vertices[0];
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
	_device->DrawPrimitive(D3DPT_LINESTRIP, 0, 4 * resolution);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
}

void d3d9::draw_filled_gradient_rect(const carbon::io_vec4 rect, carbon::io_color start, carbon::io_color end, const bool horizontal) {
	vertex_t* vertices;

	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y), 1.0f, start };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y), 1.0f, horizontal ? end : start };
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y + rect.h), 1.0f, horizontal ? start : end };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y + rect.h), 1.0f, end };
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void d3d9::draw_outlined_gradient_rect(const carbon::io_vec4 rect, carbon::io_color start, carbon::io_color end, bool horizontal) {
	vertex_t* vertices;

	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y), 1.0f, start };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y), 1.0f, horizontal ? end : start };
		*vertices++ = { static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y + rect.h), 1.0f, end };
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y + rect.h), 1.0f, horizontal ? start : end };
		*vertices++ = { static_cast<float>(rect.x), static_cast<float>(rect.y), 1.0f, start };
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
}

void d3d9::draw_filled_circle(const carbon::io_vec2 pos, const int radius, const carbon::io_color color, const int resolution, const int split, const float rotation) {
	std::vector<vertex_t> circle(resolution + 2);

	auto pi = M_PI;
	if (split != 1)
		pi = pi / split;

	circle[0] = { static_cast<float>(pos.x), static_cast<float>(pos.y), 0.0f, color };

	for (auto i = 1; i < resolution + 2; i++)
		circle[i] = { static_cast<float>(pos.x - radius * cos(pi * ((i - 1) / (resolution / 2.0f)))), static_cast<float>(pos.y - radius * sin(pi * ((i - 1) / (resolution / 2.0f)))), 0.0f, color };

	const auto angle = rotation * M_PI / 180.0f;
	for (auto i = 0; i < resolution + 2; i++)
		circle[i] = { static_cast<float>(pos.x + cos(angle) * (circle[i].x - pos.x) - sin(angle) * (circle[i].y - pos.y)), static_cast<float>(pos.y + sin(angle) * (circle[i].x - pos.x) + cos(angle) * (circle[i].y - pos.y)), 0.0f, color };

	vertex_t* vertices;

	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, (resolution + 2) * sizeof(vertex_t), reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		memcpy(vertices, &circle[0], (resolution + 2) * sizeof(vertex_t));
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
	_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
}

void d3d9::draw_outlined_circle(const carbon::io_vec2 pos, const int radius, const carbon::io_color color, const int resolution, const int split, const float rotation) {
	std::vector<vertex_t> circle(resolution + 1);

	auto pi = M_PI;
	if (split != 1)
		pi = pi / split;

	for (auto i = 0; i < resolution + 1; i++)
		circle[i] = { static_cast<float>(pos.x - radius * cos(pi * (i / (resolution / 2.0f)))), static_cast<float>(pos.y - radius * sin(pi * (i / (resolution / 2.0f)))), 0.0f, color };

	const auto angle = rotation * M_PI / 180.0f;
	for (auto i = 0; i < resolution + 1; i++)
		circle[i] = { static_cast<float>(pos.x + cos(angle) * (circle[i].x - pos.x) - sin(angle) * (circle[i].y - pos.y)), static_cast<float>(pos.y + sin(angle) * (circle[i].x - pos.x) + cos(angle) * (circle[i].y - pos.y)), 0.0f, color };

	vertex_t* vertices;

	_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(vertex_t));

	_vertex_buffer->Lock(0, (resolution + 1) * sizeof(vertex_t), reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
	{
		memcpy(vertices, &circle[0], (resolution + 1) * sizeof(vertex_t));
	}
	_vertex_buffer->Unlock();

	_device->SetTexture(0, nullptr);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
	_device->DrawPrimitive(D3DPT_LINESTRIP, 0, resolution);
	_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
}

struct textBuffer {
	const carbon::io_vec2 pos;
	const carbon::io_color color;
	const io_int font_flags;
	const int font;
	const std::string_view text;
	const bool ignore_color;
	const float scale;
};

void d3d9::draw_text(const carbon::io_vec2 pos, const carbon::io_color color, const io_int font_flags, const int font, const std::string_view text, const bool ignore_color, const float scale) {
	font_manager.get_fonts().at(font)->draw(text, pos, color, font_flags, ignore_color, scale);
	//DrawTextCache("cock", pos.x, pos.y, 10, 0xFFFFFFFF, false, TEXT_ALIGN_LEFT, TEXT_ALIGN_TOP);
}

void d3d9::draw_u32_text(const carbon::io_vec2 pos, const carbon::io_color color, const io_int font_flags, const int font, const std::u32string_view text, const bool ignore_color, const float scale) {
	font_manager.get_fonts().at(font)->draw(text, pos, color, font_flags, ignore_color, scale);
	//DrawTextCache("cock", pos.x, pos.y, 10, 0xFFFFFFFF, false, TEXT_ALIGN_LEFT, TEXT_ALIGN_TOP);
}

carbon::io_vec2 d3d9::get_text_bounds(const int font, const std::string_view text) {
	return font_manager.get_fonts().at(font)->get_text_size(text, false);
}

carbon::io_vec2 d3d9::get_u32_text_bounds(const int font, const std::u32string_view text) {
	return font_manager.get_fonts().at(font)->get_text_size(text, false);
}