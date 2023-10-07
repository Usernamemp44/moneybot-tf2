#include "DirectX.h"

#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_win32.h"
#include "..\imgui\imgui_impl_dx9.h"

#include "..\Renderer.h"

#include "../Cheats/CheatList.h"
#include "../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"

#include "../SourceSDK/Interfaces/CHud/CHud.h"
#include "../SourceSDK/Interfaces/CTextureDictionary/CTextureDictionary.h"


#include "../SourceSDK/Interfaces/ISurface/ISurface.h"
#include "../Cheats/Miscellaneous/Console/Console.h"
#include "../imgui/imgui_freetype.h"
#include "../SourceSDK/Interfaces/ConVar/ConVar.h"
#include <time.h>
#include "../carbon/renderer/d3d9.hpp"
#include "../carbon/renderer/font.hpp"
#include "../carbon/carbon.hpp"
#include "../carbon/renderer/c_input.hpp"

#include "../carbon/handler/Handler.h"

void * PresentOriginal;
void * ResetOriginal;

bool Initialized = false;

typedef long(__stdcall* Present)(LPDIRECT3DDEVICE9, void*, void*, HWND, void*);
typedef long(__stdcall* Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS *pPresentationParameters);

bool MenuOpen = false;
void MenuRenderLoop() {
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		MenuOpen = !MenuOpen;
		ISurface::factory()->SetCursorAlwaysVisible(MenuOpen);
	}
	if (MenuOpen) {
		if (ImGui::Begin("NX", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginTabBar("MainMenu")) {

				for (auto const& x : CheatModules)
				{
					if (!x.second->CanRender)
						continue;
					if (x.second->Group != NULL)
					{
						ImGui::BeginTabItem(x.second->Group);
						ImGui::BeginTabBar(x.second->Group);
					}
					if (ImGui::BeginTabItem(x.first.c_str())) {

						x.second->RenderMenu();
						ImGui::EndTabItem();
					}
					if (x.second->Group != NULL)
					{
						ImGui::EndTabBar();
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}

		}
		ImGui::End();
	}
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



WNDPROC oldWindowProc;
long __stdcall WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (MenuOpen) {
		switch (uMsg) {
			case WM_MOUSEMOVE:
				input::capture_mouse_move(lParam);
				break;
			default:
				input::register_key_press(uMsg, wParam);
				break;
		}

		return 1;
	}

	return CallWindowProc(oldWindowProc, hWnd, uMsg, wParam, lParam);
}

void ImGui_ApplyStyle() {
	ImGuiIO& io = ImGui::GetIO();

	ImGuiStyle &Style = ImGui::GetStyle();

	Style.AntiAliasedFill = false;
	//Style.AntiAliasedLines = false;
	Style.TabRounding = 0;
	Style.WindowBorderSize = 0.0f;
	Style.WindowRounding = 0;
	Style.ScrollbarRounding = 0;
	Style.FrameBorderSize = 1.0f;
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
	colors[ImGuiCol_Border] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(.60f, .60f, .60f, .85f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.55f, 0.55f, .85f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.6f, 0.6f, 0.6f, .85f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.0f);
	colors[ImGuiCol_TabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.39f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 1.0f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_Highlight] = ImVec4(0.f, 1.f, 1.f, 1.f);
	Style.GrabMinSize = 0.1;
	Style.AntiAliasedFill = false;
	//Style.AntiAliasedLines = false;
}

bool init = false;
static auto boolean_test = false;
static auto slider_test = 0.0f;
static char* textbox_test;
static uint32_t hotkey_test = 0;
static auto boolean_test2 = false;
static auto progress_bar_test = 0.0f;
static carbon::io_color color_test = { 158, 0, 255, 180 };

void testmenu(LPDIRECT3DDEVICE9 pDevice) {
	//pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(88, 88, 88), 1.0f, 0);
	//pDevice->BeginScene();

	
	if (d3d9::run_frame()) {
		d3d9::begin();
		carbon::new_frame();
		static auto window_element = std::make_shared<carbon::element::io_window>(U"moneybot", "window", 1, carbon::io_vec4{ 150, 150, 579, 496 }, carbon::element::io_window::window_flags_javascript | carbon::element::io_window::window_flags_dedicated_window);

		if (!init) {
			init = true;

			auto tab_sheet = std::make_shared<carbon::element::io_tab_sheet>();
			Handler::Init(tab_sheet);

			// Creates new tabs (On the left)
			auto tab_legit = Handler::RegisterPage("Legit", U"Legit"); {
				// Creates new tabs (On the top)
				auto sub_main_first = Handler::RegisterSubPage("Legit", "First");
				auto sub_main_second = Handler::RegisterSubPage("Legit", "Second");
			}

			auto tab_rage = Handler::RegisterPage("Rage", U"Rage"); {

			}

			auto tab_visual = Handler::RegisterPage("Visual", U"Visual"); {

				auto sub_enemies = Handler::RegisterSubPage("Visual", "Enemies"); {
					auto mainGroupbox = Handler::RegisterGroupBox("Visual", "Enemies", "Main", carbon::io_vec4{ 16, 0, 210, 320 }); {
						mainGroupbox->add_child(std::make_shared<carbon::element::io_checkbox>("Enable", "enabled enemies", carbon::io_vec2{ 16, 38 }, &boolean_test))->add_child(std::make_shared<carbon::element::io_tooltip>("Enables esp for enemies"));
					}

				}
				auto sub_friendlies = Handler::RegisterSubPage("Visual", "Friendlies");
				auto sub_local = Handler::RegisterSubPage("Visual", "Local");
				auto sub_world = Handler::RegisterSubPage("Visual", "World");
			}

			auto tab_misc = Handler::RegisterPage("Misc", U"Misc"); {

			}

			auto tab_config = Handler::RegisterPage("Config", U"Config"); {

			}


			// Creates new group box
			auto sub_test_group = Handler::RegisterGroupBox("Legit", "First", "groupbox", carbon::io_vec4{ 16, 0, 210, 320 });



			sub_test_group->add_child(std::make_shared<carbon::element::io_checkbox>("checkbox", "test_checkbox", carbon::io_vec2{ 16, 38 }, &boolean_test));
			auto sub_test_button = std::make_shared<carbon::element::io_button>("beep", "beep_button", carbon::io_vec4{ 16, 38, 120, 30 }, []() {
				Beep(250, 250);
			});
			// was just testing tooltips
			sub_test_button->add_child(std::make_shared<carbon::element::io_tooltip>("hello there"));
			sub_test_group->add_child(sub_test_button);
			// todo: slider format
			sub_test_group->add_child(std::make_shared<carbon::element::io_slider<float>>("float slider", "test_slider", carbon::io_vec4{ 16, 38, 120, 0 }, &slider_test, 0.0f, 100.0f, U"%"));
			sub_test_group->add_child(std::make_shared<carbon::element::io_label>("im a label", "test_label", carbon::io_vec2{ 16, 38 }));
			sub_test_group->add_child(std::make_shared<carbon::element::io_textbox>("textbox", "test_textbox", carbon::io_vec4{ 16, 38, 175, 0 }, "", 50, false, textbox_test));
			sub_test_group->add_child(std::make_shared<carbon::element::io_label>("another label", "test_label2", carbon::io_vec2{ 16, 38 }));
			auto test_checkbox2 = std::make_shared<carbon::element::io_checkbox>("another checkbox", "test_checkbox2", carbon::io_vec2{ 16, 38 }, &boolean_test2);
			test_checkbox2->add_child(std::make_shared<carbon::element::io_tooltip>("you can switch this button"));
			test_checkbox2->add_child(std::make_shared<carbon::element::io_color_picker>("test_colorpicker", carbon::io_vec2{ 170, 0 }, &color_test));
			sub_test_group->add_child(test_checkbox2);
			sub_test_group->add_child(std::make_shared<carbon::element::io_progress_bar>(carbon::io_vec4{ 16, 38, 80, 0 }, &progress_bar_test));
			//sub_test_group->add_child(std::make_shared<carbon::element::io_hotkey>("test", carbon::io_vec2{ 100, 38 }, &hotkey_test));

			window_element->add_child(tab_sheet);
		}
		carbon::add_window(window_element);
		carbon::end_frame();
		d3d9::draw();
		d3d9::end();
	}
//	pDevice->EndScene();
	//pDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

long __stdcall PresentHook(LPDIRECT3DDEVICE9 pDevice, void * a1, void * a2, HWND a3, void * a4) {
	static bool bD3D9_Init = false;
	if (!bD3D9_Init) {
		d3d9::initialize(pDevice);
		carbon::initialize();
		bD3D9_Init = true;

		// IMGUI STUFF
		ImGui::CreateContext();
		ImGui_ApplyStyle();

		D3DDEVICE_CREATION_PARAMETERS cparams;

		pDevice->GetCreationParameters(&cparams);

		ImGui_ImplWin32_Init(cparams.hFocusWindow);
		ImGui_ImplDX9_Init(pDevice);
		//ImGuiIO& io = ImGui::GetIO();

		//ImFontConfig config;
		//config.OversampleH = 1;
		//config.OversampleV = 1;
		//config.OversampleH = 4;
		//config.OversampleV = 4;

		//for (int i = 1; i < 51; i++) {
		//	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", i, &config); //scale it down
		//}
		//ImGuiFreeType::BuildFontAtlas(io.Fonts, ImGuiFreeType::Monochrome | ImGuiFreeType::MonoHinting);


		// WncProc Hook for Carbon
		oldWindowProc = (WNDPROC)SetWindowLongPtr(cparams.hFocusWindow, GWL_WNDPROC, (LONG_PTR)&WndProcHook);
	}

	// sRGB fix (fixes colors)
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

//	IMaterial* hi = CTextureDictionary::factory()->GetTextureMaterial(1);
	//CHudTexture* test = CHud::factory()->GetIcon("shotgun_primary");

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		MenuOpen = !MenuOpen;
		ISurface::factory()->SetCursorAlwaysVisible(MenuOpen);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	if (MenuOpen) {	
		testmenu(pDevice);
	}

	// FOR CHEAT VISUALS
	if (CEngineClient::factory()->IsInGame()) {
		for (auto &x : CheatModules) {
			x.second->Render();
		}
	}
	//Console::Render();
	//MenuRenderLoop();

	//pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	Present orig = (Present)PresentOriginal;
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	long ret = orig(pDevice, a1, a2, a3, a4);
	return ret;
}

HRESULT ResetHook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters) {
	d3d9::invalidate_objects();
	ImGui_ImplDX9_InvalidateDeviceObjects();

	Reset orig = (Reset)ResetOriginal;
	HRESULT frick = orig(pDevice, pPresentationParameters);

	d3d9::create_objects();
	ImGui_ImplDX9_CreateDeviceObjects();

	return frick;
}