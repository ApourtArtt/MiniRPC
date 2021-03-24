#include "MiniRPC.h"

int main(int argc, char *argv[])
{
	asio::io_context ioCtx;
	
	MiniRPC rpc(ioCtx, "127.0.0.1", 6000, "password\n");
	rpc.Initialize();

	printf("...");

	while (true) { Sleep(5000); } // ...
}
