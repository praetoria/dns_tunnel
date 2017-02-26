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

void handle_stdin(std::string& output);
void handle_incoming(tunnel_dns& tun_in, hsocket& s);
void handle_outgoing(tunnel_dns& tun_out,hsocket& s);
message make_message(std::string& data,message::message_type t);
message parse_message(std::string& data);

int main(int argc, char** argv) {
    hsocket s(hsocket::UDP);
    std::string domain = "example.org";
    tunnel_dns tun_in(tunnel::INCOMING, dns::response_t, dns::A, domain);
    tunnel_dns tun_out(tunnel::OUTGOING, dns::query_t, dns::A, domain);
    message heartbeat(message::HEARTBEAT);
    tun_in.set_response_limit(10);
    std::string data_out,data_in;
    s.connect(53,"127.0.0.1");
    s << hsocket::NONBLOCKING;
    int counter = 0;
    while (1) {
        usleep(10);
        counter++;
        // get data to be sent from stdin
        handle_stdin(data_out);
        if (data_out.length() > 0) {
            // consumes the data_out
            message m = make_message(data_out,message::OK);
            tun_out << m.str();
        }
        // send outgoing data from tunnel
        if (counter > 100) {
            if (tun_out.bytes_available() == 0)
                tun_out << heartbeat.str();
            handle_outgoing(tun_out, s);
            counter = 0;
        }
        // recieve incoming data to tunnel
        handle_incoming(tun_in, s);
        tun_in >> data_in;
        message recvd(data_in);
        if (recvd.type == message::OK) {
            std::cout << recvd.data << '\n';
        }

    }
    return 0;
}


/* Makes a message from data_out and consumes all data
 * which is taken from data_out
 */
message make_message(std::string& data,message::message_type t) {
    unsigned int maxlen = message::MAXLEN;
    message m(t,data.substr(0,maxlen));
    data.erase(0,maxlen);
    return m;
}
void handle_outgoing(tunnel_dns& tun_out,hsocket& s) {
    dns_packet d(1337,dns::query_t);
    std::string qname;
    tun_out >> qname;
    if (qname.empty()) return;
    d.add_question(qname,dns::A);
    s << d.str();
}
void handle_incoming(tunnel_dns& tun_in, hsocket& s) {
    std::string data;
    s >> data;
    if (data.empty()) return;
    dns_packet d(data);
    // We need the pque because DNS protocol does not guarantee the order of responses
    pque<std::string> Q;
    for (auto r : d.responses)
        Q.insert(r.data);
    while (Q.size() > 0) {
        tun_in << Q.pop();
        Q.remove();
    }
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
        //std::cin >> tmp;
        std::getline(std::cin, tmp);
        buffer.append(tmp);
    }
}
