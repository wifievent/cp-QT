#include "weudpclient.h"
#include "weudpserver.h"
#include "weuiserver.h"

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
    WEUIServer uiserver;
    uiserver.rootdir_ = "./webui";
    uiserver.start(80);

    a.exec();
    uiserver.stop();
    wus.stop();
}
