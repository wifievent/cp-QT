#include "weudpclient.h"
#include "weudpserver.h"
#include "wesslserver.h"
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

    WEUdpServer wus;
    wus.start(7284);
    WESslServer wss;
    wss.redirectpage_ = "http://test.gilgil.net";
    wss.start(443, "../../bin/certkey-test/server.crt", "../../bin/certkey-test/server.key");

    GRtmEntry* entry = GNetInfo::instance().rtm().getBestEntry(QString("8.8.8.8"));

    CaptivePortal cp;
    cp.intfname_ = entry->intf()->name();
    cp.gwIp_ = entry->intf()->gateway();
    cp.redirectpage_ = "http://test.gilgil.net";

    QJsonObject jo = GJson::loadFromFile();
    if (!jo.isEmpty())
        jo["cp"] >> cp;
    jo["cp"] << cp;
    GJson::saveToFile(jo);

    GThreadMgr::suspendStart();
    cp.open();
    GThreadMgr::resumeStart();

    a.exec();

    wss.stop();
    wus.stop();
}
