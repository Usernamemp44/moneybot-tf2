#include "Input.h"
#include "../../Misc/PatternFinder.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../Hooks.h"
#include <intrin.h>
#include "../Misc/VMT.h"
#include "../../SourceSDK/Interfaces/CClientState/CClientState.h"
#include "../SourceSDK/Structures/bitbuf/bitbuf.h"

// Largest # of commands to send in a packet
#define NUM_NEW_COMMAND_BITS		4
#define MAX_NEW_COMMANDS			((1 << NUM_NEW_COMMAND_BITS)-1)

// Max number of history commands to send ( 2 by default ) in case of dropped packets
#define NUM_BACKUP_COMMAND_BITS		3
#define MAX_BACKUP_COMMANDS			((1 << NUM_BACKUP_COMMAND_BITS)-1)

#define MAX_CMD_BUFFER				4000

class CLC_Move {
	public:
		int				m_nBackupCommands;
		int				m_nNewCommands;
		int				m_nLength;
		bf_read			m_DataIn;
		bf_write		m_DataOut;
};

void WriteUserCmd(bf_write *buf, CUserCmd_t *to, CUserCmd_t *from) {
	typedef void(__cdecl* Fn)(void*, CUserCmd_t*, CUserCmd_t*);
	static Fn WriteUserCmd = reinterpret_cast<Fn>(FindPatternInClient("55 8B EC 83 EC 08 53 8B 5D 0C 56 8B 75 10 57 8B 7D 08 8B 46 04 40 39 43 04 8B 47 0C 0F 84 ?? ?? ?? ??"));

	WriteUserCmd(buf, to, from);
}

int32_t nTickBaseShift = 0;
int32_t nSinceUse = 0;
bool bInSendMove = false, bFirstSendMovePack = false;
using CL_SendMove_Type = void(__cdecl*)(void);
using WriteUsercmdDeltaToBufferType = bool(__thiscall*)(void*, void*, int, int, bool);
bool __fastcall WriteUsercmdDeltaToBufferHook(void * _this, void * edx, bf_write * buf, int from, int to, bool isNewCmd)
{
	static CL_SendMove_Type CL_SendMove = (CL_SendMove_Type)FindPatternInEngine("55 8B EC 81 EC ?? ?? ?? ?? A1 ?? ?? ?? ?? 8D");
	static void* CL_SendMove_Return = (void*)FindPatternInEngine("84 C0 74 04 B1 01");

	static int m_nLastOutgoingCommand = **(int**)(FindPatternInEngine("8B 0D ?? ?? ?? ?? A1 ?? ?? ?? ?? 41") + 0x2);
	static int m_nChokedCommands = **(int**)(FindPatternInEngine("A1 ?? ?? ?? ?? 41 03 C1") + 0x1);

	WriteUsercmdDeltaToBufferType WriteUsercmdDeltaToBufferOriginal = InputHook->GetMethod< WriteUsercmdDeltaToBufferType>(WRITE_USERCMD_DELTA_TO_BUFFER_INDEX);

	if (nTickBaseShift <= 0 || _ReturnAddress() != CL_SendMove_Return)
		return WriteUsercmdDeltaToBufferOriginal(_this, buf, from, to, isNewCmd);
	if (from != -1)
		return true;


	auto msg = reinterpret_cast<CLC_Move*>((uintptr_t) _AddressOfReturnAddress() + 0xFCC);

	//int *pNumBackupCommands = (int*)(reinterpret_cast<uintptr_t>(buf) - 0x30);
	//int *pNumNewCommands = (int*)(reinterpret_cast<uintptr_t>(buf) - 0x2C);	
	auto net_channel = CEngineClient::factory()->GetNetChannelInfo();

	int32_t new_commands = msg->m_nNewCommands;

	if (!bInSendMove)
	{
		if (new_commands <= 0) {
//			nTickBaseShift = 0;

			return false;
		}

		bInSendMove = true;
		bFirstSendMovePack = true;
		nTickBaseShift += new_commands;

		while (nTickBaseShift > 0)
		{
			CL_SendMove();
			net_channel->Transmit(false);
			bFirstSendMovePack = false;
		}

		bInSendMove = false;
		return false;
	}

	int nextCmdNr = m_nLastOutgoingCommand + m_nChokedCommands + 1;
	int totalNewCommands = min(
		nTickBaseShift, 13);
	nTickBaseShift -= totalNewCommands;

	from = -1;
	msg->m_nNewCommands = totalNewCommands;
	msg->m_nBackupCommands = 0;

	for (to = nextCmdNr - new_commands + 1; to <= nextCmdNr; to++)
	{
		if (!WriteUsercmdDeltaToBufferOriginal(_this, buf, from, to, true))
			return false;

		from = to;
	}

	//byte data[MAX_CMD_BUFFER];
	//msg->m_DataOut.StartWriting(data, sizeof(data));

	CUserCmd_t* pLastRealCmd = (CUserCmd_t*)IInput::factory()->GetUserCmd(from);
	CUserCmd_t cmdFrom;;

	if (pLastRealCmd)
		cmdFrom = *pLastRealCmd;

	// Make the CUserCmd invalid so it doesn't get processed but still shifts tickbase
	CUserCmd_t cmdTo = cmdFrom;
	cmdTo.CommandNumber++;
	cmdTo.TickCount += 200;

	for (int i = new_commands; i <= totalNewCommands; i++)
	{
		WriteUserCmd(buf, &cmdTo, &cmdFrom);
		cmdFrom = cmdTo;
		cmdTo.CommandNumber++;
		cmdTo.TickCount++;
	}

	//msg->m_nLength = buf->GetNumBitsWritten();
	//msg->m_DataOut = data;

	return true;
}
