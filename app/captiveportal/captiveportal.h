#pragma once

#include "stdafx.h"
#include "werawclient.h"

struct ClientData
{
    uint16_t port_;
    GIp webip_;
};

class CaptivePortal : public GStateObj
{
    Q_OBJECT
    Q_PROPERTY(QString intfname MEMBER intfname_)
    Q_PROPERTY(QString redirectpage MEMBER redirectpage_)

    GAutoArpSpoof capturer_;
    GTcpBlock tcpblock_;
    GPcapDeviceWrite writer_;

    WERawClient socket_;

    GIp host_;

    std::map<GIp, std::vector<ClientData>> dict_;

public:
    QString intfname_;
    QString redirectpage_;
    GIp myIp_;
    GIp gwIp_;

public:
    CaptivePortal();
    ~CaptivePortal() override {close();};

private:
    void setComponent();
    void getIPAddress();

    void setClientDict(GIp keyip, GIp webip, uint16_t port);
    void delClientDict(GIp keyip);
    GIp getClientDict(GIp keyip, uint16_t port);

protected:
    bool doOpen() override;
    bool doClose() override;

public:
    void propLoad(QJsonObject jo) override;
    void propSave(QJsonObject& jo) override;

public slots:
    void processPacket(GPacket* packet);
};
