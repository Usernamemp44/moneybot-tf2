#pragma once

struct CUserCmd_t;
class bf_write;

class IInput {
public:
	bool WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
	CUserCmd_t* GetUserCmd(int iSequenceNumber);
	static IInput* factory();
	IInput factory_noptr();
};