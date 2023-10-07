#include "CheatList.h"

std::map<std::string, BaseCheat*>* _CheatModules = NULL;
std::map<std::string, BaseCheat*>* GetCheatModules() {
	if (_CheatModules == NULL)
		_CheatModules = new std::map<std::string, BaseCheat*>();
	return _CheatModules;
}