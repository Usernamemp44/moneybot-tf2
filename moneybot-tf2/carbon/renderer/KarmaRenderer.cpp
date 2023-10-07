#include "KarmaRenderer.h"
#include <cassert>
#include <ft2build.h>
#include <string>
#include <shlobj.h>
#include FT_BITMAP_H
#include FT_FREETYPE_H

uint32_t rc_crc32(uint32_t crc, const char *buf, size_t len)
{
	static uint32_t table[256];
	static int have_table = 0;
	uint32_t rem;
	uint8_t octet;
	int i, j;
	const char *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if (have_table == 0) {
		/* Calculate CRC table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320;
				}
				else
					rem >>= 1;
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for (p = buf; p < q; p++) {
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}

inline size_t GetPrimitiveSize(D3DPRIMITIVETYPE type) {
	switch (type) {
	default:
		return 0;
	case D3DPRIMITIVETYPE::D3DPT_LINELIST:
	case D3DPRIMITIVETYPE::D3DPT_LINESTRIP:
		return 2;
	case D3DPRIMITIVETYPE::D3DPT_POINTLIST:
		return 1;
	case D3DPRIMITIVETYPE::D3DPT_TRIANGLEFAN:
	case D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST:
	case D3DPRIMITIVETYPE::D3DPT_TRIANGLESTRIP:
		return 3;

	}
}

std::string GetFontPath(const std::string font_name) {
	HKEY key;
	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &key);

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

	return (std::string(str_buffer) + '\\' + str_path);
}
#undef CreateFont
void DirectXRenderer::CreateFont(Font_t font) {
	FT_Library ft;
	assert(!FT_Init_FreeType(&ft));
	FT_Face arial;
	assert(!FT_New_Face(ft, font.fontPath, 0, &arial));
	D3DDEVICE_CREATION_PARAMETERS params;
	device->GetCreationParameters(&params);
	static int dpi = GetDpiForWindow(params.hFocusWindow);
	FT_Set_Char_Size(arial, font.size * 64, 0, dpi, 0);
	FT_Select_Charmap(arial, FT_ENCODING_UNICODE);
	uint32_t crc = rc_crc32(0, (const char*)(&font), sizeof(Font_t));

	for (char i = ' '; i < 127; i++) {
		if (font.antiAliased)
			FT_Load_Char(arial, i, FT_LOAD_RENDER);
		else
			FT_Load_Char(arial, i, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO | FT_LOAD_MONOCHROME);

		UINT width = arial->glyph->bitmap.width ? arial->glyph->bitmap.width : 16;
		UINT height = arial->glyph->bitmap.rows ? arial->glyph->bitmap.rows : 16;

		assert(device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_DEFAULT, &fontMap[crc][i].texture, NULL) == D3D_OK);


		D3DLOCKED_RECT lockRect;
		fontMap[crc][i].texture->LockRect(0, &lockRect, nullptr, D3DLOCK_DISCARD);


		unsigned char* pSrcPixels = arial->glyph->bitmap.buffer;
		unsigned char* pDestPixels = (unsigned char*)lockRect.pBits;


		if (pSrcPixels && pDestPixels) {
			switch (arial->glyph->bitmap.pixel_mode)
			{
			default:
				assert(0); //Implement me!
				break;
			case FT_PIXEL_MODE_MONO:
				for (uint32_t y = 0; y < height; y++, pSrcPixels += arial->glyph->bitmap.pitch, pDestPixels += lockRect.Pitch)
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
				for (UINT i = 0; i < height; ++i)
				{
					//copy a row
					memcpy(pDestPixels, pSrcPixels, width); //2 bytes per pixel (1byte alpha, 1byte greyscale)

					//advance row pointers
					pSrcPixels += arial->glyph->bitmap.pitch;
					pDestPixels += lockRect.Pitch;
				}

			}
			break;
			}
		}
		fontMap[crc][i].texture->UnlockRect(0);

		D3DSURFACE_DESC desc;
		fontMap[crc][i].texture->GetLevelDesc(0, &desc);
		fontMap[crc][i].sizex = width;
		fontMap[crc][i].sizey = height;

		fontMap[crc][i].bearingx = arial->glyph->bitmap_left;
		fontMap[crc][i].bearingy = arial->glyph->bitmap_top;

		fontMap[crc][i].advance = arial->glyph->advance.x;
		fontMap[crc][i].exists = true;
	}

	FT_Done_Face(arial);
	FT_Done_FreeType(ft);

}

std::map<char, Character_t> DirectXRenderer::FindFont(Font_t font) {
	strcpy_s(font.fontPath, 260, GetFontPath(font.fontName).c_str());
	uint32_t hash = rc_crc32(0, (const char*)&(font), sizeof(Font_t));


	if (fontMap.count(hash) > 0)
		return fontMap[hash];
	CreateFont(font);
	return fontMap[hash];

}

void DirectXRenderer::Initialize(LPDIRECT3DDEVICE9 device) {
	DeleteObjects();
	CreateObjects(device);
}

IDirect3DStateBlock9* ourStateBlock = 0;
void DirectXRenderer::DrawRenderCommands(LPDIRECT3DDEVICE9 device)
{
	IDirect3DStateBlock9* stateBlock;
	device->CreateStateBlock(D3DSBT_ALL, &stateBlock);
	if (!ourStateBlock) {
		device->BeginStateBlock();
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		device->SetRenderState(D3DRS_ALPHAREF, 0x08);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		device->SetRenderState(D3DRS_STENCILENABLE, false);
		device->SetRenderState(D3DRS_CLIPPING, true);
		device->SetRenderState(D3DRS_CLIPPLANEENABLE, false);
		device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, false);
		device->SetRenderState(D3DRS_FOGENABLE, false);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);

		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);//D3DTEXF_LINEAR
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		device->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);

		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CONSTANT);
		device->EndStateBlock(&ourStateBlock);
	}
	ourStateBlock->Apply();
	uint32_t vertices = 0;
	for (size_t i = 0; i < rendererCommands.size(); i++)
		vertices += rendererCommands[i].vertices.size();
	if (!vertexBuffer || vertices > allocatedVertexBufferSize) {
		if (vertexBuffer) {
			vertexBuffer->Release();
			vertexBuffer = 0;
		}
		assert(device->CreateVertexBuffer(sizeof(RendererVertex) * vertices, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_DIFFUSE | D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertexBuffer, NULL) == D3D_OK);
		allocatedVertexBufferSize = vertices;
	}

	RendererVertex* vertexBufferLock = 0;
	if (vertexBuffer->Lock(0, (UINT)(vertices * sizeof(RendererVertex)), (void **)&vertexBufferLock, D3DLOCK_DISCARD) != D3D_OK) {
		DeleteObjects();
		DrawRenderCommands(device);
		return;
	}
	//assert(vertexBuffer->Lock(0, (UINT)(vertices * sizeof(RendererVertex)), (void **)&vertexBufferLock, D3DLOCK_DISCARD) == D3D_OK);

	for (size_t i = 0; i < rendererCommands.size(); i++)
	{
		memcpy(vertexBufferLock, &rendererCommands[i].vertices[0], sizeof(RendererVertex) * rendererCommands[i].vertices.size());
		vertexBufferLock += rendererCommands[i].vertices.size();
	}
	assert(vertexBuffer->Unlock() == D3D_OK);

	//D3DRS_ALPHABLENDENABLE
	device->SetStreamSource(0, vertexBuffer, 0, sizeof(RendererVertex));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	//device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	uint32_t counter = 0;
	for (size_t i = 0; i < rendererCommands.size(); i++) {
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);


		if (rendererCommands[i].texture) {
			device->SetTextureStageState(0, D3DTSS_CONSTANT, rendererCommands[i].vertices[0].colour); //not supporting anything else

			device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			//device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}

		device->SetTexture(0, rendererCommands[i].texture);

		device->DrawPrimitive(rendererCommands[i].type, counter, rendererCommands[i].primitiveCount);
		counter += rendererCommands[i].vertices.size();

		if (rendererCommands[i].texture) {
			device->SetTextureStageState(0, D3DTSS_CONSTANT, 0xFFFFFFFF); //not supporting anything else
		}

	}

	rendererCommands.clear();
	stateBlock->Apply();
	stateBlock->Release();
}

void DirectXRenderer::PushRenderCommand(RendererCommand command)
{
	rendererCommands.push_back(command);
}

void DirectXRenderer::PushRenderCommand(RendererCommand * command)
{
	rendererCommands.push_back(*command);
}

void DirectXRenderer::PushRenderCommand(std::vector<RendererVertex> vertices, D3DPRIMITIVETYPE type, int primitiveCount, IDirect3DTexture9 * texture)
{
	RendererCommand command;
	command.vertices = vertices;
	command.type = type;
	command.texture = texture;
	command.primitiveCount = primitiveCount;
	rendererCommands.push_back(command);
}

void DirectXRenderer::DrawTexturedQuad(Vector2 min, Vector2 max, D3DCOLOR color, IDirect3DTexture9* texture) {

	std::vector<RendererVertex> vertex{ {}, {}, {}, {}, {}, {} };


	vertex[0] = { min.x, max.y,			1.f, 0.f, color,	0.0f, 1.0f };
	vertex[1] = { max.x, max.y,			1.f, 0.f, color,	1.0f, 1.0f };
	vertex[2] = { min.x, min.y,			1.f, 0.f, color,	0.0f, 0.0f };

	vertex[3] = { max.x, max.y,			1.f, 0.f, color,	1.0f, 1.0f };
	vertex[4] = { max.x, min.y,			1.f, 0.f, color,	1.0f, 0.0f };
	vertex[5] = { min.x, min.y,			1.f, 0.f, color,	0.0f, 0.0f };
	RendererCommand command;
	command.texture = texture;
	command.vertices = vertex;
	command.type = D3DPT_TRIANGLELIST;
	command.primitiveCount = 2;
	command.isScissor = false;
	PushRenderCommand(command);
}

void DirectXRenderer::DrawCharacter(char chr, Font_t font, Vector2 location, D3DCOLOR color) {
	if (!isprint(chr))
		return;
	if (chr == ' ')
		return;
	std::map<char, Character_t> chars = FindFont(font);
	Character glyph = chars[chr];
	float w = float(glyph.sizex) + 0.5f; //https://docs.microsoft.com/en-gb/windows/win32/direct3d9/directly-mapping-texels-to-pixels
	float h = float(glyph.sizey) + 0.5f;
	float xpos = (float(location.x) + float(glyph.bearingx)) - .5f;
	float ypos = (float(location.y) - float((glyph.bearingy))) - .5f;


	DrawTexturedQuad(Vector2(xpos, ypos), Vector2(xpos + w, ypos + h), color, glyph.texture);
}

void DirectXRenderer::DrawString(const char* string, Vector2 location, D3DCOLOR color, int flag, const char* fontName, int size, bool antiAliased) {
	float x = location.x;
	float y = location.y;
	if (flag & FontFlags::CenteredX || flag & FontFlags::CenteredY) {
		Vector2 bounds = GetTextExtents(string, fontName, size, antiAliased);
		if (flag & FontFlags::CenteredX)
			x = float(x - bounds.x / 2);
		if (flag & FontFlags::CenteredY)
			y = float(y - bounds.y / 2);
	}
	location = Vector2(x, y);
	if (flag & FontFlags::Outline) {

		Vector2 shadowPos1(location);
		Vector2 shadowPos2(location);
		Vector2 shadowPos3(location);
		Vector2 shadowPos4(location);

		shadowPos1.x -= 1;
		shadowPos1.y -= 1;
		DrawString(string, shadowPos1, 0x0000000, 0, fontName, size, antiAliased);
		shadowPos2.x -= 1;
		shadowPos2.y += 1;
		DrawString(string, shadowPos2, 0x0000000, 0, fontName, size, antiAliased);
		shadowPos3.x += 1;
		shadowPos3.y -= 1;
		DrawString(string, shadowPos3, 0x0000000, 0, fontName, size, antiAliased);
		shadowPos4.x += 1;
		shadowPos4.y += 1;
		DrawString(string, shadowPos4, 0x0000000, 0, fontName, size, antiAliased);
	}
	if (flag & FontFlags::Dropshadow) {

		Vector2 shadowPos1(location);
		shadowPos1.x += 1;
		shadowPos1.y += 1;
		DrawString(string, shadowPos1, 0x0000000, 0, fontName, size, antiAliased);
	}


	Font_t font;

	font.antiAliased = antiAliased;
	strcpy_s(font.fontName, 64, fontName);
	font.size = size;

	const char* string2 = string;
	while (*string2) {
		char c = *string2;
		string2++;
		std::map<char, Character_t> chars = FindFont(font);
		Character glyph = chars[c];
		DrawCharacter(c, font, Vector2(x, y), color);
		x += (glyph.advance / 64);
	}


}
void DirectXRenderer::CreateObjects(LPDIRECT3DDEVICE9 device) 
{
	this->device = device;
}

void DirectXRenderer::DeleteObjects(void)
{
	ourStateBlock = 0;
	if (vertexBuffer) {
		vertexBuffer->Release();
		vertexBuffer = 0;
		allocatedVertexBufferSize = 0;
	}
	fontMap.clear();
	device = 0;
}

void DirectXRenderer::SetScissorRect(Vector2 start, Vector2 end)
{
	RECT rect;
	rect.left = start.x;
	rect.top = start.y;
	rect.right = end.x;
	rect.bottom = end.y;
	device->SetScissorRect(&rect); //???????????
}

void DirectXRenderer::DrawLine(Vector2 start, Vector2 end, D3DCOLOR colour)
{
	std::vector<RendererVertex> vertex{ {}, {} };
	vertex[0].x = start.x;
	vertex[0].y = start.y;
	vertex[0].z = 1;

	vertex[1].x = end.x;
	vertex[1].y = end.y;
	vertex[1].z = 1;

	vertex[0].colour = vertex[1].colour = colour;


	PushRenderCommand(vertex, D3DPT_LINELIST, 1);
}

void DirectXRenderer::DrawRectangle(Vector2 start, Vector2 end, D3DCOLOR colour, bool filled) {
	if (filled) {
		std::vector<RendererVertex> vertex{ {}, {}, {}, {} };
		vertex[0].x = start.x;
		vertex[0].y = start.y;
		vertex[0].z = 1;
		vertex[0].colour = colour;

		vertex[1].x = end.x;
		vertex[1].y = start.y;
		vertex[1].z = 1;
		vertex[1].colour = colour;

		vertex[2].x = start.x;
		vertex[2].y = end.y;
		vertex[2].z = 1;
		vertex[2].colour = colour;

		vertex[3].x = end.x;
		vertex[3].y = end.y;
		vertex[3].z = 1;
		vertex[3].colour = colour;

		PushRenderCommand(vertex, D3DPT_TRIANGLESTRIP, 2);
	}
	else {
		std::vector<RendererVertex> vertex{ {}, {}, {}, {}, {} };
		vertex[0].x = start.x;
		vertex[0].y = start.y;
		vertex[0].z = 1;
		vertex[0].colour = colour;

		vertex[1].x = end.x;
		vertex[1].y = start.y;
		vertex[1].z = 1;
		vertex[1].colour = colour;

		vertex[2].x = end.x;
		vertex[2].y = end.y;
		vertex[2].z = 1;
		vertex[2].colour = colour;

		vertex[3].x = start.x;
		vertex[3].y = end.y;
		vertex[3].z = 1;
		vertex[3].colour = colour;

		vertex[4].x = start.x;
		vertex[4].y = start.y;
		vertex[4].z = 1;
		vertex[4].colour = colour;

		PushRenderCommand(vertex, D3DPT_LINESTRIP, 4);
	}
}

void DirectXRenderer::DrawCircle(Vector2 centre, float radius, D3DCOLOR colour, bool filled) {
	radius = fabsf(radius);
	double step = (3.141 * 2.0) / radius;
	if (!filled) {
		for (double a = 0; a < (3.141 * 2.0); a += step) {
			float x1 = float(radius * cos(a) + centre.x);
			float y1 = float(radius * sin(a) + centre.y);
			float x2 = float(radius * cos(a + step) + centre.x);
			float y2 = float(radius * sin(a + step) + centre.y);
			DrawLine(Vector2(x1, y1), Vector2(x2, y2), colour);
		}
	}
	else {
		std::vector<RendererVertex> vertex{};
		for (double i = 0.0; i < 3.141 * 2.0; i += step) {
			RendererVertex vertexx;
			vertexx.x = float(centre.x + radius * cosf(i));
			vertexx.y = float(centre.y + radius * sinf(i));
			vertexx.colour = colour;
			vertex.push_back(vertexx);
		}
		PushRenderCommand(vertex, D3DPT_TRIANGLEFAN, vertex.size());
	}
}


void DirectXRenderer::DrawGradientRectangle(Vector2 start, Vector2 end, D3DCOLOR colourstart, D3DCOLOR colourend, bool filled) {
	if (filled) {
		std::vector<RendererVertex> vertex{ {}, {}, {}, {} };
		vertex[0].x = start.x;
		vertex[0].y = start.y;
		vertex[0].z = 1;
		vertex[0].colour = colourstart;

		vertex[1].x = end.x;
		vertex[1].y = start.y;
		vertex[1].z = 1;
		vertex[1].colour = colourstart;

		vertex[2].x = start.x;
		vertex[2].y = end.y;
		vertex[2].z = 1;
		vertex[2].colour = colourend;

		vertex[3].x = end.x;
		vertex[3].y = end.y;
		vertex[3].z = 1;
		vertex[3].colour = colourend;

		PushRenderCommand(vertex, D3DPT_TRIANGLESTRIP, 2);
	}
	else {
		std::vector<RendererVertex> vertex{ {}, {}, {}, {}, {} };
		vertex[0].x = start.x;
		vertex[0].y = start.y;
		vertex[0].z = 1;
		vertex[0].colour = colourstart;

		vertex[1].x = end.x;
		vertex[1].y = start.y;
		vertex[1].z = 1;
		vertex[1].colour = colourstart;

		vertex[2].x = end.x;
		vertex[2].y = end.y;
		vertex[2].z = 1;
		vertex[2].colour = colourend;

		vertex[3].x = start.x;
		vertex[3].y = end.y;
		vertex[3].z = 1;
		vertex[3].colour = colourend;

		vertex[4].x = start.x;
		vertex[4].y = start.y;
		vertex[4].z = 1;
		vertex[4].colour = colourstart;

		PushRenderCommand(vertex, D3DPT_LINESTRIP, 4);
	}
}



void DirectXRenderer::DrawGradientRectangle(Vector2 start, Vector2 end, D3DCOLOR corner1, D3DCOLOR corner2, D3DCOLOR corner3, D3DCOLOR corner4, bool filled) {
	if (filled) {
		std::vector<RendererVertex> vertex{ {}, {}, {}, {} };
		vertex[0].x = start.x;
		vertex[0].y = start.y;
		vertex[0].z = 1;
		vertex[0].colour = corner1;

		vertex[1].x = end.x;
		vertex[1].y = start.y;
		vertex[1].z = 1;
		vertex[1].colour = corner2;

		vertex[2].x = start.x;
		vertex[2].y = end.y;
		vertex[2].z = 1;
		vertex[2].colour = corner3;

		vertex[3].x = end.x;
		vertex[3].y = end.y;
		vertex[3].z = 1;
		vertex[3].colour = corner4;

		PushRenderCommand(vertex, D3DPT_TRIANGLESTRIP, 2);
	}
	else {
		std::vector<RendererVertex> vertex{ {}, {}, {}, {}, {} };
		vertex[0].x = start.x;
		vertex[0].y = start.y;
		vertex[0].z = 1;
		vertex[0].colour = corner1;

		vertex[1].x = end.x;
		vertex[1].y = start.y;
		vertex[1].z = 1;
		vertex[1].colour = corner2;

		vertex[2].x = end.x;
		vertex[2].y = end.y;
		vertex[2].z = 1;
		vertex[2].colour = corner3;

		vertex[3].x = start.x;
		vertex[3].y = end.y;
		vertex[3].z = 1;
		vertex[3].colour = corner4;

		vertex[4].x = start.x;
		vertex[4].y = start.y;
		vertex[4].z = 1;
		vertex[4].colour = corner4;

		PushRenderCommand(vertex, D3DPT_LINESTRIP, 4);
	}
}


void DirectXRenderer::DrawRoundedRectangle(Vector2 start, Vector2 end, D3DCOLOR colour, int radius, bool filled) {
	DrawRectangle(start, end, colour, filled); //IMPLEMENT ME!
}

Vector2 DirectXRenderer::GetTextExtents(const char* string, const char* fontName, int size, bool antiAliased) {
	float x = 0;
	float y = 0;

	Font_t font;

	font.antiAliased = antiAliased;
	strcpy_s(font.fontName, 64, fontName);
	font.size = size;


	while (*string) {
		char c = *string;
		string++;
		std::map<char, Character_t> chars = FindFont(font);
		Character glyph = chars[c];
		y = max(y, glyph.sizey);
		x += (glyph.advance / 64);
	}
	return Vector2(x, y);
}
#ifdef _DEBUG
#pragma comment (lib, "freetyped.lib")
#endif
#ifdef _RELEASE
#pragma comment (lib, "freetype.lib")
#endif
DirectXRenderer g_DirectXRenderer;