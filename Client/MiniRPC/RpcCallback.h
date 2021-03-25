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

	void Process(RpcResponse resp)
	{
		uint32_t id = resp.GetCallbackId();

		if (callbacks.find(id) == callbacks.end())
			return;

		callbacks[id](resp);
		callbacks.erase(id);
	}

private:
	std::map<uint32_t, std::function<void(RpcResponse)>> callbacks;
};
