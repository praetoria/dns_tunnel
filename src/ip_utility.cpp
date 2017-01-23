#include "ip_utility.h"
#include <string>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
extern "C" WINSOCK_API_LINKAGE const char* WSAAPI inet_ntop(int af, const void* src,
char *dst, socklen_t size);
#else
#include <arpa/inet.h>
#endif

uint32_t ipton(const std::string& ipstr) {
    struct sockaddr_in sa;
    inet_pton(AF_INET,ipstr.c_str(), &(sa.sin_addr));
    return sa.sin_addr.s_addr;
    // TODO ipv6
}
