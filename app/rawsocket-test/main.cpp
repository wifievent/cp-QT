#include "werawclient.h"

int main()
{
    WERawClient wec;

    wec.setSocketOpt("192.168.1.16", 443);
    wec.setIpHeader(*ipHdr);
    wec.setTcpHeader(*tcpHdr);
    wec.send();
}
