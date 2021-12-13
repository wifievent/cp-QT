#pragma once

#include <QCoreApplication>

#include <GAutoArpSpoof>
#include <GTcpBlock>
#include <GPcapDeviceWrite>
#include <GApp>
#include <GJson>
#include <GNetFilter>

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
#include <map>

#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/ether.h>
#include <unistd.h>
