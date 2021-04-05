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

void MiniRPC::write(const std::string& msg)
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
                std::cout << "An error occured while trying to read : %s\n", ec.message();
            }
            else
            {
                if (bytes > 0)
                {
                    packet += std::string(buffer, bytes);

                    size_t pos;
                    while ((pos = packet.find_first_of('\n')) != std::string::npos)
                    {
                        std::string subPacket = packet.substr(0, pos);
                        RpcResponse resp(subPacket);
                        packet.erase(0, pos + 1);

                        if (resp.IsBounded())
                            rpcCb.Process(resp);
                    }
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

    lastIndex++;

    write(formatedMsg);
}

void MiniRPC::Send(const std::string& message)
{
    write(message);
}
