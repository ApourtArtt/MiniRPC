#pragma once
#include <asio.hpp>

class MiniRPC
{
public:
	MiniRPC(asio::io_context& IoCtx, std::string Ip, unsigned short Port, std::string Password);
	~MiniRPC();

	bool Initialize();

private:
	void login();
	void write(std::string& msg);
	void read();

	asio::io_context& ioCtx;
	std::string ip;
	unsigned short port;
	std::string password;

	char buffer[400];

	asio::ip::tcp::socket socket;
	std::thread rpcThread;
};
