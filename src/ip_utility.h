#ifndef _IP_UTILITY_H
#define _IP_UTILITY_H
#include <string>
/**
 * Transforms an IP address in a string into a uint32_t into the network byte order.
 */
uint32_t ipton(const std::string& ipstr);
#endif
