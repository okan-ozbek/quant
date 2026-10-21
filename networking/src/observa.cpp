//
// Created by Dorza on 21/10/2026.
//

#include "../include/observa.h"

#include <iostream>

Observa::Observa(const NetworkRole role)
    : networkRole_{ role }
{

}

void Observa::LogInfo(const std::string& message) const
{
    std::cout << "[" << GetNetworkRoleString() << " INFO]: " << message << "\n";
}

void Observa::LogError(const std::string& message) const
{
    std::cerr << "[" << GetNetworkRoleString() << " ERROR]: " << message << "\n";
}

[[nodiscard]] std::string Observa::ErrorMessage(const std::string& message) const
{
    return "[" + GetNetworkRoleString() + " ERROR]: " + message;
}

[[nodiscard]] std::string Observa::GetNetworkRoleString() const
{
    switch (networkRole_)
    {
        case NetworkRole::SERVER:
            return "SERVER";
        case NetworkRole::CLIENT:
            return "CLIENT";
        case NetworkRole::SYSTEM:
            return "SYSTEM";
        case NetworkRole::SOCKET:
            return "SOCKET";
        default:
            return "";
    }
}