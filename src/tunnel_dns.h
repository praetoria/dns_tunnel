#ifndef _TUNNEL_DNS_H
#define _TUNNEL_DNS_H
#include "tunnel.h"
#include "dns_packet.h"

class tunnel_dns : protected tunnel {
    static const int MAX_DNS_LABEL = 30;
    const dns::dns_type d_type;
    const dns::qtype q_type;
    std::string data;
    std::string domain;
    std::string data_to_dns();
    void dns_to_data(std::string);
    int response_limit;
    public:
        tunnel_dns(tunnel_type, dns::dns_type,dns::qtype,std::string);
        virtual tunnel_dns& operator<<(const std::string&);
        virtual tunnel_dns& operator>>(std::string&);
        void set_response_limit(int);
};
#endif
