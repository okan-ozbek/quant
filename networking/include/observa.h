//
// Created by Dorza on 21/10/2026.
//

#ifndef QUANT_OBSERVA_H
#define QUANT_OBSERVA_H

#include <string>

class Observa
{
public:
    enum class NetworkRole
    {
        SERVER,
        CLIENT,
        SYSTEM,
        SOCKET,
    };

    explicit Observa(NetworkRole);

    void LogInfo(const std::string_view&) const;
    void LogError(const std::string_view&) const;
    [[nodiscard]] std::string ErrorMessage(const std::string_view&) const;



private:
    NetworkRole networkRole_;

    [[nodiscard]] std::string GetNetworkRoleString() const;
};

#endif //QUANT_OBSERVA_H
