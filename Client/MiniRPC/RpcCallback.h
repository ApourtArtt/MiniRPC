#pragma once
#include <map>
#include <functional>
#include "RpcResponse.h"

class RpcCallback
{
public:
	RpcCallback() {}
	~RpcCallback() {}

	bool Add(uint32_t index, std::function<void(RpcResponse)>& Callback)
	{
		if (callbacks.find(index) != callbacks.end())
			return false;

		callbacks.insert({ index, std::move(Callback) });
		return true;
	}

	void Process(uint32_t index)
	{
		if (callbacks.find(index) == callbacks.end())
			return;

		callbacks[index](RpcResponse());
		callbacks.erase(index);
	}

private:
	std::map<uint32_t, std::function<void(RpcResponse)>> callbacks;
};
