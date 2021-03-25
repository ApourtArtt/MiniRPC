#include "MiniRPC.h"

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

	rpc.Send("Hey !", [](RpcResponse resp) {
		printf("Example 1\n");
		});

	rpc.Send("Hoy !", [](RpcResponse resp) {
		printf("Example 2\n");
		});

	while (true) { Sleep(5000); } // ...
}
