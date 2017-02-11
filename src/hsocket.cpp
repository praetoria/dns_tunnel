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

/* Constructor for the hsocket.
 * At the moment this does not support TCP fully
 * so only call it with hsocket::UDP
 */
hsocket::hsocket(hsocket::socket_t type) : type(type) {
#ifdef WIN32
    // Only initiate Winsock2 if there are no sockets open
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

/* hsocket::good()
 * used to see if socket is still good, or connnected (TCP).
 * In case of an error the value
 * of the socket is set to INVALID_SOCKET
 */
int hsocket::good() {
    return s != INVALID_SOCKET;
}

/* hsocket::connect(port,addr)
 * With UDP sockets this is used to specify the address
 * that the packets are sent to through the socket.
 * UDP is actually connectionless, but this is called connect
 * so that the use would be consistent with TCP and UDP sockets.
 */
void hsocket::connect(uint16_t port,const std::string& addr) {
    connect(port,ipton(addr));
}
/* With this connect, the addr has to be in network byte order.
 */
void hsocket::connect(uint16_t port,uint32_t addr) {
    init(port,addr,send_addr);
    send_len = sizeof(send_addr);
}
/* hsocket::bind(port,addr)
 * starts listening on the port and interface
 */
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
        // We should cleanup if this is the last open socket
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
/* used to set the socket into either blocking or nonblocking mode
 * e.g. s << hsocket::NONBLOCKING
 */
hsocket& hsocket::operator<<(const block_mode_t mode) {
#ifdef WIN32
    u_long nMode = (mode == NONBLOCKING) ? 1 : 0;
    if (ioctlsocket (s, FIONBIO, &nMode) == SOCKET_ERROR) {
        this->close();
    }
#else
    int flags = fcntl(s,F_GETFL);
    flags = (mode == NONBLOCKING) ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) != 0) {
        this->close();
    }
#endif
    return *this;
}
hsocket& hsocket::operator<<(const timeout t) {
#ifdef WIN32
    DWORD timeout = t.t * 1000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
    struct timeval timeout;
    timeout.tv_sec = t.t;
    timeout.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
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
