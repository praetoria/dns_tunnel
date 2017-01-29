#include <iostream>
#include "../ip_utility.h"
#include "../hsocket.h"

void test_listen_udp (int port, const std::string& ip);

int main(int argc, char** argv) {
    test_listen_udp(1337,"127.0.0.1");
    return 0;
}

void test_listen_udp (int port, const std::string& ip) {
    hsocket s(hsocket::UDP);
    s.bind(port,ip);
    std::string data;
    s >> data;
    std::cout << data << '\n';
}
