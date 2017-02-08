#ifndef _MESSAGE_H
#define _MESSAGE_H
#include <string>

class message {
    public:
    enum message_type : uint8_t { HEARTBEAT, OK, ERROR };
    message_type type;
    std::string data;

    message(message_type,std::string);
    message(std::string);
    std::string str() const;
};
bool operator!=(const message& lhs, const message& rhs);
bool operator==(const message& lhs, const message& rhs);
#endif
