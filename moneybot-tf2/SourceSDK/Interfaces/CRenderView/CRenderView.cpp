#include "CRenderView.h"
#include "../../Misc/GetInterface.h"
CRenderView* cached_renderview = nullptr;
CRenderView * CRenderView::factory()
{
	if (cached_renderview == nullptr) {
		cached_renderview = (CRenderView*)(GetInterface("engine.dll", "VEngineRenderView014"));
	}
	return cached_renderview;
}