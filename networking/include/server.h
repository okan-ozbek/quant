//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_SERVER_H
#define QUANT_SERVER_H

#include <cstring>
#include <vector>

#include "base.h"
#include "data.h"
#include "socket.h"

class TCPServer : public TCPBase
{
public:
    TCPServer(const Address& address, const Port& port);
    ~TCPServer();

    void Bind() const;
    void Listen() const;
    void Accept();

    void Close() const;

private:
    std::vector<SOCKET> clientSockets_{};

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
