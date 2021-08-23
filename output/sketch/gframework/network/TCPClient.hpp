#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\network\\TCPClient.hpp"
#pragma once
#include <Ethernet.h>

template <typename T>
class TCPClient
{
private:
    EthernetClient client;
    IPAddress ip;
    int port;

public:
    TCPClient(IPAddress ip, int port)
    {
        this->ip = ip;
        this->port = port;
    }

    TCPClient(EthernetClient client)
    {
        this->client = client;
        this->ip = client.remoteIP();
        this->port = client.remotePort();
    }
};