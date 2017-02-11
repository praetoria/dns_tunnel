#ifndef _MESSAGE_H
#define _MESSAGE_H
#include <string>

class message {
    public:
    enum message_type : uint8_t { HEARTBEAT, OK, M_ERROR };
    message_type type;
    std::string data;

    message(message_type,std::string data = "");
    
    message(std::string); // construct a message from bytes
    std::string str() const; // convert a message into bytes
};
bool operator!=(const message& lhs, const message& rhs);
bool operator==(const message& lhs, const message& rhs);
#endif
