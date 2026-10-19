//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_SOCKET_H
#define QUANT_SOCKET_H

#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

class Socket
{
public:
    enum class Protocol : std::uint8_t
    {
        TCP,
        UDP
    };

    Socket() = delete;
    Socket(const Socket&) = delete;
    Socket(Socket&&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket& operator=(Socket&&) = delete;

    explicit Socket(const Protocol& protocol) : socket_(INVALID_SOCKET)
    {
        CreateWindowsSocketAPI();
        CreateSocket(protocol);
    }

    ~Socket()
    {
        std::cout << "Destroying socket..." << "\n";

        Cleanup();

        std::cout << "Socket destroyed" << "\n";
    }

    [[nodiscard]]
    const SOCKET& Get() const
    {
        return socket_;
    }

    [[nodiscard]]
    bool IsInvalid() const
    {
        return socket_ == INVALID_SOCKET;
    }

private:
    SOCKET socket_;

    void CreateWindowsSocketAPI()
    {
        WSADATA wsaData;
        const auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw std::runtime_error("Socket: WSAStartup failed");
        }
    }

    void CreateSocket(const Protocol& protocol)
    {
        switch (protocol) {
            case Protocol::UDP:
                socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                break;
            case Protocol::TCP:
                socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                break;
            default:
                WSACleanup();
                throw std::invalid_argument("Socket: Invalid protocol");
        }

        if (socket_ == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Socket: Socket creation failed");
        }

        std::cout << "Socket created successfully" << "\n";
    }

    void Cleanup() const
    {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
        }

        WSACleanup();
    }
};

#endif //QUANT_SOCKET_H
