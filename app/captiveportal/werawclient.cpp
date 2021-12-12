#include "werawclient.h"
#include <iostream>

WERawClient::WERawClient() {
    sock_ = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock_ == -1)
	{
		perror("Failed to create socket");
		exit(1);
    }
}

void WERawClient::setSocketOpt(GIp myip) {
    QString tlsip = QString(myip);

    memset(&sin_, 0, sizeof(sin_));
    sin_.sin_family = AF_INET;
    sin_.sin_addr.s_addr = inet_addr(qPrintable(tlsip));

    int one = 1;
    const int *val = &one;

    if (setsockopt(sock_, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }
}

void WERawClient::setHeader(GPacket* packet) {
    qDebug() << "This packet is from here:" << QString(packet->ipHdr_->sip());
    packet->ipHdr_->dip_ = sin_.sin_addr.s_addr;
    packet->ipHdr_->sum_ = htons(GIpHdr::calcChecksum(packet->ipHdr_));

    //Now the TCP checksum
    packet->tcpHdr_->sum_ = htons(GTcpHdr::calcChecksum(packet->ipHdr_, packet->tcpHdr_));
}

void WERawClient::send(GPacket* packet) {
    qDebug() << "Sending data to tls server data";
    show_binary((char*)packet->buf_.data_, packet->ipHdr_->len());
    if (sendto(sock_, (char*)packet->ipHdr_, packet->ipHdr_->len(), 0, (struct sockaddr *)&sin_, sizeof(sin_)) < 0) {
        qDebug() << "sendto failed";
    }
    else {
        qDebug() << "Packet Send. Length : " << packet->ipHdr_->len();
    }
}

void WERawClient::show_binary(char* pAddr, int size)
{
    qDebug() << "sizeof print:" << size;
    char dwBuffer[6];
    uint8_t dwValue;

    for (int i = 1; i <= size; i++) {
        dwValue = pAddr[i-1];
        sprintf(dwBuffer, "%02X ", dwValue);
        std::cout << dwBuffer;
        if(i % 16 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}
