#include "tunnel_dns.h"
#include "dns_packet.h"
#include "convert_utility.h"

/* Creates a tunnel, which transforms raw data into
 * domain names or dns responses, or vice versa.
 * For example:
 * tunnel_dns my_tunnel(tunnel::OUTGOING, dns::query_t, dns::A)
 */
tunnel_dns::tunnel_dns(tunnel_type t_type, dns::dns_type d_type, dns::qtype q_type, std::string domain) : tunnel(t_type),d_type(d_type),q_type(q_type),domain(domain),response_limit(1) {
}

tunnel_dns& tunnel_dns::operator<<(const std::string& d) {
    if (t_type == OUTGOING) {
        data.append(d);
    } else {
        // TODO parse the dns query/response
    }
    return *this;   
}
/* Note: this does not append to the string.
 * This replaces a given string with the dns data.
 */
tunnel_dns& tunnel_dns::operator>>(std::string& d) {
    if (t_type == OUTGOING) {
        d = data_to_dns();
    } else {
        d = data;
        data.clear();
    }
    return *this;
}

/* Converts data from buffer into a dns record or domain name.
 * At the moment only A type of questions are supported.
 */
std::string tunnel_dns::data_to_dns() {
    std::string ret;
    if (d_type != dns::query_t || q_type != dns::A)
        return ret;
    for (int i = 0; i < 2; i++) {
        ret.append(to_hex(this->data,MAX_DNS_LABEL));
        ret.append(1,'.');
        this->data.erase(0,MAX_DNS_LABEL);
        if (data.empty())
            break;
    }
    ret.append(this->domain);
    return ret;
}

/* Sets how many response records per query we want to
 * generate.
 */
void tunnel_dns::set_response_limit(int limit) {
    response_limit = limit;
}
