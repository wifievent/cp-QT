#pragma once
#include "tcpserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "captiveportal.h"

#include <fstream>
#include <ctime>
#include <cstdio>

class WEUIServer : public TcpServer
{
    HTTPRequest uirequest_;
    HTTPResponse uiresponse_;
    char ui_[BUFSIZE];

    CaptivePortal cp_;

public:
    std::string rootdir_;

public:
    WEUIServer();
    ~WEUIServer() {}

protected:
    void handleClnt(TcpClientSocket* clntsock) override;
    void setHttpResponse();
    int getWebUIData(std::string path);
    std::string getDateTime();
    bool denyDotDotPacket(std::string path);
};
