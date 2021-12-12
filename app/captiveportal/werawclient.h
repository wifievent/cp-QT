#pragma once

#include "stdafx.h"

class WERawClient
{
    int sock_;
    struct sockaddr_in sin_;

public:
    WERawClient();
    ~WERawClient() {}

public:
    void setSocketOpt(GIp myip);
    void setreqHeader(GPacket* packet);
    void setrespHeader(GPacket* packet, GIp webip);
    void send(GPacket* packet);
    void show_binary(char* pAddr, int size);
};
