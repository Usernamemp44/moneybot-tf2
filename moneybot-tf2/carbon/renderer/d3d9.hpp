#ifndef _UI_RENDERER_D3D9_HPP_
#define _UI_RENDERER_D3D9_HPP_

#include <d3d9.h>

#include "font.hpp"

#include "../carbon/carbon.hpp"

// todo: batch drawing for everything

struct vertex_t {
    float x, y, z, rhw = 1.0f;
    D3DCOLOR color;

    // specifically for fonts
    float tx, ty;

    vertex_t() : x(0.0f), y(0.0f), z(0.0f), color(0), tx(0.0f), ty(0.0f) {
    }

    vertex_t(const float x, const float y, const float z, const carbon::io_color color)
        : x(x),
        y(y),
        z(z),
        color(D3DCOLOR_RGBA(color.r, color.g, color.b, color.a)),
        tx(0.0f),
        ty(0.0f) {}

    vertex_t(const float x, const float y, const float z, const carbon::io_color color, const float tx, const float ty)
        : x(x),
        y(y),
        z(z),
        color(D3DCOLOR_RGBA(color.r, color.g, color.b, color.a)),
        tx(tx),
        ty(ty) {}
};

namespace d3d9 {
    struct batch_t {
        batch_t(const std::size_t count, const D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr) : count(count), topology(topology), texture(texture) {
        }

        std::size_t count;
        D3DPRIMITIVETYPE topology;
        IDirect3DTexture9* texture;
    };

    class c_draw_list {
    public:
        std::vector<vertex_t> vertices;
        std::vector<batch_t> batches;

        c_draw_list() = default;

        c_draw_list(const size_t max_vertices) {
            vertices.reserve(max_vertices);
        }

        void clear() {
            vertices.clear();
            batches.clear();
        }
    };

    inline size_t max_vertices = 1536;
    inline std::shared_ptr<c_draw_list> draw_list = nullptr;

    void initialize(IDirect3DDevice9* device);

    void create_objects();
    void invalidate_objects();

    bool run_frame();

    void begin();
    void end();

    template <std::size_t N>
    void add_vertices(const vertex_t(&vertex_array)[N], D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr) {
        if (std::empty(draw_list->batches) || draw_list->batches.back().topology != topology || draw_list->batches.back().texture != texture)
            draw_list->batches.emplace_back(0, topology, texture);

        draw_list->batches.back().count += N;

        draw_list->vertices.resize(std::size(draw_list->vertices) + N);
        std::memcpy(&draw_list->vertices[std::size(draw_list->vertices) - N], &vertex_array[0], N * sizeof(vertex_t));

        switch (topology) {
        case D3DPT_LINESTRIP:
        case D3DPT_TRIANGLESTRIP:
            draw_list->batches.emplace_back(0, D3DPT_FORCE_DWORD, nullptr);
            break;
        default:
            break;
        }
    }

    void draw();

    void shutdown();

    HRESULT get_scissor_rect(carbon::io_vec4* out);
    HRESULT set_scissor_rect(carbon::io_vec4* rect);

    void draw_line(carbon::io_vec2 start, carbon::io_vec2 end, int width, carbon::io_color color);

    void draw_filled_rect(carbon::io_vec4 rect, carbon::io_color color);
    void draw_outlined_rect(carbon::io_vec4 rect, carbon::io_color color);

    void draw_filled_rounded_rect(carbon::io_vec4 rect, int radius, carbon::io_color color, int resolution = 16);
    void draw_outlined_rounded_rect(carbon::io_vec4 rect, int radius, carbon::io_color color, int resolution = 16);

    void draw_filled_gradient_rect(carbon::io_vec4 rect, carbon::io_color start, carbon::io_color end, bool horizontal = false);
    void draw_outlined_gradient_rect(carbon::io_vec4 rect, carbon::io_color start, carbon::io_color end, bool horizontal = false);

    void draw_filled_circle(carbon::io_vec2 pos, int radius, carbon::io_color color, int resolution = 12, int split = 1, float rotation = 0.0f);
    void draw_outlined_circle(carbon::io_vec2 pos, int radius, carbon::io_color color, int resolution = 12, int split = 1, float rotation = 0.0f);

    extern c_font_manager font_manager;

    void draw_text(carbon::io_vec2 pos, carbon::io_color color, io_int font_flags, int font, std::string_view text, bool ignore_color = false, float scale = 1.0f);
    void draw_u32_text(carbon::io_vec2 pos, carbon::io_color color, io_int font_flags, int font, std::u32string_view text, bool ignore_color = false, float scale = 1.0f);

    carbon::io_vec2 get_text_bounds(int font, std::string_view text);
    carbon::io_vec2 get_u32_text_bounds(int font, std::u32string_view text);

    // todo: special stuff like drawing polygon using convex hull for drawing things like molotov fire area
}

#endif