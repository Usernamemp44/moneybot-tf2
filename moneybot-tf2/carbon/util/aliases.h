#pragma once

#include "d3d9_impl.h"
#include "font_impl.h"
#include "input_impl.h"

#include "iota.h"

#include "../imgui/freetype/freetype.h"
inline d3d9_impl d3d9;
inline input_impl input;
inline c_fonts fonts;

#include <fstream>


namespace alias {
    inline void draw_filled_rect(iota::io_vec4 rect, iota::io_color color) {
        d3d_vertex* vertices;

        d3d9.device->SetStreamSource(0, d3d9.vertex_buffer, 0, sizeof(d3d_vertex));

        d3d9.vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
        {
            *vertices++ = { float(rect.x + rect.w), float(rect.y), 1.0f, color };
            *vertices++ = { float(rect.x), float(rect.y + rect.h), 1.0f, color };
            *vertices++ = { float(rect.x + rect.w), float(rect.y + rect.h), 1.0f, color };
            *vertices++ = { float(rect.x), float(rect.y), 1.0f, color };
            *vertices++ = { float(rect.x), float(rect.y + rect.h), 1.0f, color };
            *vertices++ = { float(rect.x + rect.w), float(rect.y), 1.0f, color };
        }
        d3d9.vertex_buffer->Unlock();

        d3d9.device->SetTexture(0, nullptr);
        d3d9.device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
    }

    inline void draw_outline(iota::io_vec4 rect, iota::io_color color) { }

    inline void draw_filled_circle(iota::io_vec4 rect, iota::io_color color, float radius, float rotation, float resolution, int type = 0) {
        std::vector<d3d_vertex> circle(resolution + 2);

        const float angle = rotation * D3DX_PI / 180.0f;

        float pi = D3DX_PI;
        if (type == 1) {
            pi = D3DX_PI / 2;
        }
        else if (type == 2) {
            pi = D3DX_PI / 4;
        }

        circle[0] = { float(rect.x), float(rect.y), 0.0f, color };

        for (int i = 1; i < resolution + 2; i++) {
            circle[i] = { float(rect.x - radius * cos(pi * ((i - 1) / (resolution / 2.0f)))), float(rect.y - radius * sin(pi * ((i - 1) / (resolution / 2.0f)))), 0.0f, color };
        }

        const int _res = static_cast<int>(resolution) + 2;
        for (int i = 0; i < _res; i++) {
            circle[i] = { float(rect.x + cos(angle) * (circle[i].x - rect.x) - sin(angle) * (circle[i].y - rect.y)), float(rect.y + sin(angle) * (circle[i].x - rect.x) + cos(angle) * (circle[i].y - rect.y)), 0.0f, color };
        }

        d3d_vertex* vertices;

        d3d9.device->SetStreamSource(0, d3d9.vertex_buffer, 0, sizeof(d3d_vertex));

        d3d9.vertex_buffer->Lock(0, (resolution + 2) * sizeof(d3d_vertex), reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
        {
            memcpy(vertices, &circle[0], (resolution + 2) * sizeof(d3d_vertex));
        }
        d3d9.vertex_buffer->Unlock();

        d3d9.device->SetTexture(0, nullptr);
        d3d9.device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
        d3d9.device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
        d3d9.device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
    }

    inline void draw_gradient_rect(iota::io_vec4 rect, iota::io_color start, iota::io_color end, int type) {
        d3d_vertex* vertices;

        d3d9.device->SetStreamSource(0, d3d9.vertex_buffer, 0, sizeof(d3d_vertex));

        d3d9.vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), D3DLOCK_DISCARD);
        {
            switch (type) {
            case 0: // vertical
                *vertices++ = { float(rect.x), float(rect.y), 1.0f, start };
                *vertices++ = { float(rect.x + rect.w), float(rect.y), 1.0f, start };
                *vertices++ = { float(rect.x), float(rect.y + rect.h), 1.0f, end };
                *vertices++ = { float(rect.x + rect.w), float(rect.y + rect.h), 1.0f, end };
                break;
            case 1: // horiz
                *vertices++ = { float(rect.x), float(rect.y), 1.0f, start };
                *vertices++ = { float(rect.x + rect.w), float(rect.y), 1.0f, end };
                *vertices++ = { float(rect.x), float(rect.y + rect.h), 1.0f, start };
                *vertices++ = { float(rect.x + rect.w), float(rect.y + rect.h), 1.0f, end };
                break;
            default: break;
            }
        }
        d3d9.vertex_buffer->Unlock();


        d3d9.device->SetTexture(0, nullptr);
        d3d9.device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }

    inline void draw_text(iota::io_vec4 vec4, iota::io_color color, io_int font_flags, const int font, const std::string_view text, bool ignore_color = false, float scale = 1.0f) {
        fonts.get_fonts().at(font)->draw(text, vec4, color, font_flags, ignore_color, scale);
    }

    inline void draw_u32text(iota::io_vec4 vec4, iota::io_color color, io_int font_flags, const int font, const std::u32string_view text, bool ignore_color = false, float scale = 1.0f) {
        fonts.get_fonts().at(font)->draw(text, vec4, color, font_flags, ignore_color, scale);
    }

    inline iota::io_vec4 get_text_bounds(int font, const std::string_view text) {
        const auto size = fonts.get_fonts().at(font)->get_text_size(text, false);
        return { 0, 0, size.x, size.y };
    }

    inline iota::io_vec4 get_u32text_bounds(int font, const std::u32string_view text) {
        const auto size = fonts.get_fonts().at(font)->get_text_size(text, false);
        return { 0, 0, size.x, size.y };
    }

    inline iota::io_vec2 get_cursor_pos() {
        return input.mouse_pos;
    }

    inline bool button_pressed(const int key) {
        return input.is_key_pressed(key);
    }

    inline int get_scroll_state() {
        return input.scroll_wheel_state;
    }
}
