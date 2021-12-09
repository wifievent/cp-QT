#include "tcpclient.h"

using namespace std;

TcpClient::TcpClient()
{
}

TcpClient::~TcpClient()
{
}

int TcpClient::connect(string ip, int port) //connect
{
    memset(&sockaddr_, 0, sizeof(sockaddr_));
    sockaddr_.sin_family=AF_INET;
    sockaddr_.sin_addr.s_addr=inet_addr(ip.c_str());
    sockaddr_.sin_port=htons(port);
    return ::connect(
                sock_,
                (struct sockaddr*)&(sockaddr_),
                sizeof(sockaddr_)
                );
    //success 0, fail -1
}
