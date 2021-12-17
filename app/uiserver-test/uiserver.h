#pragma once
#include "tcpserver.h"
#include "httprequest.h"
#include "httpresponse.h"

#include <fstream>
#include <ctime>
#include <cstdio>
#include <string>

class UIServer : public TcpServer
{
    HTTPRequest uirequest_;
    HTTPResponse uiresponse_;
    char ui_[BUFSIZE];

public:
    std::string rootdir_;

public:
    UIServer();
    ~UIServer() {}

protected:
    void handleClnt(TcpClientSocket* clntsock) override;
    void setHttpResponse();
    int getWebUIData(std::string path);
    std::string getDateTime();
    bool denyDotDotPacket(std::string path);
};
