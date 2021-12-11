#pragma once

#include "stdafx.h"

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
    void setSocketOpt(GIp myip, int port);
    void setIpHeader(GIpHdr iphdr);
    void setTcpHeader(GTcpHdr tcphdr);
    void send();
};

/*
setSocketOpt();
setIpHeader();
setTcpHeader();
*/
