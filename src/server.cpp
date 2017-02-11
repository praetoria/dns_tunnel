#include "hsocket.h"
#include "dns_packet.h"
#include "tunnel_dns.h"
#include "message.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    hsocket s(hsocket::UDP);
    std::string domain = "example.org";
    tunnel_dns tun_in(tunnel::INCOMING, dns::query_t, dns::A, domain);
    tunnel_dns tun_out(tunnel::OUTGOING, dns::response_t, dns::A, domain);
    tun_out.set_response_limit(10);
    message msg_ok(message::OK,"ok");
    std::string pkt,data;
    s.bind(53,"127.0.0.1");
    while (1) {
        data.clear();
        pkt.clear();
        s >> pkt;
        dns_packet d(pkt);
        std::string qname = d.questions[0].name;
        tun_in << qname;
        tun_in >> data;
        message m(data);
        std::cout << m.data << '\n';
        tun_out << msg_ok.str();
        d.set_response();
        for (int i = 0; i < 10; i++) {
            std::string ip;
            tun_out >> ip;
            if (ip.empty())
                break;
            d.add_response(ip,dns::A);
        }
        s << d.str();
    }
    s.close();
    return 0;
}
