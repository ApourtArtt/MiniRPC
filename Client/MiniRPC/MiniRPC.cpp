#include "MiniRPC.h"
#include <iostream>
#include <charconv>
#include "Utils.h"

bool MiniRPC::Initialize()
{
    asio::error_code error;

    socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port), error);
    if (error)
    {
        std::cout << "MiniRPC error while connecting : " << error.message() << std::endl;
        return false;
    }


    rpcThread = std::thread([=] {
        ioCtx.run();
    });

    login();
    read();

    return true;
}

MiniRPC::MiniRPC(asio::io_context& IoCtx, std::string Ip, unsigned short Port, std::string Password)
    : ioCtx(IoCtx)
    , ip(Ip)
    , port(Port)
    , password(Password)
    , socket(ioCtx)
    , lastIndex(0)
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
    std::string* newMsg = new std::string(msg + "\n");
    asio::async_write(socket, asio::buffer(*newMsg),
        [this, newMsg](std::error_code ec, size_t length)
        {
            printf("SENT %s %d\n", ec.message().c_str(), length);
            delete newMsg;
        });
}

void MiniRPC::read()
{
    socket.async_receive(asio::buffer(buffer), [this](const asio::error_code& ec, size_t bytes)
        {
            if (ec)
            {
                std::cout << "Disconnect, failed to receive : %s\n", ec.message();
                return;
            }
            if (bytes > 0)
            {
                std::string msg = buffer;
                std::string fword = msg.substr(0, msg.find_first_of(" "));

                auto [value, valid] = ToNumber<uint32_t>(fword.c_str());
                if (valid)
                {
                    rpcCb.Process(value);
                }
            }
            read();
        });
}

void MiniRPC::Send(const std::string& message, std::function<void(RpcResponse)> callback)
{
    while (!rpcCb.Add(lastIndex, callback))
        lastIndex++;

    std::string idx = std::to_string(lastIndex);

    std::string formatedMsg;
    formatedMsg.reserve(idx.size() + message.size() + 1);
    formatedMsg = std::to_string(lastIndex) + " " + message;

    write(formatedMsg);
}
