#pragma once
#include <vector>
#include <string>
#include <map>
#include "BaseCheat.h"

//extern std::map<std::string, BaseCheat*> CheatModules;

#define CheatModules (*GetCheatModules())
std::map<std::string, BaseCheat*>* GetCheatModules();