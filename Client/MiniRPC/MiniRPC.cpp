#include "MiniRPC.h"
#include <iostream>

bool MiniRPC::Initialize()
{
    asio::error_code error;

    socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port), error);
    if (error)
    {
        std::cout << "MiniRPC error while connecting : " << error.message() << std::endl;
        return false;
    }

    login();
    read();

    //rpcThread = std::thread([=] {
        ioCtx.run();
    //});



    return true;
}

MiniRPC::MiniRPC(asio::io_context& IoCtx, std::string Ip, unsigned short Port, std::string Password)
    : ioCtx(IoCtx)
    , ip(Ip)
    , port(Port)
    , password(Password)
    , socket(ioCtx)
{
    memset(buffer, 0, sizeof(buffer));
}

MiniRPC::~MiniRPC()
{
}

void MiniRPC::login()
{
    write(password);
}

void MiniRPC::write(std::string& msg)
{
    asio::async_write(socket, asio::buffer(msg),
        [this](std::error_code ec, size_t length)
        {
            printf("%s %d\n", ec.message().c_str(), length);
        });
}

void MiniRPC::read()
{
    socket.async_receive(asio::buffer(buffer), [self = this](const asio::error_code& ec, size_t bytes)
        {
            if (ec)
            {
                std::cout << "Disconnect, failed to receive : %s\n", ec.message();
                return;
            }
            if (bytes > 0)
            {
                std::cout << self->buffer;
            }
            self->read();
        });
}
