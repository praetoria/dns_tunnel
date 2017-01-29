#include "hsocket.h"
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#define INVALID_SOCKET 0
#endif
#include <iostream>
#include <algorithm>
#include "ip_utility.h"

// amount of sockets
int hsocket::socket_n = 0;

hsocket::hsocket(hsocket::socket_t type) : type(type) {
#ifdef WIN32
    if (socket_n == 0) {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
        if (iResult != 0) {
            s = INVALID_SOCKET;
            return;
        }
    }
#endif
    socket_n++;
    int t = SOCK_DGRAM;
    if (type == hsocket::TCP)
        t = SOCK_STREAM;
    s = ::socket(AF_INET,t,0);
    send_len = 0;
}
void hsocket::init(uint16_t port, uint32_t address, struct sockaddr_in& addr) {
    addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = address;
}
int hsocket::good() {
    return s != INVALID_SOCKET;
}
void hsocket::connect(uint16_t port,const std::string& addr) {
    connect(port,ipton(addr));
}
void hsocket::connect(uint16_t port,uint32_t addr) {
    init(port,addr,send_addr);
    send_len = sizeof(send_addr);
}
void hsocket::bind(uint16_t port,const std::string& addr) {
    bind(port,ipton(addr));
}
void hsocket::bind(uint16_t port,uint32_t addr) {
    init(port,addr,recv_addr);
    ::bind(s,(struct sockaddr*)&recv_addr,sizeof(recv_addr));
}
void hsocket::close() {
    if (s != INVALID_SOCKET) {
        shutdown(s,2);
        s = INVALID_SOCKET;
        socket_n--;
#ifdef WIN32
        if (socket_n == 0)
            WSACleanup();
#endif
    }
}
hsocket& hsocket::operator<<(const std::string& data) {
    if (type == UDP) {
        sendto(s,data.data(),data.length(),0,(struct sockaddr*)&(send_addr),sizeof(send_addr));
    } else {
    }
    return *this;
}
hsocket& hsocket::operator>>(std::string& data) {
    char recieved[8190];
    struct sockaddr_in dummy;
    socklen_t dummy_len = sizeof(struct sockaddr_in);
    int len = recvfrom(s,recieved,8190,0,(sockaddr*)&dummy,&dummy_len);
    if (len <= 0) return *this;
    if (!send_len) {
        send_addr = dummy;
        send_len = dummy_len;
    } else if (send_addr.sin_addr.s_addr != dummy.sin_addr.s_addr) {
        std::cout << "recieved data from " << std::hex << dummy.sin_addr.s_addr << ", expected from " << send_addr.sin_addr.s_addr << "\n";
        return *this;
    } else {
        send_addr.sin_port = dummy.sin_port;
    }
    data.append(recieved,len);
    return *this;
}
