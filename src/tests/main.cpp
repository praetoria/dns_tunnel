#ifdef WIN32
#include <windows.h>
#define usleep Sleep
#else
#include <unistd.h>
#endif
#include <iostream>
#include <vector>
#include "../ip_utility.h"
#include "../hsocket.h"

std::string test_udp (int& success);
std::string test_nonblocking_udp (int& success);

int main(int argc, char** argv) {
    std::cout << "Running tests...\n";
    std::vector<std::string(*)(int&)> tests = { test_udp, test_nonblocking_udp };
    int succeeded = tests.size();
    for ( auto test : tests) {
        int success = 0;
        std::string test_name = test(success);
        if (success) {
            std::cout << "Success: ";
        } else {
            std::cout << "Fail: ";
            succeeded--;
        }
        std::cout << test_name << '\n';
    }
    if (succeeded == tests.size()) {
        std::cout << "All tests passed!\n";
        return 0;
    } else {
        std::cout << succeeded << " tests passed, " << tests.size()-succeeded << " tests failed\n";
        return 1;
    }
}

/* Tests are defined here
 * 
 */

std::string test_udp (int& success) {
    hsocket s(hsocket::UDP);
    s.bind(1337,"127.0.0.1");
    hsocket sender(hsocket::UDP);
    sender.connect(1337,"127.0.0.1");

    std::string data = "udp_listen_test";
    std::string received;

    sender << data;
    s >> received;

    s.close();
    sender.close();
    success = data == received;
    return "UDP blocking test";
}
std::string test_nonblocking_udp (int& success) {
    hsocket listener(hsocket::UDP);
    hsocket connector(hsocket::UDP);
    listener.bind(1234,"127.0.0.1");
    listener << hsocket::NONBLOCKING;
    connector << hsocket::NONBLOCKING;
    connector.connect(1234,"127.0.0.1");
    success = 0;
    int sent = 0;
    std::string recieved;
    std::string data = "Test data 123123";
    while (1) {
        usleep(100);
        listener >> recieved;
        if (!sent) {
            connector << data;
            sent = 1;
        } else {
            break;
        }
    }
    listener.close();
    connector.close();
    success = recieved == data;
    return "UDP nonblocking test";
}
