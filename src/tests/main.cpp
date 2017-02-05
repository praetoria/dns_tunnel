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
#include "../dns_packet.h"

class dns_test_p : public dns_packet {
    public:
        dns::DNS_HEADER get_header(){ return this->header; }
        dns_test_p(unsigned short id, dns::dns_type type) : dns_packet(id,type) {};
        dns_test_p(const std::string& str) : dns_packet(str) {};
};


std::string test_udp (int& success);
std::string test_nonblocking_udp (int& success);
std::string test_dns_to_str_questions (int& success);
std::string test_dns_to_str_answers (int& success);

int main(int argc, char** argv) {
    std::cout << "Running tests...\n";
    std::vector<std::string(*)(int&)> tests = { test_udp, test_nonblocking_udp, test_dns_to_str_questions, test_dns_to_str_answers };
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

// hsocket tests start

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
            if (recieved.length())
                break;
        }
    }
    listener.close();
    connector.close();
    success = recieved == data;
    return "UDP nonblocking test";
}

// dns tests start

std::string test_dns_to_str_questions (int& success) {
    // create a dns query packet with two questions
    dns_test_p first (1337,dns::query_t);
    first.add_question("helsinki.fi", dns::A);
    first.add_question("helsinki.fi", dns::ANY);

    // create the second from the byte representation of the first
    std::string d = first.str();
    dns_test_p second(d);
    
    success = 1;
    // compare the two
    if (first.get_header() != second.get_header()) {
        std::cout << "testing headers failed\n";
        first.print_header();
        second.print_header();
        success = 0;
    }

    if (first.questions.size() == second.questions.size()) {
        for (int i = 0 ; i < 2; ++i) {
            if (first.questions[i] != second.questions[i]) {
                first.print_questions();
                second.print_questions();
                success = 0;
                break;
            }
        }
    } else {
        success = 0;
    }
    return "DNS to str test with questions";
}
std::string test_dns_to_str_answers (int& success) {
    // create a dns query packet with two questions
    dns_test_p first (1337,dns::query_t);
    first.add_question("helsinki.fi", dns::A);
    // add response
    first.add_response(ipton("127.0.0.1"),dns::A);

    // create the second from the byte representation of the first
    std::string d = first.str();
    dns_test_p second(d);
    
    success = 1;
    // compare the two
    if (first.get_header() != second.get_header()) {
        std::cout << "testing headers failed\n";
        first.print_header();
        second.print_header();
        success = 0;
    }

    if (first.questions.size() == second.questions.size()) {
        for (int i = 0 ; i < first.questions.size(); ++i) {
            if (first.questions[i] != second.questions[i]) {
                first.print_questions();
                second.print_questions();
                success = 0;
                break;
            }
        }
    } else if (first.responses.size() == second.responses.size()) {
        for (int i = 0 ; i < first.responses.size(); ++i) {
            if (first.responses[i] != second.responses[i]) {
                first.print_responses();
                second.print_responses();
                success = 0;
                break;
            }
        }
    } else {
        success = 0;
    }
    return "DNS to str test with questions and answers";
}
