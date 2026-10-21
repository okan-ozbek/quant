//
// Created by Dorza on 19/10/2026.
//

#ifndef QUANT_BASE_H
#define QUANT_BASE_H

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <cstddef>
#include <winsock2.h>

#include "observa.h"
#include "../include/socket.h"

#define MAX_PAYLOAD_SIZE (1024 * 1024) // 1 MB
#define GRACEFUL_DISCONNECT 0

class TCPBase
{
    public:
    using AddressInfo = sockaddr_in;
    using Address = std::string;
    using Port = std::uint16_t;
    using ByteCount = int;
    using Bytes = std::vector<std::byte>;

    TCPBase() = delete;
    TCPBase(const TCPBase&) = delete;
    TCPBase(TCPBase&&) = delete;
    TCPBase& operator=(const TCPBase&) = delete;
    TCPBase& operator=(TCPBase&&) = delete;

    TCPBase(
        const Socket::Protocol&,
        const Observa::NetworkRole&,
        const Address&,
        const Port&
    );

    void Send(const SOCKET&, const Bytes&) const;
    [[nodiscard]] Bytes Receive(const SOCKET&) const;
    int FillBuffer(const SOCKET&, char*, const ByteCount&) const;

    [[nodiscard]] const sockaddr* GetAddressInfo() const;
    [[nodiscard]] const SOCKET& GetSocket() const;
    [[nodiscard]] const Observa& GetObserva() const;

private:
    Socket socket_;
    AddressInfo addressInfo_;
    Observa observa_;
};

class UDPBase
{

};

#endif //QUANT_BASE_H
