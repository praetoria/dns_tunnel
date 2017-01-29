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
/* When compiling for Windows, the first socket
 * opened runs WSAStartup, and when the last of
 * the currently active sockets closes, it runs
 * WSACleanup.
 */

class hsocket {
    public:
    enum socket_t { TCP, UDP };
    enum block_mode_t { NONBLOCKING, BLOCKING };
    hsocket(socket_t);
    hsocket& operator<<(const block_mode_t);
    hsocket& operator<<(const std::string&);
    hsocket& operator>>(std::string&);
    int good();
    // addr has to be in network byte order
    void connect(uint16_t port,uint32_t addr);
    void connect(uint16_t port,const std::string& addr);
    void close();
    // addr has to be in network byte order
    void bind(uint16_t port,uint32_t addr);
    void bind(uint16_t port,const std::string& addr);
    private:
    // amount of open sockets
    static int socket_n;
    socket_t type;
    SOCKET s;
    /* If using UDP the first to connect to a listening
     * socket is marked as the only one to listen to.
     * Kind of simulating a connection with UDP.
     * send_addr contains the address to listen to.
     */
    struct sockaddr_in send_addr;
    socklen_t send_len;
    struct sockaddr_in recv_addr;
    static void init(uint16_t,uint32_t,struct sockaddr_in&);

};
#endif
