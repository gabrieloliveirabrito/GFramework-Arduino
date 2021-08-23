#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\network\\TCPServer.hpp"
#pragma once
#include <Ethernet.h>
#include "../collections/includes.h"
#include "../bases/includes.h"
#include "TCPClient.hpp"

template <typename T>
class TCPServer
{
public:
    EventHandler<T, void> *OnServerStarted, *OnServerStopped;
    EventHandler<T, void, TCPClient<T>> *OnClientConnected;
    EventHandler<T, void, TCPClient<T>> *OnClientDisconnected;

    TCPServer(int port)
    {
        this->port = port;
        this->clients = new List<TCPClient<T> *>();
    }

    void Start()
    {
        if (!running)
        {
            server = new EthernetServer(port);
            server->begin();

            if (OnServerStarted != NULL)
                OnServerStarted->Call();
            running = true;
        }
    }

    void Update()
    {
        if (running)
        {
            EthernetClient client = server->available();
            if (client)
            {
                TCPClient<T> *tcpClient = new TCPClient<T>(client);
                clients->Add(tcpClient);
            }
        }
    }

private:
    List<TCPClient<T> *> *clients;
    EthernetServer *server;
    int port;
    bool running = false;
};