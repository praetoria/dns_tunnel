#ifdef WIN32
#define usleep Sleep
#else
#include <unistd.h>
#endif
#include <iostream>
#include <string>
#include "hsocket.h"
#include "dns_packet.h"
#include "tunnel_dns.h"
#include "message.h"
#include "pque.h"


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
        // Have to use priority que as the DNS standard does not guarantee the responses in order
        pque<std::string> Q;
        for (auto r : resp.responses) {
            Q.insert(r.data);
        }
        while (Q.size() > 0) {
            tun_in << Q.pop();
            Q.remove();
        }
        tun_in >> pkt;
        message msg_resp(pkt);
        std::cout << msg_resp.data << '\n';
    }
    return 0;
}
