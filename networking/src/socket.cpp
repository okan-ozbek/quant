//
// Created by Dorza on 21/10/2026.
//

#include "../include/socket.h"

#include <stdexcept>

Socket::Socket(const Protocol& protocol)
    : socket_{ INVALID_SOCKET }
    , observa_{ Observa::NetworkRole::SOCKET }
{
    CreateWindowsSocketAPI();
    CreateSocket(protocol);
}


Socket::~Socket()
{
    observa_.LogInfo("Destroying socket...");

    Cleanup();

    observa_.LogInfo("Socket destroyed successfully!");
}

const SOCKET& Socket::Get() const
{
    return socket_;
}

bool Socket::IsInvalid() const
{
    return socket_ == INVALID_SOCKET;
}

void Socket::CreateWindowsSocketAPI()
{
    WSADATA wsaData;
    const auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        throw std::runtime_error(observa_.ErrorMessage("WSAStartup failed with error: " + std::to_string(result)));
    }
}

void Socket::CreateSocket(const Protocol& protocol)
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
            throw std::invalid_argument(observa_.ErrorMessage("Invalid protocol specified for socket creation"));
    }

    if (socket_ == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error(observa_.ErrorMessage("Socket creation failed"));
    }

    // u_long mode = SOCKET_NON_BLOCKING_MODE;
    // const auto result = ioctlsocket(socket_, FIONBIO, &mode);
    //
    // if (result != NO_ERROR) {
    //     Cleanup();
    //     throw std::runtime_error("Socket: Failed to set non-blocking mode");
    // }

    observa_.LogInfo("Socket created successfully!");
}

void Socket::Cleanup() const
{
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
    }

    WSACleanup();
}