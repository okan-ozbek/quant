//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_CLIENT_H
#define QUANT_CLIENT_H

#include "base.h"

class TCPClient : public TCPBase
{
public:
    TCPClient(const Address& address, const Port& port);

    void Connect() const;
};

class UDPClient
{

};

#endif //QUANT_CLIENT_H
