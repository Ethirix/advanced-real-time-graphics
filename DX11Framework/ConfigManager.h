#pragma once
#include <memory>

#include "Config.h"

#define SP_C std::shared_ptr<Config>

class ConfigManager
{
public:
	static SP_C GetConfig();
	
private:
	inline static SP_C _instance = nullptr;
};