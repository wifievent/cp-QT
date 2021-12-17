#include "udp.h"
#include "udpclient.h"

int main()
{
    char sendbuf[BUFSIZE] = "run already?";
    char recvbuf[BUFSIZE];
    UdpClient* client = new UdpClient();
    client->setSocketBroadcastable();
    client->setRecvTimeout(5, 0);
    client->setSockAddrBroadcast(7284);
    client->send(sendbuf, strlen(sendbuf) + 1);
    if(client->recv(recvbuf, BUFSIZE) != -1)
    {
        qDebug() << recvbuf;
    }
    else
    {
        qDebug() << "Can't get data";
    }
    return 0;
}
