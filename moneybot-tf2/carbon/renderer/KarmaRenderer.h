#pragma once
#include <d3d9.h>
#include <vector>
#include <map>
#undef CreateFont

typedef enum FontFlags_t {
	Outline = 1,
	Dropshadow = 2,
	CenteredX = 4,
	CenteredY = 8
} FontFlags;
typedef struct Vector2_t {
	float x, y;
	Vector2_t(float a, float b) {
		x = a;
		y = b;
	};
} Vector2;

typedef struct Vector3_t {
	float x, y, z;
} Vector3;
typedef struct Character_t {
	IDirect3DTexture9* texture;
	UINT sizex, sizey;
	int bearingx, bearingy;
	int advance;
	bool exists = false;
} Character;
typedef struct RendererVertex_t {
	float x, y, z, rhw = 1.0f;
	D3DCOLOR colour;
	float u = 0;
	float v = 0;
} RendererVertex;

typedef struct RendererCommand_t {
	std::vector<RendererVertex> vertices;
	IDirect3DTexture9* texture;
	D3DPRIMITIVETYPE type;
	int primitiveCount = 0;
	bool isScissor = false;
} RendererCommand;

typedef struct Font_t {
	char fontName[64];
	int size;
	bool antiAliased;

	char fontPath[260];
} Font;


class DirectXRenderer {
	LPDIRECT3DDEVICE9 device;
	std::vector<RendererCommand> rendererCommands;
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer = 0;
	uint32_t allocatedVertexBufferSize = 0;

	std::map<uint32_t, std::map<char, Character_t>> fontMap;
	IDirect3DStateBlock9 *state;
	IDirect3DStateBlock9 *oldstate;

public:
	void CreateFont(Font_t font);

	std::map<char, Character_t> FindFont(Font_t);

	void Initialize(LPDIRECT3DDEVICE9 device);

	void DrawRenderCommands(LPDIRECT3DDEVICE9);

	void PushRenderCommand(RendererCommand command);
	void PushRenderCommand(RendererCommand* command);
	void PushRenderCommand(std::vector<RendererVertex> vertices, D3DPRIMITIVETYPE type, int primitiveCount, IDirect3DTexture9* texture = 0);

	void DrawTexturedQuad(Vector2 min, Vector2 max, D3DCOLOR color, IDirect3DTexture9 * texture);

	void DrawCharacter(char chr, Font_t font, Vector2 location, D3DCOLOR color);

	void DrawString(const char * string, Vector2 location, D3DCOLOR color = 0xFFFFFFFF, int flags = 0, const char* fontName = "Arial", int size = 32, bool antiAliased = true);

	void CreateObjects(LPDIRECT3DDEVICE9 device);
	void DeleteObjects(void);

	void SetScissorRect(Vector2 start, Vector2 end);
	void DrawLine(Vector2 start, Vector2 end, D3DCOLOR colour);
	void DrawRectangle(Vector2 start, Vector2 end, D3DCOLOR colour, bool filled);
	void DrawCircle(Vector2, float radius, D3DCOLOR colour, bool filled);
	void DrawGradientRectangle(Vector2 start, Vector2 end, D3DCOLOR colourstart, D3DCOLOR colourend, bool filled);
	void DrawGradientRectangle(Vector2 start, Vector2 end, D3DCOLOR corner1, D3DCOLOR corner2, D3DCOLOR corner3, D3DCOLOR corner4, bool filled);
	void DrawRoundedRectangle(Vector2 start, Vector2 end, D3DCOLOR colour, int radius, bool filled);
	Vector2 GetTextExtents(const char * string, const char * fontName, int size, bool antiAliased);
};
extern DirectXRenderer g_DirectXRenderer;