#pragma once
#include "../../SourceSDK/Interfaces/IInput/IInput.h"
#define WRITE_USERCMD_DELTA_TO_BUFFER_INDEX 5
bool __fastcall WriteUsercmdDeltaToBufferHook(void* _this, void* edx, bf_write* buf, int from, int to, bool isNewCmd);
