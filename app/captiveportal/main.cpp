#include "weudpclient.h"
#include "weudpserver.h"
#include "captiveportal.h"

const char* version()
{
    return
#include "../../version.txt"
#ifdef _DEBUG
            " Debug Build(" __DATE__ " " __TIME__ ")";
#else
            " Release Build(" __DATE__ " " __TIME__ ")";
#endif
}

int main(int argc, char *argv[])
{
    GApp a(argc, argv);
    WEUdpClient client;
    if(client.searchProduct(7284, 1, 0, "run already?"))
    {
        qDebug() << "Captive Portal stop thanks to using";
        return 0;
    }
    else
    {
        qInfo() << "CaptivePortal Started" << version();
    }

    WEUdpServer ws;
    ws.start(7284);

    GRtmEntry* entry = GNetInfo::instance().rtm().getBestEntry(QString("8.8.8.8"));

    CaptivePortal cp;
    cp.intfname_ = entry->intf()->name();
    cp.gwIp_ = entry->intf()->gateway();
    cp.redirectpage_ = "http://test.gilgil.net";
    cp.open();

    a.exec();

    ws.stop();
}
