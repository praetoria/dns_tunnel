#include "tunnel_dns.h"
#include "dns_packet.h"
#include "convert_utility.h"

/* Creates a tunnel, which transforms raw data into
 * domain names or dns responses, or vice versa.
 * For example:
 * tunnel_dns my_tunnel(tunnel::OUTGOING, dns::query_t, dns::A)
 */
tunnel_dns::tunnel_dns(tunnel_type t_type, dns::dns_type d_type, dns::qtype q_type, std::string domain) : tunnel(t_type),d_type(d_type),q_type(q_type),domain(domain),response_limit(1),response_count(0),response_len(0) {
}
size_t tunnel_dns::bytes_available() const {
    return data.length();
}

tunnel_dns& tunnel_dns::operator<<(const std::string& d) {
    if (t_type == OUTGOING) {
        data.append(d);
        if (d_type == dns::response_t)
            response_count = 0;
    } else {
        this->dns_to_data(d);
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
        d.append(data);
        data.clear();
    }
    return *this;
}

/* Converts data from buffer into a dns record or domain name.
 * At the moment only questions of A type are supported.
 * Only used in OUTGOING tunnels.
 */
std::string tunnel_dns::data_to_dns() {
    std::string ret;
    if (this->data.empty())
        return ret;
    if (q_type != dns::A)
        return ret;

    if (d_type == dns::query_t) {
        for (int i = 0; i < 2; i++) {
            ret.append(to_hex(this->data,MAX_DNS_LABEL));
            ret.append(1,'.');
            this->data.erase(0,MAX_DNS_LABEL);
            if (data.empty())
                break;
        }
        ret.append(this->domain);
    } else {
        // Max amount of data is calculated like this because there is
        // metadata for more information check dns_protocol document
        // (which doesn't exist yet) TODO
        int data_left = response_limit*4 - response_limit - 1;

        if (response_count > 0)
            data_left -= (response_count-1)*3 - 2;

        if (data_left > data.length())
            data_left = data.length();

        if (data_left == 0) return ret;

        if (response_count == 0) {
            ret.append(1,(char)response_count++);
            ret.append(1,(char)data_left);
            ret.append(data.substr(0,2));

            while (ret.length() < 4) ret.append(1,'\0');
            data.erase(0,2);
        } else {
            ret.append(1,(char)response_count++);
            ret.append(data.substr(0,3));

            while (ret.length() < 4) ret.append(1,'\0');
            data.erase(0,3);
        }
        if (response_count >= response_limit)
            response_count = 0;
    }
    return ret;
}

/* Converts data from dns form to raw data and appends it
 * to the data buffer.
 * At the moment only A type is supported
 * Only used in INCOMING tunnels.
 */
void tunnel_dns::dns_to_data(std::string d) {
    if (q_type != dns::A)
        return;

    if (d_type == dns::query_t) {
        size_t end = d.rfind(domain);
        if (end == std::string::npos) return;
        std::string subdomain = d.substr(0,end);

        size_t pos = 0, dot = std::string::npos;
        while ((dot = subdomain.find('.',pos)) != std::string::npos) {
            std::string hex_data = subdomain.substr(pos,dot-pos);
            data.append(from_hex(hex_data));
            pos = dot+1;
        }
    } else {
        if (d[0] != response_count)
            return;

        if (d[0] == 0) {
            response_len = d[1];

            int length = (response_len < 2) ? response_len : 2;
            data.append(d.substr(2,length));
            response_len -= length;
        } else {
            int length = (response_len < 3) ? response_len : 3;   
            data.append(d.substr(1,length));
            response_len -= length;
        }
        response_count++;
        if (response_count >= response_limit || response_len <= 0)
            response_count = 0;
    }
}

/* Sets how many response records max per query we want to
 * generate.
 */
void tunnel_dns::set_response_limit(int limit) {
    response_limit = limit;
}
