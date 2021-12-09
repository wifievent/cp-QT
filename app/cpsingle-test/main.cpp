#include "stdafx.h"
#include "captiveportal.h"

int main(int argc, char *argv[])
{
    GApp a(argc, argv);

    CaptivePortal* cp = new CaptivePortal("http://test.gilgil.net/ek.jpg", "eth0");

    cp->open();

    return a.exec();
}
