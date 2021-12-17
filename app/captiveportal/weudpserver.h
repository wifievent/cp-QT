#ifndef WEUDPSERVER_H
#define WEUDPSERVER_H

#include "udpserver.h"
#include "stdafx.h"

class WEUdpServer : public UdpServer
{
public:
    WEUdpServer() {}
    ~WEUdpServer() {}

    bool check = true;

    std::thread* t1;

    void start(int port);
    void stop();

    void handleClnt();
};

#endif // WEUDPSERVER_H
