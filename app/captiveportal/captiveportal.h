#pragma once

#include "stdafx.h"

class CaptivePortal : public GStateObj
{
    Q_OBJECT
    Q_PROPERTY(QString intfname MEMBER intfname_)
    Q_PROPERTY(QString redirectpage MEMBER redirectpage_)

    GAutoArpSpoof capturer_;
    GTcpBlock tcpblock_;
    GPcapDeviceWrite writer_;

    GIp host_;

public:
    QString intfname_;
    QString redirectpage_;
    GIp gwIp_;

public:
    CaptivePortal();
    ~CaptivePortal() override {close();};

private:
    void setComponent();

protected:
    bool doOpen() override;
    bool doClose() override;

public:
    void propLoad(QJsonObject jo) override;
    void propSave(QJsonObject& jo) override;

public slots:
    void processPacket(GPacket* packet);
};