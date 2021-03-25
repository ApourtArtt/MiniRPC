#pragma once
#include <string>
#include "Utils.h"
#include "RpcHeader.h"
#include <iostream>

class RpcResponse
{
public:
	RpcResponse(const std::string& Packet)
		: isBounded(false)
		, cbId(0)
		, packet(Packet)
		, header(RpcHeader::None)
	{
		std::string firstWord = Packet.substr(0, Packet.find_first_of(" "));

		auto [value, valid] = ToNumber<uint32_t>(firstWord.c_str());

		isBounded = std::move(valid);
		cbId = std::move(value);

		// Header...
	}

	bool IsBounded() const { return isBounded; }
	uint32_t GetCallbackId() const { return cbId; }
	const std::string& GetPacket() const { return packet; }

	RpcHeader GetHeader() const { return header; }

private:
	bool isBounded;
	uint32_t cbId;
	std::string packet;

	RpcHeader header;
};