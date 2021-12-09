#pragma once

#include "stdafx.h"

using namespace std;

class CaptivePortal : public GStateObj
{
    Q_OBJECT

    GAutoArpSpoof capturer_;
    GTcpBlock tcpblock_;
    GPcapDeviceWrite writer_;

    QString redirectpage_;
    GIp host_;

public:
    QString intfname_{"eth0"};

public:
    CaptivePortal(QString redirectpage, QString intfname);
    ~CaptivePortal() override {close();};

private:
    void setComponent();
    string getDomainAtUrl(string url);

protected:
    bool doOpen() override;
    bool doClose() override;

public slots:
    void processPacket(GPacket* packet);
};
