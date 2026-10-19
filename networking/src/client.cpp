//
// Created by Dorza on 19/10/2026.
//

#include "../include/client.h"
#include "../include/base.h"
#include "../include/socket.h"

TCPClient::TCPClient(const Address& address, const Port& port)
    : TCPBase(Socket::Protocol::TCP, Observa::NetworkRole::CLIENT, address, port)
{
    Connect();
}

void TCPClient::Connect() const
{
    const auto result{ connect(GetSocket(), GetAddressInfo(), sizeof(AddressInfo)) };

    if (result == SOCKET_ERROR) {
        throw std::runtime_error(GetObserva().ErrorMessage("Connect failed"));
    }
}

