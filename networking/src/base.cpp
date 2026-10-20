//
// Created by Dorza on 19/10/2026.
//

#include "../include/base.h"

TCPBase::TCPBase(const Socket::Protocol& protocol, const Observa::NetworkRole& networkRole, const Address& addressInfo, const Port& port)
    : socket_{ protocol }
    , addressInfo_{
        .sin_family = AF_INET,
        .sin_port = htons(port),
    }
    , observa_{ networkRole }
{
    const auto result = InetPton(AF_INET, addressInfo.c_str(), &addressInfo_.sin_addr);

    if (result <= 0) {
        throw std::runtime_error(GetObserva().ErrorMessage("Invalid address"));
    }

    if (socket_.IsInvalid()) {
        throw std::runtime_error(GetObserva().ErrorMessage("Invalid socket"));
    }
}

TCPBase::ByteCount TCPBase::Send(const SOCKET& socket, const Bytes& bytes) const
{
    if (socket == INVALID_SOCKET) {
        throw std::runtime_error(GetObserva().ErrorMessage("Invalid socket"));
    }

    const auto bufferSize{ static_cast<int>(bytes.size() * sizeof(std::byte)) };
    auto buffer{ reinterpret_cast<const char*>(bytes.data()) };

    auto remainingBytes = bufferSize;
    while (remainingBytes > 0) {
        const auto sentBytes{ send(socket, buffer, remainingBytes, 0) };

        if (sentBytes == SOCKET_ERROR) {
            throw std::runtime_error(GetObserva().ErrorMessage("Unable to send to peer"));
        }

        remainingBytes -= sentBytes;
        buffer += sentBytes;

        GetObserva().LogInfo("Sending " + std::to_string(sentBytes) + " bytes...");
    }

    return bufferSize;
}

TCPBase::Bytes TCPBase::Receive(const SOCKET& socket) const
{
    constexpr int headerSize{ 4 };
    Bytes bytes(headerSize);
    int receivedBytes{ 0 };

    receivedBytes = FillBuffer(socket, reinterpret_cast<char*>(bytes.data()), headerSize);

    if (receivedBytes == GRACEFUL_DISCONNECT) {
        return {};
    }

    std::uint32_t messageLength{ 0 };
    for (std::size_t i = 0; i < sizeof(messageLength); ++i) {
        messageLength |= static_cast<std::uint32_t>(bytes[i]) << (i * 8);
    }

    if (messageLength == 0) {
        return bytes;
    }

    if (messageLength > MAX_PAYLOAD_SIZE) {
        GetObserva().LogError("Message length exceeds maximum allowed size (1 MB), disconnecting client");
        return {};
    }

    bytes.resize(headerSize + messageLength);
    receivedBytes = FillBuffer(socket, reinterpret_cast<char*>(bytes.data() + headerSize), messageLength);

    if (receivedBytes == GRACEFUL_DISCONNECT) {
        return {};
    }

    return bytes;
}

int TCPBase::FillBuffer(const SOCKET& socket, char* buffer, const ByteCount& size) const
{
    int received = 0;

    while (received < size) {
        const int result = recv(socket, buffer + received, size - received, 0);

        if (result == GRACEFUL_DISCONNECT) {
            return GRACEFUL_DISCONNECT;;
        }

        if (result == SOCKET_ERROR) {
            GetObserva().LogError("Client disconnected prematurely: " + std::to_string(WSAGetLastError()));
            return GRACEFUL_DISCONNECT;
        }

        received += result;
    }

    return received;
}

const sockaddr* TCPBase::GetAddressInfo() const
{
    return reinterpret_cast<const sockaddr*>(&addressInfo_);
}

const SOCKET& TCPBase::GetSocket() const
{
    return socket_.Get();
}

const Observa& TCPBase::GetObserva() const
{
    return observa_;
}