#include "socket.h"

class TcpSocket : public Socket
{
public:
    int sock_{0};
    struct sockaddr_in sockaddr_;

public:
    TcpSocket();
    ~TcpSocket() override;
    int send(char* buf, size_t len) override;
    int recv(char* buf, size_t len) override;
    int disconnect() override;
};
