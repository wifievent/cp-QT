#include "captiveportal.h"

CaptivePortal::CaptivePortal()
{
    GRtmEntry* entry = GNetInfo::instance().rtm().getBestEntry(QString("8.8.8.8"));
    intfname_ = entry->intf()->name();
    gwIp_ = entry->intf()->gateway();

    getIPAddress();

	capturer_.hostDetect_.checkDhcp_ = true;
	capturer_.hostDetect_.checkArp_ = true;
    capturer_.hostDetect_.checkIp_ = true;

    tcpblock_.backwardBlockType_ = GTcpBlock::Fin;

    GCommandItem opencommand;
    filter_.command_.openCommands_.clear();
    filter_.command_.openCommands_.push_back(new GCommandItem(this, QStringList{"su -c \"iptables -A OUTPUT -p tcp -s "
                                                                                 + QString(myIp_) + " --sport 443 -j NFQUEUE --queue-num 0\""}
    ));

    GCommandItem closecommand;
    filter_.command_.closeCommands_.clear();
    filter_.command_.closeCommands_.push_back(new GCommandItem(this, QStringList{"su -c \"iptables -D OUTPUT -p tcp -s "
                                                                                 + QString(myIp_) + " --sport 443 -j NFQUEUE --queue-num 0\""}
    ));

    QObject::connect(
                &filter_,
                SIGNAL(captured(GPacket*)),
                this,
                SLOT(getSendPacket(GPacket*)),
                Qt::DirectConnection
                );

	QObject::connect(
				&capturer_,
				SIGNAL(captured(GPacket*)),
				this,
				SLOT(processPacket(GPacket*)),
				Qt::DirectConnection
                );

	tcpblock_.writer_ = &writer_;
}

void CaptivePortal::setComponent()
{
    capturer_.intfName_ = intfname_;
    tcpblock_.backwardFinMsg_ = QStringList{"HTTP/1.0 302 Redirect\r\n"
                                            "Location: "+redirectpage_+"\r\n"
                                                                       "\r\n"};
}

void CaptivePortal::getIPAddress()
{
    uint32_t ipaddr;
    int sock;
    struct ifreq ifr;
    struct sockaddr_in *sin;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        fprintf(stderr, "%s", "SOCK ERROR");
        return;
    }

    strcpy(ifr.ifr_name, qPrintable(intfname_));
    if (ioctl(sock, SIOCGIFADDR, &ifr)< 0)
    {
        fprintf(stderr, "%s", "IOCTL ERROR");
        ::close(sock);
        return;
    }
    sin = (struct sockaddr_in*)&ifr.ifr_addr;
    ipaddr = ntohl((sin->sin_addr).s_addr);
    ::close(sock);
    myIp_ = GIp(ipaddr);
    qInfo() << "My Ip Address:" << QString(myIp_);
    return;
}

void CaptivePortal::setClientDict(GIp keyip, GIp webip, uint16_t port)
{
    ClientData target = {port, webip};
    dict_.insert({keyip, std::vector<ClientData>{{port, webip}}});
    std::vector<ClientData>::iterator iter;
    for(iter = dict_[keyip].begin() ; iter!= dict_[keyip].end() ; iter++) {
        if(iter->port_ == port) {
            iter->webip_ = webip;
            return;
        }
    }
    dict_[keyip].push_back(target);
    return;
}

void CaptivePortal::delClientDict(GIp keyip)
{
    dict_[keyip].clear();
    dict_.erase(keyip);
    return;
}

GIp CaptivePortal::getClientDict(GIp keyip, uint16_t port)
{
    std::vector<ClientData>::iterator iter;
    for(iter = dict_[keyip].begin() ; iter!= dict_[keyip].end() ; iter++) {
        if(iter->port_ == port) {
            return iter->webip_;
        }
    }
    qDebug() << "There is no WEBIP";
    return GIp("8.8.8.8");
}

void CaptivePortal::showClientDict()
{

}

bool CaptivePortal::doOpen()
{
    QUrl url = redirectpage_;
    struct addrinfo *servinfo;
    struct addrinfo hints;
    char host[16];

    QString domain = url.host();

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(qPrintable(domain), NULL, &hints, &servinfo))
    {
        qDebug() << "failed to get host ip";
        exit(1);
    }

    getnameinfo(servinfo->ai_addr,
                servinfo->ai_addrlen,
                host, sizeof(host),
                NULL,
                0,
                NI_NUMERICHOST);

    QString ip = QString(host);
    host_ = GIp(ip);
    qInfo() << "domain=" << redirectpage_ << "," << "ip=" << QString(host_);

    setComponent();
    if(!(writer_.open()))
    {
        qDebug() << "failed to open writer";
        return false;
    }
    if(!(tcpblock_.open()))
    {
        qDebug() << "failed to open tcpblock";
        return false;
    }
    if(!(filter_.open()))
    {
        qDebug() << "failed to open filter";
        return false;
    }
    if(!(capturer_.open()))
    {
        qDebug() << "failed to open arpspoof";
        return false;
    }
    return true;
}

bool CaptivePortal::doClose()
{
    if(!(writer_.close()))
    {
        qDebug() << "failed to close writer";
        return false;
    }
    if(!(tcpblock_.close()))
    {
        qDebug() << "failed to close tcpblock";
        return false;
    }
    if(!(filter_.close()))
    {
        qDebug() << "failed to close filter";
        return false;
    }
    if(!(capturer_.close()))
    {
        qDebug() << "failed to close arpspoof";
        return false;
    }
    return true;
}

void CaptivePortal::propLoad(QJsonObject jo)
{
    GProp::propLoad(jo);
    jo["capturer"] >> capturer_;
    jo["tcpblock"] >> tcpblock_;
    jo["writer"] >> writer_;
}

void CaptivePortal::propSave(QJsonObject &jo)
{
    GProp::propSave(jo);
    jo["capturer"] << capturer_;
    jo["tcpblock"] << tcpblock_;
    jo["writer"] << writer_;
}

void CaptivePortal::processPacket(GPacket *packet)
{
	GEthHdr* ethHdr = packet->ethHdr_;
	if (ethHdr == nullptr) {
		qCritical() << "ethHdr is null";
		return;
	}

	if (ethHdr->type() != GEthHdr::Ip4)
        return;

	GIpHdr* ipHdr = packet->ipHdr_;
	if (ipHdr == nullptr) {
		qCritical() << "ipHdr is null";
		return;
	}

	if(ipHdr->p() != GIpHdr::Tcp)
        return;

	GTcpHdr* tcpHdr = packet->tcpHdr_;

	if (tcpHdr == nullptr) {
		qCritical() << "tcpHdr is null";
		return;
	}

    if (tcpHdr->dport() == 443 && ipHdr->dip() != myIp_)
    {
        packet->ctrl.block_ = true;
        if (ipHdr->dip() != host_) {
            qDebug() << "There is client to tls request";
            setClientDict(ipHdr->sip(), ipHdr->dip(), tcpHdr->sport());
            forspoofsocket_.setreqHeader(packet, myIp_);
            forspoofsocket_.send(packet);
        }
        return;
    }

	if (tcpHdr->dport() == 80)
    {
		GBuf tcpData = packet->tcpData_;
		if(!tcpData.valid())
			return;

		const char* castedtcpdata = reinterpret_cast<const char*>(tcpData.data_);
		if(strncmp(castedtcpdata, "GET ", 4) == 0 && ipHdr->dip() != host_)
        {
            qDebug() << "Send redirect page data to client";
            tcpblock_.block(packet);
        }
		if(strncmp(castedtcpdata, "POST ", 5) == 0)
        {
            std::string api = "infected=false";
            std::string tcpdata = castedtcpdata;
			auto it = std::search(tcpdata.begin(), tcpdata.end(), std::boyer_moore_searcher(api.begin(), api.end()));
            if (it != tcpdata.end())
            {
				qDebug() << "infection off" << QString(ipHdr->sip());
				capturer_.removeFlows(ipHdr->sip(), gwIp_, gwIp_, ipHdr->sip());
                delClientDict(ipHdr->sip());
            }
            else if(it == tcpdata.end())
            {
                qDebug() << "infection keep";
                return;
            }
        }
    }
}

void CaptivePortal::getSendPacket(GPacket *packet)
{
    qDebug() << "Capture Packet from local!";
    GIpHdr* ipHdr = packet->ipHdr_;
    GTcpHdr* tcpHdr = packet->tcpHdr_;

    if(tcpHdr->sport() == 443 && ipHdr->sip() == myIp_)
    {
        packet->ctrl.block_ = true;
        if (ipHdr->dip() != host_) {
            qDebug() << "There is tls to client response";
            GIp webip = getClientDict(ipHdr->dip(), tcpHdr->dport());
            forfiltersocket_.setrespHeader(packet, webip);
            packet->ctrl.changed_ = true;
            forfiltersocket_.send(packet);
        }
        return;
    }
}
