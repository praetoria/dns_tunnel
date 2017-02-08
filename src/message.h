#ifndef _MESSAGE_H
#define _MESSAGE_H
#include <string>

class message {
    public:
    enum message_type : uint8_t { HEARTBEAT, OK, M_ERROR };
    message_type type;
    std::string data;

    message(message_type,std::string);
    // construct a message from bytes
    message(std::string);
    // convert a message into bytes
    std::string str() const;
};
bool operator!=(const message& lhs, const message& rhs);
bool operator==(const message& lhs, const message& rhs);
#endif
