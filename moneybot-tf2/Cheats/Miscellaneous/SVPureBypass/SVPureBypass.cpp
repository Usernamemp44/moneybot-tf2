#include "SVPureBypass.h"
#include "../../../imgui/imgui_internal.h"
#include "../../../Misc/PatternFinder.h"
#include "../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../../Cheats/CheatList.h"
#include "../Console/Console.h"
void SVPureBypass::RenderMenu()
{
	if (bypassed) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("Disable sv_pure")) {
		
		if (!CEngineClient::factory()->IsInGame() && bypassed == false)
		{
			void** pure_addr = nullptr;

			if (!pure_addr)
			{
				pure_addr = *reinterpret_cast<void***>(FindPatternInEngine(_strdup("A1 ? ? ? ? 56 33 F6 85 C0")) + 1);
			}
			
			*pure_addr = (void*)0;

			bypassed = true;
			Console::puts("sv_pure disabled.");
			return;
		}
	}
	if (bypassed)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

SVPureBypass::SVPureBypass() {
	this->CanRender = true;
	CheatModules["SVPureBypass"] = this;
}
SVPureBypass g_SVPureBypass;