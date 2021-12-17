#include "captiveportal.h"

CaptivePortal::CaptivePortal(QString redirectpage, QString intfname)
{
    string domain;
    struct addrinfo *servinfo;
    struct addrinfo hints;
    char host[16];

    redirectpage_ = redirectpage;
    intfname_ = intfname;
    domain = getDomainAtUrl(redirectpage.toStdString());

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(domain.c_str(), NULL, &hints, &servinfo))
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
    qInfo() << "domain=" << redirectpage << "," << "ip=" << QString(host_);

    setComponent();

    QObject::connect(
                &capturer_,
                SIGNAL(captured(GPacket*)),
                this,
                SLOT(processPacket(GPacket*)),
                Qt::DirectConnection
                );
}

void CaptivePortal::setComponent()
{
    capturer_.intfName_ = intfname_;

    tcpblock_.backwardRst_ = false;
    tcpblock_.backwardFin_ = true;
    tcpblock_.backwardFinMsg_ = QStringList{"HTTP/1.0 302 Redirected\r\n"
                                            "Location: "+redirectpage_+"\r\n"
                                            "\r\n"};
    tcpblock_.writer_ = &writer_;
}

string CaptivePortal::getDomainAtUrl(string url)
{
    string result;
    int cursorbegin = 0, cursorend = 0;
    if((cursorend = url.find("https://", cursorbegin)) != string::npos)
    {
        cursorbegin += 8;
    }
    else if((cursorend = url.find("http://", cursorbegin)) != string::npos)
    {
        cursorbegin += 7;
    }
    if((cursorend = url.find("/", cursorbegin)) != string::npos)
    {
        result = url.substr(cursorbegin, cursorend-cursorbegin);
    }
    else if((cursorend = url.find("/", cursorbegin)) == string::npos)
    {
        result = url.substr(cursorbegin);
    }
    return result;
}

bool CaptivePortal::doOpen()
{
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

void CaptivePortal::processPacket(GPacket *packet)
{
    const char* castedtcpdata = nullptr;
    if(packet->ethHdr_->type() != GEthHdr::Ip4)
    {
        return;
    }
    if(packet->ipHdr_->p() != GIpHdr::Tcp)
    {
        return;
    }
    if(packet->tcpHdr_->dport() == 443)
    {
        packet->ctrl.block_ = true;
        return;
    }
    else if(packet->tcpHdr_->dport() == 80)
    {
        if(packet->tcpData_.valid())
        {
            castedtcpdata = reinterpret_cast<const char*>(packet->tcpData_.data_);
        }
        else
        {
            qDebug() << "there is no tcpdata in packet";
            return;
        }
        qDebug() << uint32_t(packet->ipHdr_->dip())
                 << ", "
                 << uint32_t(host_) << "check both ip address";
        if(strncmp(castedtcpdata, "GET ", 4) == 0 && packet->ipHdr_->dip() != host_)
        {
            qDebug() << "Send redirect page data to client";
            tcpblock_.block(packet);
        }
    }
}
