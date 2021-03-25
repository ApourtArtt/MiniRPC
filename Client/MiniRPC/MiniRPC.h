#pragma once
#include <asio.hpp>
#include "RpcCallback.h"


class MiniRPC
{
public:
	MiniRPC(asio::io_context& IoCtx, std::string Ip, unsigned short Port, std::string Password);
	~MiniRPC();

	bool Initialize();
	void Send(const std::string& message, std::function<void(RpcResponse)> callback);

private:
	void login();
	void write(std::string& msg);
	void read();

	asio::io_context& ioCtx;
	std::string ip;
	unsigned short port;
	std::string password;

	char buffer[400]; // @TODO

	RpcCallback rpcCb;
	uint32_t lastIndex;

	asio::ip::tcp::socket socket;
	std::thread rpcThread;
};
