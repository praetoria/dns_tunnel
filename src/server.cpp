#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#define usleep Sleep
#else
#include <unistd.h>
#endif
#include <iostream>
#include <string>
#include "hsocket.h"
#include "dns_packet.h"
#include "message.h"
#include "tunnel_dns.h"
#define RESP_LIM 10
message make_message(std::string& data, message::message_type t);
void handle_incoming(tunnel_dns& tun_in, hsocket& s, dns_packet& last);
void handle_outgoing(tunnel_dns& tun_out, hsocket& s, dns_packet& last);
void handle_stdin(std::string& buffer);

int main(int argc, char** argv) {
    hsocket s(hsocket::UDP);

    std::string domain = "example.org";
    tunnel_dns tun_in(tunnel::INCOMING, dns::query_t, dns::A, domain);
    tunnel_dns tun_out(tunnel::OUTGOING, dns::response_t, dns::A, domain);
    tun_out.set_response_limit(RESP_LIM);
    message heartbeat(message::HEARTBEAT);

    std::string data_out,data_in;
    s.bind(53,"127.0.0.1");

    s << hsocket::NONBLOCKING;
    dns_packet last(1,dns::none);
    while (1) {
        usleep(10);
#ifndef WIN32
        // async IO does not work like this in Windows
        // TODO thread or WaitForSingleObject solution
        handle_stdin(data_out);
#endif
        handle_incoming(tun_in,s,last);
        tun_in >> data_in;
        message m(data_in);
        if (m.type == message::OK) {
            std::cout << m.data << '\n';
        }
        if (!data_out.empty()) {
            m = make_message(data_out,message::OK);
            tun_out << m.str();
        }
        if (last.rcode() == dns::NON_ERROR && tun_out.bytes_available() == 0) {
            tun_out << heartbeat.str();
        }
        handle_outgoing(tun_out,s,last);
    }
    s.close();
    return 0;
}

message make_message(std::string& data,message::message_type t) {
    unsigned int maxlen = message::MAXLEN;
    message m(t,data.substr(0,maxlen));
    data.erase(0,maxlen);
    return m;
}

void handle_outgoing(tunnel_dns& tun_out, hsocket& s, dns_packet& last) {
    if (last.rcode()!=dns::NON_ERROR || !tun_out.bytes_available())
        return;

    for (int i = 0; i < RESP_LIM; i++) {
        std::string ip;
        tun_out >> ip;
        if (ip.empty())
            break;
        last.add_response(ip,dns::A);
    }
    last.set_response();
    s << last.str();
    last = dns_packet(1,dns::none);
    return;
}
void handle_incoming(tunnel_dns& tun_in, hsocket& s, dns_packet& last) {
    std::string data;
    s >> data;
    if (data.empty())
        return;
    dns_packet d(data);
    if (d.rcode() != dns::NON_ERROR || d.questions.size() == 0)
        return;
    last = d;
    std::string qname = d.questions[0].name;
    tun_in << qname;
}

void handle_stdin(std::string& buffer) {
    struct timeval tv;
    fd_set fds;
    FD_ZERO (&fds);   
    FD_SET (STDIN_FILENO, &fds);
    tv.tv_sec = 0; 
    tv.tv_usec = 100;
    int result = select (STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    if (result != 0) {
        std::string tmp;
        std::getline(std::cin, tmp);
        buffer.append(tmp);
    }
}
