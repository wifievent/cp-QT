#pragma once

#include "stdafx.h"

class WERawClient
{
    int sock_;
    struct sockaddr_in sin_;
    GBuf buf_;

public:
    WERawClient();
    ~WERawClient() {}

public:
    void setSocketOpt(GIp myip);
    void setHeader(GPacket* packet);
    void send(GPacket* packet);
    void show_binary(char* pAddr, int size);
};
