#include <iostream>
#include <vector>
#include "../ip_utility.h"
#include "../hsocket.h"

std::string test_listen_udp (int& success);

int main(int argc, char** argv) {
    std::cout << "Running tests...\n";
    std::vector<std::string(*)(int&)> tests = { test_listen_udp };
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

std::string test_listen_udp (int& success) {
    hsocket s(hsocket::UDP);
    s.bind(1337,"127.0.0.1");
    std::string data;
    s >> data;
    success = data == "udp_listen_test";
    return "UDP listen test";
}
