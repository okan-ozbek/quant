//
// Created by Dorza on 19/10/2026.
//

#include "../include/server.h"

#include "../include/base.h"
#include "../include/base.h"
#include "../include/data.h"

TCPServer::TCPServer(const Address& address, const Port& port)
    : TCPBase(Socket::Protocol::TCP, Observa::NetworkRole::SERVER, address, port)
{
    Bind();
}

TCPServer::~TCPServer()
{
    Close();
}

[[noreturn]]
void TCPServer::Poll()
{
    while (true) {
        Listen();
        Accept();

        while (HasConnectedClients()) {
            Run();
        }

        GetObserva().LogInfo("No connected clients. Waiting for new connections...");
    }
}

void TCPServer::Bind() const
{
    const auto result{ bind(GetSocket(), GetAddressInfo(), sizeof(AddressInfo)) };

    if (result == SOCKET_ERROR) {
        throw std::runtime_error(GetObserva().ErrorMessage("Bind failed"));
    }
}

void TCPServer::Listen() const
{
    const auto result{ listen(GetSocket(), SOMAXCONN) };

    if (result == SOCKET_ERROR) {
        throw std::runtime_error(GetObserva().ErrorMessage("Listen failed"));
    }

    GetObserva().LogInfo("Listening for incoming connections...");
}

void TCPServer::Accept()
{
    const auto clientSocket{ accept(GetSocket(), nullptr, nullptr) };

    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error(GetObserva().ErrorMessage("Accept failed"));
    }

    try {
        GetObserva().LogInfo("Accepted a new client connection");
        clientSockets_.push_back(clientSocket);
    }
    catch (const std::exception& exception) {
        closesocket(clientSocket);
        throw std::runtime_error(GetObserva().ErrorMessage(std::string(exception.what())));
    }
}

void TCPServer::Run()
{
    for (std::size_t i{}; i < clientSockets_.size(); ++i) {
        const auto& clientSocket{ clientSockets_[i] };

        if (clientSocket == INVALID_SOCKET) {
            GetObserva().LogError("Invalid client socket");
            continue;
        }

        const auto payload = Receive(clientSocket);

        if (payload.empty()) {
            GetObserva().LogInfo("(Client " + std::to_string(i + 1) + "): disconnected");
            RemoveClientSocket(clientSocket, i);
            continue;
        }

        const auto [textLength, text] = Decode(payload);
        GetObserva().LogInfo("(Client " + std::to_string(i + 1) + "): " + text);
    }
}

void TCPServer::Close() const
{
    for (const auto& clientSocket : clientSockets_) {
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
        }
    }
}
