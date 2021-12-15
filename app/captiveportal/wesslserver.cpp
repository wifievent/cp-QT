#include "wesslserver.h"

WESslServer::WESslServer()
{

}

void WESslServer::handleClnt(SslClientSocket* clntsock) {
    char buffer[BUFSIZE];
    std::string message = "HTTP/1.0 302 Redirect\r\n"
                          "Location: "+redirectpage_+"\r\n"
                          "\r\n";
    int len = 0;
    while((len = clntsock->recv(buffer, BUFSIZE)) != -1) {
        if(len == 0) {
            DLOG(INFO) << "clntsock is shutdown";
            return;
        }
        DLOG(INFO) << "recv data from client";
        if(strncmp(buffer, "GET ", 4) == 0)
        {
            DLOG(INFO) << "send redirect data to client";
            clntsock->send(message.data(), message.length() + 1);
        }
    }
    return;
}
