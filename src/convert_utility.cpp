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
