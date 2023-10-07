#pragma once

#include <stdint.h>

/*class CClientState
{
public:
	char pad_0000[148]; //0x0000
	class INetChannel* m_pNetChannel; //0x0094
	char pad_0098[16]; //0x0098
	unsigned int m_nRetryNumber; //0x00A8
	char pad_00AC[84]; //0x00AC
	int m_nSignonState; //0x0100
	char pad_0104[8]; //0x0104
	float m_flNextCmdTime; //0x010C
	int m_nServerCount; //0x0110
	int m_nCurrentSequence; //0x0114
	char pad_0118[75]; //0x0118
	int m_nServerTick; //0x0163
	int m_nClientTick; //0x0167
	int m_nDeltaTick; //0x016B
	char pad_016F[4]; //0x016F
	int m_nViewEntity; //0x0173
	char pad_0177[8]; //0x0177
	char m_szLevelName[260]; //0x017F
	char m_szLevelNameShort[40]; //0x0283
	char pad_02AB[18940]; //0x02AB
	char pad_0x2DA0; //0x4CA7
	int lastoutgoingcommand; //0x4CA8
	int chokedcommands; //0x4CAC
	int last_command_ack; //0x4CB0
	int command_ack; //0x4CB4
	int m_nSoundSequence; //0x4CB8
	char pad_4CBC[8]; //0x4CBC
	bool ishltv; //0x4CC4
}; //Size: 0x4CC5*/
class CClientState
{
public:
	char pad_0000[280]; //0x0000
	char *m_sRetrySourceTag; //0x0118
	uint32_t m_retryChallenge; //0x011C
	uint32_t m_nSignonState; //0x0120
	char pad_0124[4]; //0x0124
	double m_flNextCmdTime; //0x0128
	char pad_0130[96]; //0x0130
	uint32_t m_nDeltaTick; //0x0190
	char pad_0194[12]; //0x0194
	char m_szLevelFileName[40]; //0x01A0
	char m_szLevelBaseName[40]; //0x01C8
	uint32_t m_nMaxClients; //0x01F0
	char pad_01F4[67696]; //0x01F4
	uint32_t lastoutgoingcommand; //0x10A64
	uint32_t chokedcommands; //0x10A68
	uint32_t last_command_ack; //0x10A6C
	uint32_t command_ack; //0x10A70
	char pad_10A74[9744]; //0x10A74

	static CClientState* factory();
	CClientState factory_noptr();
}; //Size: 0x13084