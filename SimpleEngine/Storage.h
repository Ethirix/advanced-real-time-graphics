#pragma once
#include <optional>
#include <string>
#include <unordered_map>

template <typename T>
class Storage
{
public:
	bool Store(std::string key, T object)
	{
		auto ret = _objectMap.try_emplace(key, object);
		return ret.second;
	}

	std::optional<T> Retrieve(std::string key)
	{
		if (auto result = _objectMap.find(key); result != _objectMap.end())
			return result->second;

		return {};
	}

	std::optional<std::string> GetKeyFromObject(T object)
	{
		for (auto pair : _objectMap)
			if (pair.second == object)
				return pair.first;

		return {};
	}

private:
	std::unordered_map<std::string, T> _objectMap;
};

