#pragma once
#include "../../BaseCheat.h"
struct CUserCmd_t;

class BunnyHop : public BaseCheat {
public:
	BunnyHop();
	void CreateMove(CUserCmd_t*&) override;
	void RenderMenu() override;
	bool Enabled;
};