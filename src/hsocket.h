#ifndef __SOCKET_H
#define __SOCKET_H
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#define SOCKET int
#include <netinet/in.h>
#endif
#include <string>

class hsocket {
    public:
    enum socket_t { TCP, UDP };
    enum block_mode_t { NONBLOCKING, BLOCKING };
    hsocket(socket_t);
    hsocket& operator<<(const block_mode_t);
    hsocket& operator<<(const std::string&);
    hsocket& operator>>(std::string&);
    int good();
    void connect(uint16_t port,uint32_t addr);
    void connect(uint16_t port,const std::string& addr);
    void close();
    void bind(uint16_t port,uint32_t addr);
    void bind(uint16_t port,const std::string& addr);
    private:
    static int socket_n;
    socket_t type;
    SOCKET s;
    struct sockaddr_in send_addr;
    socklen_t send_len;
    struct sockaddr_in recv_addr;
    static void init(uint16_t,uint32_t,struct sockaddr_in&);

};
#endif
