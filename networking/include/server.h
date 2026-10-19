//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_SERVER_H
#define QUANT_SERVER_H

#include <vector>

#include "base.h"
#include "socket.h"

class TCPServer : public TCPBase
{
public:
    TCPServer(const Address& address, const Port& port);
    ~TCPServer();

    [[noreturn]]
    void Poll();

private:
    std::vector<SOCKET> clientSockets_{};

    void Bind() const;
    void Listen() const;
    void Accept();
    void Run() const;
    void Close() const;

    [[nodiscard]]
    bool HasConnectedClients() const
    {
        return !clientSockets_.empty();
    }
};

// TODO: Implement UDPServer class
class UDPServer
{
public:

private:
    Socket socket_{ Socket::Protocol::UDP };
};

#endif //QUANT_SERVER_H
