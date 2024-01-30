#include "Helpers.h"

#include "DataStore.h"

//Must be deleted after usage!
wchar_t* Helpers::StringToWideString(std::string str)
{
	str.append(" ");
	auto* wideString = new wchar_t[str.size() + 1];
	size_t outSize;
	auto error = mbstowcs_s(&outSize, wideString,
		str.size(), str.c_str(), str.size() - 1);

	return wideString;
}