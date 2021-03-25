#include "MiniRPC.h"
#include <iostream>

int main(int argc, char *argv[])
{
	asio::io_context ioCtx;
	
	MiniRPC rpc(ioCtx, "127.0.0.1", 6000, "password");
	if (!rpc.Initialize())
	{
		printf("Failed initializing RPC");
		return 0;
	}

	Sleep(5000);

	rpc.Send("Hey ! ergzergz er", [](RpcResponse resp) {
		std::cout << resp.GetPacket() << std::endl;
	});

	rpc.Send("Hoy !gz ergzer gze", [](RpcResponse resp) {
		std::cout << resp.GetPacket() << std::endl;
	});

	while (true) { Sleep(5000); } // ...
}
