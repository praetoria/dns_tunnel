#include "message.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/* Basic constructor
 */
message::message(message_type type, std::string data) : type(type), data(data) {
}

/* Parses a message from bytes
 */
message::message(std::string &bytes) {
    if (bytes.length() < 3) {
        this->type = M_ERROR;
        return;
    }
    this->type = (message_type)bytes[0];
    if (this->type != HEARTBEAT && this->type != OK) {
        this->type = M_ERROR;
        return;
    }
    uint16_t data_len = *(uint16_t*)&(bytes[1]);
    data_len = ntohs(data_len);
    if (data_len + 3 > bytes.length()) {
        this->type = M_ERROR;
        return;
    }
    this->data = bytes.substr(3,data_len);
    bytes.erase(0,data_len+3);
}

/* Converts a message into bytes, with the following form:
 * |type|data length| data |
 * |8bit|   16bit   | nbits|
 * Data length is in network byte order.
 */
std::string message::str() const {
    std::string ret;
    uint16_t len = this->data.length();
    uint16_t network_len = htons(len);

    ret.append(1,this->type);
    ret.append((char*)&network_len,sizeof(uint16_t));
    ret.append(this->data,0,len);
    return ret;
}

bool operator!=(const message& lhs, const message& rhs) {
    if (lhs.type != rhs.type)
        return true;
    if (lhs.data != rhs.data)
        return true;
    return false;
}
bool operator==(const message& lhs, const message& rhs) {
    return !(lhs != rhs);
}
