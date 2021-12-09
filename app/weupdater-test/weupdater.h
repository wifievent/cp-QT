#pragma once
#include "stdafx.h"
#include "tcp.h"
#include "tcpclient.h"
#include "httprequest.h"
#include "httpresponse.h"

class WEUpdater : public TcpClient
{
    HTTPRequest versionrequest_;
    HTTPResponse versionresponse_;
    GIp host_;
    QString path_;

public:
    WEUpdater();

protected:
    void getIpAddress(QString versionurl);
    void setHttpRequest();

public:
    bool checkVersion(QString versionurl, string currentversion);
};
