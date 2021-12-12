#include "captiveportal.h"

CaptivePortal::CaptivePortal()
{
	capturer_.hostDetect_.checkDhcp_ = true;
	capturer_.hostDetect_.checkArp_ = true;
	capturer_.hostDetect_.checkIp_ = true;

    capturer_.internalFilter_ = "ether host 2C:8D:B1:E9:43:7D or ether host 64:EE:B7:93:E7:D0";

    tcpblock_.backwardBlockType_ = GTcpBlock::Fin;

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
    qDebug() << "My Ip Address:" << QString(myIp_);
    return;
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

    getIPAddress();

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

	if (tcpHdr->dport() == 443)
    {
        packet->ctrl.block_ = true;
        if (ipHdr->dip() != host_) {
            qDebug() << "There is tls connection request";
            socket_.setSocketOpt(myIp_);
            socket_.setHeader(packet);
            socket_.send(packet);
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
            }
            else if(it == tcpdata.end())
            {
                qDebug() << "infection keep";
                return;
            }
        }
    }
}
