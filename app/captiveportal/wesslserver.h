#pragma once
#include "sslserver.h"
#include "stdafx.h"

class WESslServer : public SslServer
{
public:
    std::string redirectpage_;

public:
    WESslServer();
    ~WESslServer() {}

protected:
    void handleClnt(SslClientSocket* clntsock) override;
};
