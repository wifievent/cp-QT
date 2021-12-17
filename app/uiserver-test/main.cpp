#include <QCoreApplication>
#include "uiserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UIServer uiserver;
    uiserver.rootdir_ = "../../bin/webui";
    uiserver.start(80);

    a.exec();

    uiserver.stop();

    return 0;
}
