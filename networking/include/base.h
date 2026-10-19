//
// Created by Dorza on 19/10/2026.
//

#ifndef QUANT_BASE_H
#define QUANT_BASE_H

#include <vector>

#include "../include/socket.h"

#define MAX_PAYLOAD_SIZE 1024 * 1024 // 1 MB

class Observa
{
public:
    enum class NetworkRole
    {
        SERVER,
        CLIENT
    };

    explicit Observa(const NetworkRole role) : networkRole_{ role } {}

    void LogInfo(const std::string& message) const
    {
        std::cout << "[" << GetNetworkRoleString() << " INFO]: " << message << std::endl;
    }

    void LogError(const std::string& message) const
    {
        std::cerr << "[" << GetNetworkRoleString() << " ERROR]: " << message << std::endl;
    }

    std::string ErrorMessage(const std::string& message) const
    {
        return "[" + GetNetworkRoleString() + " ERROR]: " + message;
    }

private:
    NetworkRole networkRole_;

    std::string GetNetworkRoleString() const
    {
        switch (networkRole_)
        {
            case NetworkRole::SERVER:
                return "SERVER";
            case NetworkRole::CLIENT:
                return "CLIENT";
            default:
                return "";
        }
    }
};

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
        const Socket::Protocol& protocol,
        const Observa::NetworkRole& networkRole,
        const Address& addressInfo,
        const Port& port
    );

    ByteCount Send(const SOCKET& socket, const Bytes& bytes) const;
    Bytes Receive(const SOCKET& socket) const;
    void FillBuffer(const SOCKET& socket, char* buffer, const ByteCount& size) const;

    const sockaddr* GetAddressInfo() const;
    const SOCKET& GetSocket() const;
    const Observa& GetObserva() const;

private:
    Socket socket_;
    AddressInfo addressInfo_;
    Observa observa_;
};

class UDPBase
{

};

#endif //QUANT_BASE_H
