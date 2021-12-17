#include "werawclient.h"

WERawClient::WERawClient() {
    sock_ = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock_ == -1)
	{
		perror("Failed to create socket");
		exit(1);
	}
    memset(datagram_, 0, 4096);

	iph_ = (struct iphdr *)datagram_;
	tcph_ = (struct tcphdr *)(datagram_ + sizeof(struct ip));
	data_ = datagram_ + sizeof(struct iphdr) + sizeof(struct tcphdr);
}

unsigned short WERawClient::checkSum(unsigned short* ptr, int nbytes) {
    long sum = 0;
	unsigned short oddbyte;
    short answer;

	while( nbytes > 1 ) {
		sum += *ptr++;
		nbytes -= 2;
	}
	if(nbytes==1) {
		oddbyte = 0;
		*((u_char*)&oddbyte) = *(u_char*)ptr;
		sum += oddbyte;
	}

	sum = (sum>>16) + (sum & 0xffff);
	sum = sum + (sum>>16);
	answer = (short)~sum;
	
	return answer;
}

void WERawClient::setSocketOpt(std::string ip, int port) {
    memset(&sin_, 0, sizeof(sin_));
    sin_.sin_family = AF_INET;
    sin_.sin_port = htons(port);
    sin_.sin_addr.s_addr = inet_addr(ip.c_str());

    int one = 1;
    const int *val = &one;

    if (setsockopt(sock_, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }
}

//destination, ttl
void WERawClient::setIpHeader(GIpHdr iphdr) {
    sip_ = iphdr.sip();

    iph_->ihl = iphdr.hl();
    iph_->version = iphdr.v();
    iph_->tos = iphdr.tos();
    iph_->tot_len = iphdr.len();
    iph_->id = iphdr.id();
    iph_->frag_off = iphdr.off();
    iph_->ttl = iphdr.ttl();
    iph_->protocol = iphdr.p();

    iph_->check = 0;
    iph_->saddr = inet_addr( qPrintable(sip_) );
    iph_->daddr = sin_.sin_addr.s_addr;

    iph_->check = checkSum((unsigned short *)datagram_, iph_->tot_len);
}

void WERawClient::setTcpHeader(GTcpHdr tcphdr) {
    //TCP Header
    tcph_->source = tcphdr.sport_;
    tcph_->dest = tcphdr.dport_;
    tcph_->seq = tcphdr.seq();
    tcph_->ack_seq = tcphdr.ack();
    tcph_->doff = tcphdr.off();	//tcp header size
    tcph_->fin = tcphdr.flags_;
    tcph_->syn = tcphdr.flags_ >> 1;
    tcph_->rst = tcphdr.flags_ >> 2;
    tcph_->psh = tcphdr.flags_ >> 3;
    tcph_->ack = tcphdr.flags_ >> 4;
    tcph_->urg = tcphdr.flags_ >> 5;
    tcph_->window = tcphdr.win_;	/* maximum allowed window size */
    tcph_->check = 0;	//leave checksum 0 now, filled later by pseudo header
    tcph_->urg_ptr = tcphdr.urp_;

    //Now the TCP checksum
    psh_.source_address = inet_addr( qPrintable(sip_) );
    psh_.dest_address = sin_.sin_addr.s_addr;
    psh_.placeholder = 0;
    psh_.protocol = IPPROTO_TCP;
    psh_.tcp_length = htons(sizeof(struct tcphdr) + strlen(data_));

    int psize = sizeof(struct PseudoHeader) + sizeof(struct tcphdr) + strlen(data_);
    pseudogram_ = (char*)malloc(psize);

    memcpy(pseudogram_, (char*)&psh_, sizeof(struct PseudoHeader));
    memcpy(pseudogram_ + sizeof(struct PseudoHeader), tcph_, sizeof(struct tcphdr) + strlen(data_));

    tcph_->check = checkSum((unsigned short*)pseudogram_, psize);
}

void WERawClient::send() {
    if (sendto(sock_, datagram_, iph_->tot_len , 0, (struct sockaddr *)&sin_, sizeof(sin_)) < 0) {
        qDebug() << "sendto failed";
    }
    else {
        qDebug() << "Packet Send. Length : " << iph_->tot_len;
    }
}
