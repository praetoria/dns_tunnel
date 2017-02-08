#include "convert_utility.h"
#include <string>
#include <vector>

std::string to_hex(const std::string& data, int len) {
    std::string ret;
    if (len > data.length())
        len = data.length();
    static const std::vector<char> val_to_hex = {
        '0','1','2','3','4','5','6','7',
        '8','9','a','b','c','d','e','f' };
    for (int i = 0; i < len; i++) {
        int a = (data[i] & 0xf0)>>4;
        int b = data[i] & 0xf;
        ret.append(1,val_to_hex[a]);
        ret.append(1,val_to_hex[b]);
    }
    return ret;
}
std::string from_hex(const std::string& data) {
    std::string ret;
    if (data.length()%2 != 0) return ret;
    for (int i = 0; i < data.length()-1; i+=2) {
        int a = hex_to_val(data[i]),
            b = hex_to_val(data[i+1]);

        if (a < 0 || b < 0) {
            ret.clear();
            break;
        }
        char c = b | (a << 4);
        ret.append(1,c);
    }
    return ret;
}
int hex_to_val(char c) {
    int ret = -1;
    if (c <= 'F' && c >= 'A')
        c = c ^ 0x20; // to lowercase
    if (c <= 'f' && c >= 'a') {
        ret = c - 'a' + 10;
    } else if (c <= '9' && c >= '0') {
        ret = c - '0';
    }
    return ret;
}
