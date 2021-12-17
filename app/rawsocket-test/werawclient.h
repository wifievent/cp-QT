#pragma once

#include <QCoreApplication>

#include <GAutoArpSpoof>
#include <GTcpBlock>
#include <GPcapDeviceWrite>
#include <GApp>
#include <GJson>

#include <QObject>
#include <QString>
#include <QThread>
#include <QJsonObject>
#include <QUrl>

#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <vector>

#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

struct PseudoHeader
{
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t placeholder;
	u_int8_t protocol;
	u_int16_t tcp_length;
};

class WERawClient
{
    int sock_;
    char datagram_[4096], *data_, *pseudogram_;
    QString sip_;
    struct iphdr *iph_;
	
	struct tcphdr *tcph_;
	struct sockaddr_in sin_;
	struct PseudoHeader psh_;

public:
    WERawClient();
    ~WERawClient() {}

private:
    unsigned short checkSum(unsigned short* ptr, int nbytes);

public:
    void setSocketOpt(std::string ip, int port);
    void setIpHeader(GIpHdr iphdr);
    void setTcpHeader(GTcpHdr tcphdr);
    void send();
};

/*
setSocketOpt();
setIpHeader();
setTcpHeader();
*/
