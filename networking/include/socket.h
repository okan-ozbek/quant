//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_SOCKET_H
#define QUANT_SOCKET_H

#include <cstdint>
#include <WinSock2.h>

#include "observa.h"

#define SOCKET_BLOCKING_MODE 0
#define SOCKET_NON_BLOCKING_MODE 1

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

    explicit Socket(const Protocol&);
    ~Socket();

    [[nodiscard]] const SOCKET& Get() const;
    [[nodiscard]] bool IsInvalid() const;

private:
    SOCKET socket_;
    Observa observa_{ Observa::NetworkRole::SERVER };

    void CreateWindowsSocketAPI();
    void CreateSocket(const Protocol&);
    void Cleanup() const;
};

#endif //QUANT_SOCKET_H
