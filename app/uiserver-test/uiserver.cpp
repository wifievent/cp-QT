#include "uiserver.h"
#include <string_view>

UIServer::UIServer() {
}

void UIServer::handleClnt(TcpClientSocket *clntsock) {

    char buffer[BUFSIZE];
    int len;

    while((len = clntsock->recv(buffer, BUFSIZE)) != -1) {
        uirequest_.resetData();
        if(len == 0) {
            DLOG(INFO) << "clntsock is shutdown";
            return;
        }
        uirequest_.addRequestPacket(buffer, len);
        uirequest_.parseRequestPacket();
        setHttpResponse();
        clntsock->send((char*)uiresponse_.getResponseData()->c_str(), uiresponse_.getResponseSize());
    }
    return;
}

void UIServer::setHttpResponse() {
    std::string path = uirequest_.getURL();
    uiresponse_.resetData();
    int size = 0;
    DLOG(INFO) << "request path:" << path;

    if(path.ends_with(".css")) {
        uiresponse_.setHTTPHeader("Content-Type", "text/css;charset=UTF-8");
    }
    else if(path.ends_with(".js")) {
        uiresponse_.setHTTPHeader("Content-Type", "text/javascript;charset=UTF-8");
    }
    else {
        uiresponse_.setHTTPHeader("Content-Type", "text/html");
    }

    if(denyDotDotPacket(path)) {
        return;
    }

    if(path == "/") {
        size = getWebUIData("/index.html");
        uiresponse_.setResponseBody(ui_);
    }
    else if(path == "/Start") {
        size = 5;
        uiresponse_.setResponseBody("start");
    }
    else if(path == "/Stop") {
        size = 4;
        uiresponse_.setResponseBody("stop");
    }
    else {
        size = getWebUIData(path);
        uiresponse_.setResponseBody(ui_);
    }

    std::string len = std::to_string(size);

    uiresponse_.setProtocol(HTTP1_1);
    uiresponse_.setStatusCode(200);
    uiresponse_.setReasonPhrase();
    uiresponse_.setHTTPHeader("Connection", "keep-alive");
    uiresponse_.setHTTPHeader("Content-Length", len.c_str());
    uiresponse_.setHTTPHeader("Date", getDateTime());
    uiresponse_.setHTTPHeader("Keep-Alive", "timeout=5, max=100");
    uiresponse_.setHTTPHeader("Server", "UIServer");
    uiresponse_.makeResponse();
}

int UIServer::getWebUIData(std::string path) {
    DLOG(INFO) << "Get local data from" << rootdir_+path;
    std::ifstream fin(rootdir_+path);
    int size = 0;

    if(fin.is_open()){
        fin.seekg(0, std::ios::end);
        size = fin.tellg();
        fin.seekg(0, std::ios::beg);
        fin.read(ui_, size);
    }
    return size;
}

std::string UIServer::getDateTime() {
    char date[30] = {'\0'};

    time_t now = time(0);
    tm *gmtm = gmtime(&now);
    char* dt = asctime(gmtm);

    char mon[4];
    char dayofweek[4];
    int year, day, hour, min, sec;

    sscanf(dt, "%s %s %d %d:%d:%d %d\n", dayofweek, mon, &day, &hour, &min, &sec, &year);
    sprintf(date, "%s, %02d %s %d %02d:%02d:%02d GMT", dayofweek, day, mon, year, hour, min, sec);
    std::string dateheader = date;
    return dateheader;
}

bool UIServer::denyDotDotPacket(std::string path)
{
    if(path.find("..") != std::string::npos) {
        DLOG(INFO) << "there is .. string from path:" << path;
        uiresponse_.setProtocol(HTTP1_1);
        uiresponse_.setStatusCode(403);
        uiresponse_.setReasonPhrase();
        uiresponse_.setHTTPHeader("Date", getDateTime());
        uiresponse_.setHTTPHeader("Server", "UIServer");
        uiresponse_.makeResponse();
        return true;
    }
    return false;
}

