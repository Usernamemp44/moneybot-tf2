

#include "font.hpp"

#include <shlobj.h>
#include <stdexcept>

#include <freetype/ftbitmap.h>

#include "d3d9.hpp"

c_font::c_font(const std::string& font_name, FT_F26Dot6 width, bool anti_aliased, IDirect3DDevice9* device, FT_UInt dpi, bool custom) : device_(device), anti_aliased_(anti_aliased) {
	if (FT_Init_FreeType(&library_))
		throw std::exception("Failed to initialize FreeType");

	std::string file_path_name;
	custom ? file_path_name = font_name : file_path_name = get_path(font_name);

	if (FT_New_Face(library_, file_path_name.c_str(), 0, &face_))
		throw std::exception("Failed to create new font face");

	if (FT_Select_Charmap(face_, FT_ENCODING_UNICODE))
		throw std::exception("Failed to select char map");

	if (FT_Set_Char_Size(face_, width * 64, 0, dpi, 0))
		throw std::exception("Failed to set char size");

	if (FT_Stroker_New(library_, &stroker_))
		throw std::exception("Failed to create stroker object");

	FT_Stroker_Set(stroker_, 2 * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

	/*FT_UInt idx;
	FT_ULong charcode = FT_Get_First_Char( m_face, &idx );*/

	// generate character map
	/*while ( idx != 0 ) {
		charcode = FT_Get_Next_Char( m_face, charcode, &idx );
		create_char_texture_map( charcode);
	}*/

	for (char c = 32; c < 127; c++)
		create_char_texture_map(c);

	device_->CreateVertexBuffer(sizeof(vertex_t) * 24, D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertex_buffer_, nullptr);

	for (auto i = 0; i < 2; i++) {
		device_->BeginStateBlock();

		device_->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device_->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		device_->SetRenderState(D3DRS_ALPHAREF, 0x08);
		device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		device_->SetRenderState(D3DRS_STENCILENABLE, false);
		device_->SetRenderState(D3DRS_CLIPPING, true);
		device_->SetRenderState(D3DRS_CLIPPLANEENABLE, false);
		device_->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		device_->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, false);
		device_->SetRenderState(D3DRS_FOGENABLE, false);
		device_->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);


		device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);//D3DTEXF_LINEAR
		device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		device_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device_->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
		device_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		device_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		device_->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);

		device_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
		device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		device_->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		device_->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		device_->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		device_->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		device_->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
			D3DTTFF_DISABLE);

		device_->SetRenderState(D3DRS_COLORVERTEX, true);
		device_->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

		device_->EndStateBlock(i == 0 ? &state_block_render_ : &old_state_block_);
	}
}
#include "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include\\d3dx9.h"
void c_font::create_char_texture_map(FT_ULong c, bool ignore_color) {
	FT_Int32 flags = FT_LOAD_RENDER;

	flags |= anti_aliased_ ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO;
	flags |= FT_HAS_COLOR(face_) && !ignore_color
		? FT_LOAD_COLOR
		: 0; // likely an emoji nigga

	FT_Error ft_error;
	if ((ft_error = FT_Load_Char(face_, c, flags)))
		throw std::runtime_error("Could not load character glyph " + std::to_string(ft_error));

	FT_Bitmap bmp;
	FT_Bitmap_Init(&bmp);

	// convert a bitmap object with depth of 4bpp making the number of used
	// bytes per line (aka the pitch) a multiple of alignment
	if ((ft_error =
		FT_Bitmap_Convert(library_, &face_->glyph->bitmap, &bmp, 4))) {
		throw std::runtime_error("Failed to convert glyph" +
			std::to_string(ft_error));
	}

	glyph_t glyph;
	glyph.bounds = {
	  static_cast<int>(face_->glyph->bitmap_left), static_cast<int>(face_->glyph->bitmap_top), static_cast<int>(bmp.width),
	  static_cast<int>(bmp.rows)
	};
	glyph.advance = face_->glyph->advance.x;
	glyph.glyph_slot = face_->glyph;
	glyph.has_color = face_->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA;

	IDirect3DTexture9* texture;
	auto width = bmp.width == 0 ? 16 : bmp.width;
	auto rows = bmp.rows == 0 ? 16 : bmp.rows;

	const auto hr = device_->CreateTexture(width, rows, 1, D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP, glyph.has_color ? D3DFMT_A8R8G8B8 : D3DFMT_A8, D3DPOOL_DEFAULT, &texture, NULL);

	//const auto hr = D3DXCreateTexture(device_, bmp.width, bmp.rows, 1, D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP, glyph.has_color ? D3DFMT_A8R8G8B8 : D3DFMT_A8, D3DPOOL_DEFAULT, &texture);

	if (hr != D3D_OK)
		throw std::runtime_error("Failed to create font texture" + std::to_string(hr));

	D3DLOCKED_RECT lock_rect;
	texture->LockRect(0, &lock_rect, nullptr, D3DLOCK_DISCARD);

	if (lock_rect.pBits && bmp.buffer)
	{
		if (glyph.has_color)
			memcpy(lock_rect.pBits, face_->glyph->bitmap.buffer, face_->glyph->bitmap.rows * face_->glyph->bitmap.pitch);
		else {

			unsigned char* pSrcPixels = face_->glyph->bitmap.buffer;
			unsigned char* pDestPixels = (unsigned char*)lock_rect.pBits;
			switch (face_->glyph->bitmap.pixel_mode)
			{
			default:
				assert(0); //Implement me!
				break;
			case FT_PIXEL_MODE_MONO:
				for (uint32_t y = 0; y < rows; y++, pSrcPixels += face_->glyph->bitmap.pitch, pDestPixels += lock_rect.Pitch)
				{
					uint8_t bits = 0;
					const uint8_t *bits_ptr = pSrcPixels;
					for (uint32_t x = 0; x < width; x++, bits <<= 1)
					{
						if ((x & 7) == 0)
							bits = *bits_ptr++;
						pDestPixels[x] = (bits & 0x80) ? 255 : 0;
					}

				}
				break;
			case FT_PIXEL_MODE_GRAY: // Grayscale image, 1 byte per pixel.
			{
				for (UINT i = 0; i < rows; ++i)
				{
					//copy a row
					memcpy(pDestPixels, pSrcPixels, width); //2 bytes per pixel (1byte alpha, 1byte greyscale)

					//advance row pointers
					pSrcPixels += face_->glyph->bitmap.pitch;
					pDestPixels += lock_rect.Pitch;
				}

			}
			break;
			}
		}
	}

	texture->UnlockRect(0);

	glyph.texture = texture;
	glyph_map_[c] = glyph;
	char buf[64];

	sprintf_s(buf, 64, "%d-%d.PNG", c, rand());
	D3DXSaveTextureToFileA(buf, D3DXIFF_PNG, glyph.texture, NULL);

	if ((ft_error = FT_Bitmap_Done(library_, &bmp)))
		throw std::runtime_error("Failed to destroy bitmap object" + std::to_string(ft_error));
}

void c_font::draw(std::string_view text, carbon::io_vec2 position, const carbon::io_color color, const io_int flags, const bool ignore_color, const float scale) {
	if (old_state_block_->Capture() != D3D_OK)
		throw std::runtime_error("Failed to capture old state block");

	if (state_block_render_->Apply() != D3D_OK)
		throw std::runtime_error("Failed to apply new state block");

	const auto bearing_m = glyph_map_['M'].bounds.y;

	carbon::io_vec2 offset{ 0, 0 };
	carbon::io_vec2 text_dimensions{ 0, 0 };

	if (flags)
		text_dimensions = get_text_size(text, false);
	if (flags & font_centered_x)
		offset.x -= text_dimensions.x / 2.0f + 1.0f;
	if (flags & font_centered_y)
		offset.y -= text_dimensions.y / 2.0f;

	for (auto it : text) {
		const auto glyph = glyph_map_[it];

		// we haven't created the texture yet
		if (!glyph.texture) {
			create_char_texture_map(it, ignore_color);
		}

		if (it != ' ') {
			auto pos_x = position.x + glyph.bounds.x * scale - 0.5f;
			auto pos_y = static_cast<float>(position.y) + bearing_m -
				glyph.bounds.y * scale - 0.5f;

			pos_x += offset.x * scale;
			pos_y += offset.y * scale;

			auto w = glyph.bounds.w * scale + 0.5f;
			auto h = glyph.bounds.h * scale + 0.5f;

			device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			device_->SetStreamSource(0, vertex_buffer_, 0, sizeof(vertex_t));

			device_->SetTextureStageState(0, D3DTSS_COLOROP,
				glyph.has_color && !ignore_color
				? D3DTOP_SELECTARG2
				: D3DTOP_SELECTARG1);
			device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			device_->SetTextureStageState(0, D3DTSS_CONSTANT, color.a);
			device_->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			device_->SetTexture(0, glyph.texture);

			vertex_t* vertices;

			const auto add_vertices = [&](const float xpos, const float ypos,
				carbon::io_color col) {


				std::vector<vertex_t> vertex{ {}, {}, {}, {}, {}, {} };

				vertex[0] = { xpos, ypos + h,			1.f,  col,	0.0f, 1.0f };
				vertex[1] = { xpos + w, ypos + h,			1.f, col,	1.0f, 1.0f };
				vertex[2] = { xpos, ypos,			1.f,  col,	0.0f, 0.0f };

				vertex[3] = { xpos + w, ypos + h,			1.f,  col,	1.0f, 1.0f };
				vertex[4] = { xpos + w, ypos,			1.f, col,	1.0f, 0.0f };
				vertex[5] = { xpos, ypos,			1.f,  col,	0.0f, 0.0f };

				device_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, vertex.data(), sizeof(vertex_t));
			};

			// make sure not to render shadow if glyph has color
			if (flags & font_dropshadow && !glyph.has_color) {
				auto shadow_offset = text_dimensions.y * 0.035f;
				shadow_offset = shadow_offset < 1.1f ? 1.0f : shadow_offset;

				static auto d3d_black = carbon::io_color{ 0, 0, 0 };
				//add_vertices(pos_x + shadow_offset, pos_y + shadow_offset, d3d_black);
			}

			add_vertices(pos_x, pos_y, color);
		}

		// bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th
		// pixels by 64 to get amount of pixels))
		position.x += static_cast<float>(glyph.advance >> 6) * scale;
	}

	if (old_state_block_->Apply() != D3D_OK)
		throw std::runtime_error("Failed to apply old state block");
}

void c_font::draw(std::u32string_view text, carbon::io_vec2 position, carbon::io_color color, io_int flags, bool ignore_color, float scale) {
	if (old_state_block_->Capture() != D3D_OK)
		throw std::runtime_error("Failed to capture old state block");

	if (state_block_render_->Apply() != D3D_OK)
		throw std::runtime_error("Failed to apply new state block");

	const auto bearing_m = glyph_map_['M'].bounds.y;

	carbon::io_vec2 offset{ 0, 0 };
	carbon::io_vec2 text_dimensions{ 0, 0 };

	if (flags)
		text_dimensions = get_text_size(text, false);
	if (flags & font_centered_x)
		offset.x -= text_dimensions.x / 2.0f + 1.0f;
	if (flags & font_centered_y)
		offset.y -= text_dimensions.y / 2.0f;

	for (auto it : text) {
		const auto glyph = glyph_map_[it];

		// we haven't created the texture yet
		if (!glyph.texture) {
			create_char_texture_map(it, ignore_color);
		}

		if (it != ' ') {
			auto pos_x = position.x + glyph.bounds.x * scale - 0.5f;
			auto pos_y = static_cast<float>(position.y) + bearing_m -
				glyph.bounds.y * scale - 0.5f;

			pos_x += offset.x * scale;
			pos_y += offset.y * scale;

			auto w = glyph.bounds.w * scale + 0.5f;
			auto h = glyph.bounds.h * scale + 0.5f;

			device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			device_->SetStreamSource(0, vertex_buffer_, 0, sizeof(vertex_t));

			device_->SetTextureStageState(0, D3DTSS_COLOROP,
				glyph.has_color && !ignore_color
				? D3DTOP_SELECTARG2
				: D3DTOP_SELECTARG1);
			device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			device_->SetTextureStageState(0, D3DTSS_CONSTANT, color.a);
			device_->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			device_->SetTexture(0, glyph.texture);

			vertex_t* vertices;

			const auto add_vertices = [&](const float xpos, const float ypos,
				carbon::io_color col) {
				std::vector<vertex_t> vertex{ {}, {}, {}, {}, {}, {} };

				vertex[0] = { xpos, ypos + h,			1.f,  col,	0.0f, 1.0f };
				vertex[1] = { xpos + w, ypos + h,			1.f, col,	1.0f, 1.0f };
				vertex[2] = { xpos, ypos,			1.f,  col,	0.0f, 0.0f };

				vertex[3] = { xpos + w, ypos + h,			1.f,  col,	1.0f, 1.0f };
				vertex[4] = { xpos + w, ypos,			1.f, col,	1.0f, 0.0f };
				vertex[5] = { xpos, ypos,			1.f,  col,	0.0f, 0.0f };

				device_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, vertex.data(), sizeof(vertex_t));
			};

			// make sure not to render shadow if glyph has color
			if (flags & font_dropshadow && !glyph.has_color) {
				auto shadow_offset = text_dimensions.y * 0.035f;
				shadow_offset = shadow_offset < 1.1f ? 1.0f : shadow_offset;

				static auto d3d_black = carbon::io_color{ 0, 0, 0 };
				//add_vertices(pos_x + shadow_offset, pos_y + shadow_offset, d3d_black);
			}

			add_vertices(pos_x, pos_y, color);
		}

		// bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th
		// pixels by 64 to get amount of pixels))
		position.x += static_cast<float>(glyph.advance >> 6) * scale;
	}

	if (old_state_block_->Apply() != D3D_OK)
		throw std::runtime_error("Failed to apply old state block");
}

void c_font::release() {
	if (!library_)
		return;

	FT_Done_FreeType(library_);
	library_ = nullptr;

	if (vertex_buffer_) {
		vertex_buffer_->Release();
		vertex_buffer_ = nullptr;
	}

	for (auto c : glyph_map_) {
		if (c.second.texture) {
			c.second.texture->Release();
			c.second.texture = nullptr;
		}
	}

	glyph_map_.clear();
}

carbon::io_vec2 c_font::get_text_size(std::string_view str, const bool has_drop_shadow) {
	carbon::io_vec2 size = { 0, 0 };

	for (auto it : str) {
		const auto glyph = glyph_map_[it];

		if (!glyph.texture)
			create_char_texture_map(it);

		size.x += static_cast<int>(glyph.advance) >> 6;

		if (size.y < glyph.bounds.h)
			size.y = glyph.bounds.h;
	}

	if (has_drop_shadow)
		size += {static_cast<int>(size.y * 0.035f), static_cast<int>(size.y * 0.035f)};

	return size;
}

carbon::io_vec2 c_font::get_text_size(const std::u32string_view str, const bool has_drop_shadow) {
	carbon::io_vec2 size = { 0, 0 };

	for (auto it : str) {
		const auto glyph = glyph_map_[it];

		if (!glyph.texture)
			create_char_texture_map(it);

		size.x += static_cast<int>(glyph.advance) >> 6;

		if (size.y < glyph.bounds.h)
			size.y = glyph.bounds.h;
	}

	if (has_drop_shadow)
		size += {static_cast<int>(size.y * 0.035f), static_cast<int>(size.y * 0.035f)};

	return size;
}

std::string c_font::get_path(const std::string& font_name) {
	HKEY key;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, R"(Software\Microsoft\Windows NT\CurrentVersion\Fonts)", 0, KEY_READ, &key);

	std::string str_path;

	char str_buffer[MAX_PATH];

	for (auto i = 0;; i++) {
		// set buffer values to null
		memset(str_buffer, 0, MAX_PATH);

		DWORD buf_size = MAX_PATH;

		// export font name as a name of the value
		if (RegEnumValueA(key, i, str_buffer, &buf_size, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
			return {};

		// check if that font name is the one we are looking for
		if (std::string(str_buffer).find(font_name) != std::string::npos) {
			buf_size = MAX_PATH;
			RegQueryValueExA(key, str_buffer, nullptr, nullptr, reinterpret_cast<LPBYTE>(str_buffer), &buf_size);
			str_path = str_buffer;
			break;
		}
	}

	memset(str_buffer, 0, MAX_PATH);
	// get default font folder
	SHGetFolderPathA(nullptr, CSIDL_FONTS, nullptr, 0, str_buffer);

	return std::string(str_buffer) + '\\' + str_path;
}

