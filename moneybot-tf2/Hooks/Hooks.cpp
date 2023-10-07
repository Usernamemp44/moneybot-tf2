#include "Hooks.h"
#include "DirectX.h"

#include "../Misc/PatternFinder.h"

#include <cassert>

#include "../SourceSDK/Interfaces/CHLClient/CHLClient.h"
#include "VMT/Client.h"

#include "../SourceSDK/Interfaces/CClientModeShared/CClientModeShared.h"
#include "VMT/ClientMode.h"

#include "../SourceSDK/Interfaces/IPrediction/IPrediction.h"
#include "VMT/Prediction.h"

#include "../SourceSDK/Interfaces/CRenderView/CRenderView.h"
#include "VMT/RenderView.h"

#include "../SourceSDK/Interfaces/IPanels/IPanels.h"
#include "VMT/Panels.h"

#include "../SourceSDK/Interfaces/ISurface/ISurface.h"
#include "VMT/Surface.h"

#include "../SourceSDK/Interfaces/CModelRender/CModelRender.h"
#include "VMT/ModelRender.h"

#include "../SourceSDK/Interfaces/IInput/IInput.h"
#include "VMT/Input.h"

#include "Regular/SetupBones.h"
//#include "../Cheats/Menu/Menu.h"

VMTHook* ClientHook;
VMTHook* ClientModeHook;
VMTHook* PredictionHook;
VMTHook* ModelRenderHook;
VMTHook* PanelsHook;
VMTHook* RenderViewHook;
VMTHook* SurfaceHook;
VMTHook* InputHook;

void InitializeHooks()
{
	{
		CHLClient* Client = CHLClient::factory();
		assert(Client);
		ClientHook = new VMTHook(Client);
		assert(ClientHook->HookFunction(FRAME_STAGE_NOTIFY_INDEX, &FrameStageNotifyHook));
	}

	{
		CClientModeShared* ClientMode = CClientModeShared::factory();
		assert(ClientMode);
		ClientModeHook = new VMTHook(ClientMode);
		ClientModeHook->HookFunction(CREATE_MOVE_INDEX, &CreateMoveHook);
		ClientModeHook->HookFunction(OVERRIDE_VIEW_INDEX, &OverrideViewHook);
	}

	/*{
		IPrediction* Prediction = IPrediction::factory();
		assert(Prediction);
		PredictionHook = new VMTHook(Prediction);
		PredictionHook->HookFunction(RUN_COMMAND_INDEX, &RunCommandHook);
	}
	*/
	{
		CModelRender* ModelRender = CModelRender::factory();
		assert(ModelRender);
		ModelRenderHook = new VMTHook(ModelRender);
		ModelRenderHook->HookFunction(DRAW_MODEL_EXECUTE_INDEX, &DrawModelExecuteHook);
	}

	
	{
		CRenderView* RenderView = CRenderView::factory();
		assert(RenderView);
		RenderViewHook = new VMTHook(RenderView);
		RenderViewHook->HookFunction(SCENE_END_INDEX, &SceneEndHook);
	}

	{
		IPanel* Panels = IPanel::factory();
		assert(Panels);
		PanelsHook = new VMTHook(Panels);
		PanelsHook->HookFunction(PAINT_TRAVERSE_INDEX, &PaintTraverseHook);
	}
		
	{
		ISurface* Surface = ISurface::factory();
		assert(Surface);
		SurfaceHook = new VMTHook(Surface);
		SurfaceHook->HookFunction(LOCK_CURSOR_INDEX, &LockCursorHook);
	}

	{
		IInput* Input = IInput::factory();
		assert(Input);
		InputHook = new VMTHook(Input);
		//InputHook->HookFunction(WRITE_USERCMD_DELTA_TO_BUFFER_INDEX, &WriteUsercmdDeltaToBufferHook);
	}
	//Menu menu;
	//menu.InitializeMenu();

	kiero::Status::Enum result = kiero::init(kiero::RenderType::D3D9);
	if (result != kiero::Status::Success)
		DebugBreak();
	kiero::bind(17, &PresentOriginal, &PresentHook);
	kiero::bind(16, &ResetOriginal, &ResetHook);



	MH_Initialize();
	
	void * SetupBones = SearchForStringXref("client.dll","*** ERROR: Bone access not allowed (entity %i:%s)");
	assert(SetupBones);
	//MH_CreateHook(SetupBones, &SetupBonesHook, &SetupBonesOriginal);

	//put MinHook hooks here

	MH_EnableHook(NULL);
}
