#pragma once
#include <utility>
#include <string>
#include <vector>
#include <charconv>
#include <sstream>
#include <algorithm>

template <typename T>
static std::pair<T, bool> ToNumber(const char* str)
{
	if (!str)
		return std::make_pair(0, false);

	T var = 0;
	auto [p, ec] = std::from_chars(str, str + strlen(str), var);

	if (ec != std::errc())
		return std::make_pair(0, false);

	return std::make_pair((T)var, true);
}

static std::vector<std::string> splitString(const std::string& origin, const std::string& delim)
{
    std::vector<std::string> output;
    auto itr = origin.begin();
    auto end1 = origin.end();
    auto start2 = delim.begin();
    auto end2 = delim.end();
    auto size = delim.length();
    while (true)
    {
        auto find_itr = std::search(itr, end1, start2, end2);
        if (find_itr == end1)
        {
            output.emplace_back(itr, end1);
            break;
        }
        output.emplace_back(itr, find_itr);
        itr = find_itr;
        itr += size;
    }
    return output;
}
