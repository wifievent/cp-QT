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
    void setreqHeader(GPacket* packet, GIp dip);
    void setrespHeader(GPacket* packet, GIp webip);
    void send(GPacket* packet);
    void show_binary(char* pAddr, int size);
};
