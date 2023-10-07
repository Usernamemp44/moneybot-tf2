#ifndef _UI_RENDERER_FONT_HPP_
#define _UI_RENDERER_FONT_HPP_

#include <map>
#include <string>

#include "../carbon/datatypes/datatypes.hpp"

// todo: vcpkg like real man
//#pragma comment (lib, "C:\\Users\\colin\\\Desktop\\moneybot\\tf2\\moneybot-tf2\\freetype.lib")

#include <cassert>
#include <d3d9.h>
#include <ft2build.h>
#include <vector>

#include <freetype/ftstroke.h>

#include FT_FREETYPE_H

// todo: remove dumb shit of doing functions twice
class c_font {
    struct glyph_t {
        IDirect3DTexture9* texture;
        carbon::io_vec4 bounds;
        signed long advance; // glyph->advance.x
        bool has_color;
        FT_GlyphSlot glyph_slot;
    };

    IDirect3DDevice9* device_;
    IDirect3DVertexBuffer9* vertex_buffer_{};
    IDirect3DStateBlock9* state_block_render_{};
    IDirect3DStateBlock9* old_state_block_{};

    FT_Library library_{};
    FT_Face face_{};
    FT_Stroker stroker_{};

    std::map<unsigned int, glyph_t> glyph_map_;
    bool anti_aliased_;

public:
    // todo: bitshift instead
    enum e_font_flags {
        font_outline = 0x10,
        font_dropshadow = 0x100,
        font_centered_x = 0x1000,
        font_centered_y = 0x10000
    };

    explicit c_font(const std::string& font_name, FT_F26Dot6 width, bool anti_aliased, IDirect3DDevice9* device, FT_UInt dpi = 100, bool custom = false);

    void create_char_texture_map(FT_ULong c, bool ignore_color = false);

    void draw(std::string_view text, carbon::io_vec2 position, carbon::io_color color, io_int flags, bool ignore_color = false, float scale = 1.0f);
    void draw(std::u32string_view text, carbon::io_vec2 position, carbon::io_color color, io_int flags, bool ignore_color = false, float scale = 1.0f);

    void release();

    carbon::io_vec2 get_text_size(std::string_view str, bool has_drop_shadow);
    carbon::io_vec2 get_text_size(std::u32string_view str, bool has_drop_shadow);

    static std::string get_path(const std::string& font_name);
};

class c_font_manager {
    std::vector<std::shared_ptr<c_font>> fonts_;

public:
    void push(const std::shared_ptr<c_font>& font) {
        assert(font);
        fonts_.push_back(font);
    }

    [[nodiscard]] auto& get_fonts() const {
        assert(!fonts_.empty());
        return fonts_;
    }

    void release() {
        for (auto& font : fonts_)
            font->release();
    }
};

#endif