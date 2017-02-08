#include "ip_utility.h"
#include <string>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
// For some reason these are missing from the mingw windows headers
extern "C" WINSOCK_API_LINKAGE const char* WSAAPI inet_pton(int af, const char* src, void* dest);
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
std::string ntoipstr(const std::string& ipstr) {
    std::string ret;
    if (ipstr.length() != 4) return ret;
    struct sockaddr_in sa;
    char str[INET_ADDRSTRLEN] = {0};
    sa.sin_addr.s_addr = *(uint32_t*)ipstr.c_str();
    inet_ntop(AF_INET,&(sa.sin_addr),str,INET_ADDRSTRLEN);
    ret = str;
    return ret;
}
std::string iptonstr(const std::string& ipstr) {
    struct sockaddr_in sa;
    inet_pton(AF_INET,ipstr.c_str(), &(sa.sin_addr));
    uint32_t ip = sa.sin_addr.s_addr;
    std::string ret((char*)&ip,sizeof(ip));
    return ret;
    // TODO ipv6
}
