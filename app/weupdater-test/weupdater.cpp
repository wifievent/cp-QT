#include "weupdater.h"

WEUpdater::WEUpdater()
{

}

void WEUpdater::getIpAddress(QString versionurl)
{
    QUrl url = versionurl;
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
    path_ = url.path();
    qInfo() << "host=" << QString(host_) << "," << "path=" << path_;
}

void WEUpdater::setHttpRequest()
{
    vector<pair<string, string>> headervector;
    headervector.push_back(make_pair("Host", "wifievent.io"));
    headervector.push_back(make_pair("Connection", "keep-alive"));
    headervector.push_back(make_pair("Accept", "text/html"));
    headervector.push_back(make_pair("Accept-Encoding", "gzip, deflate"));
    headervector.push_back(make_pair("Accept-Language", "en-US, ko"));
    headervector.push_back(make_pair("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) "
                                                   "AppleWebKit/537.36 (KHTML, like Gecko) "
                                                   "Chrome/95.0.4638.69 Safari/537.36"));
    versionrequest_.setMethod(GET);
    versionrequest_.setURL(path_.toStdString());
    versionrequest_.setProtocol(HTTP1_1);
    versionrequest_.setUserAgent("Mozilla/5.0 (X11; Linux x86_64) "
                                 "AppleWebKit/537.36 (KHTML, like Gecko) "
                                 "Chrome/95.0.4638.69 Safari/537.36");
    versionrequest_.setHTTPHeaderVector(&headervector);
    qDebug() << "make Request packet";
    versionrequest_.makeRequest();
}

bool WEUpdater::checkVersion(QString versionurl, string currentversion)
{
    char buf[BUFSIZE];
    getIpAddress(versionurl);
    setHttpRequest();
    TcpClient client;
    if(client.connect(QString(host_).toStdString(), 80))
    {
        qDebug() << "connect error";
        return "";
    }
    if(client.send((char*)versionrequest_.getRequestData()->c_str(), versionrequest_.getRequestSize()) == -1)
    {
        qDebug() << "send error";
        return "";
    }
    int len = client.recv(buf, BUFSIZE);
    if(len == -1)
    {
        qDebug() << "recv error";
        return "";
    }
    client.disconnect();
    versionresponse_.addResponsePacket(buf, len);
    versionresponse_.parseResponsePacket();
    string latestversion = versionresponse_.getResponseBody().erase(versionresponse_.getResponseBody().length() - 2, 2);
    qDebug() << "latest version:" << latestversion.c_str();
    qDebug() << "current version:" << currentversion.c_str();

    int current[4];
    int latest[4];
    sscanf(latestversion.c_str(), "\"v%d.%d.%d.%d\"", &latest[0], &latest[1], &latest[2], &latest[3]);
    sscanf(currentversion.c_str(), "\"v%d.%d.%d.%d\"", &current[0], &current[1], &current[2], &current[3]);
    for(int i = 0; i < 4; i++)
    {
        if(latest[i] > current[i])
        {
            return true;
        }
        else if(latest[i] < current[i])
        {
            qDebug() << "current version is more higher error";
        }
    }
    return false;
}
