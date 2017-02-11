#include <iostream>
#include <string>
#include "hsocket.h"
#include "dns_packet.h"
#include "tunnel_dns.h"
#include "message.h"
#ifdef WIN32
#include <windows.h>
#define usleep Sleep
#else
#include <unistd.h>
#endif


int main(int argc, char** argv) {
    hsocket s(hsocket::UDP);
    std::string domain = "example.org";
    tunnel_dns tun_in(tunnel::INCOMING, dns::response_t, dns::A, domain);
    tunnel_dns tun_out(tunnel::OUTGOING, dns::query_t, dns::A, domain);
    tun_in.set_response_limit(10);
    std::string pkt,data;
    s.connect(53,"127.0.0.1");
    while (1) {
        pkt.clear();
        data.clear();
        std::string input, qname;
        std::cin >> input;
        dns_packet d(1337,dns::query_t);
        message msg(message::OK,input);
        tun_out << msg.str();
        tun_out >> qname;
        d.add_question(qname,dns::A);
        s << d.str();
        s >> pkt;
        dns_packet resp(pkt);
        for (auto r : resp.responses) {
            tun_in << r.data;
        }
        tun_in >> pkt;
        message msg_resp(pkt);
        std::cout << msg_resp.data << '\n';
    }
    return 0;
}
