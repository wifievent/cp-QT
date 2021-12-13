#include "werawclient.h"
#include <iostream>

WERawClient::WERawClient() {
    sock_ = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock_ == -1)
	{
		perror("Failed to create socket");
		exit(1);
    }

    int one = 1;
    const int *val = &one;

    if (setsockopt(sock_, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }
}

void WERawClient::setreqHeader(GPacket* packet, GIp dip) {
    qDebug() << "This packet is from here:" << QString(packet->ipHdr_->sip());
    packet->ipHdr_->dip_ = htonl(dip);
    packet->ipHdr_->sum_ = htons(GIpHdr::calcChecksum(packet->ipHdr_));

    //Now the TCP checksum
    packet->tcpHdr_->sum_ = htons(GTcpHdr::calcChecksum(packet->ipHdr_, packet->tcpHdr_));
}

//==================================================

void WERawClient::setrespHeader(GPacket *packet, GIp webip)
{
    qDebug() << "This packet is from here:" << QString(packet->ipHdr_->sip());
    packet->ipHdr_->sip_ = htonl(webip);
    packet->ipHdr_->sum_ = htons(GIpHdr::calcChecksum(packet->ipHdr_));

    //Now the TCP checksum
    packet->tcpHdr_->sum_ = htons(GTcpHdr::calcChecksum(packet->ipHdr_, packet->tcpHdr_));
}

void WERawClient::send(GPacket* packet) {
    memset(&sin_, 0, sizeof(sin_));
    sin_.sin_family = AF_INET;
    sin_.sin_addr.s_addr = packet->ipHdr_->dip_;

    qDebug() << "Sending data to tls server data";
    if (sendto(sock_, (char*)packet->ipHdr_, packet->ipHdr_->len(), 0, (struct sockaddr *)&sin_, sizeof(sin_)) < 0) {
        qDebug() << "sendto failed";
    }
    else {
        qDebug() << "Packet Send. Length : " << packet->ipHdr_->len();
    }
}
