#include "dns_packet.h"
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
extern "C" WINSOCK_API_LINKAGE const char* WSAAPI inet_ntop(int af, const void* src,
char *dst, socklen_t size);
#else
#include <arpa/inet.h>
#endif
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>

/* struct compare functions for tests */
bool operator!=(const dns::DNS_HEADER& lhs, const dns::DNS_HEADER& rhs) {
    return !(lhs == rhs);
}
bool operator==(const dns::DNS_HEADER& lhs, const dns::DNS_HEADER& rhs)
{
    bool ret = true;
    ret &= lhs.id == rhs.id;
    if (ret == false)
        std::cout << "id " << ntohs(lhs.id) << " " << ntohs(rhs.id) << '\n';
    ret &= lhs.rd == rhs.rd;
    if (!ret)
        std::cout << "rd " << (int)(lhs.rd) << " " << (int)(rhs.rd) << '\n';
    ret &= lhs.tc == rhs.tc;
    ret &= lhs.aa == rhs.aa;
    ret &= lhs.opcode == rhs.opcode;
    ret &= lhs.qr == rhs.qr;
    ret &= lhs.cd == rhs.cd;
    ret &= lhs.ad == rhs.ad;
    ret &= lhs.z == rhs.z;
    ret &= lhs.ra == rhs.ra;
    ret &= lhs.q_count == rhs.q_count;
    ret &= lhs.ans_count == rhs.ans_count;
    ret &= lhs.auth_count == rhs.auth_count;
    ret &= lhs.add_count == rhs.add_count;
    return ret;
}
bool operator!=(const dns::QUERY& lhs, const dns::QUERY& rhs) {
    return !(lhs == rhs);
}
bool operator==(const dns::QUERY& lhs, const dns::QUERY& rhs) {
    if (lhs.name != rhs.name) return false;
    if (lhs.ques.qtype != rhs.ques.qtype || lhs.ques.qclass != rhs.ques.qclass)
        return false;
    return true;
}
bool operator!=(const dns::RES_RECORD& lhs, const dns::RES_RECORD& rhs) {
    return !(lhs == rhs);
}
bool operator==(const dns::RES_RECORD& lhs, const dns::RES_RECORD& rhs) {
    if (lhs.name != rhs.name) return false;
    if (lhs.data != rhs.data) return false;
    if (lhs.resource.type != rhs.resource.type
            || lhs.resource._class != rhs.resource._class
            || lhs.resource.ttl != rhs.resource.ttl
            || lhs.resource.data_len != rhs.resource.data_len)
        return false;
    return true;
}


/* dns_packet member functions */

dns_packet::dns_packet(unsigned short message_id, dns::dns_type type) {
    if (type == dns::response_t) {
        this->header.qr = 1;
    } else {
        this->header.qr = 0;
    }
    // recursion desired
    this->header.rd = 1;
    this->header.tc = 0;
    this->header.aa = 0;
    this->header.cd = 0;
    this->header.ad = 0;
    this->header.z = 0;
    this->header.ra = 0;
    // standard query
    this->header.opcode = 0;
    this->header.id = htons(message_id);
    // this changes when new questions are added
    this->header.q_count = 0;
    this->header.ans_count = 0;
    this->header.auth_count = 0;
    this->header.add_count = 0;
    this->header.rcode = 0;
}
dns_packet::dns_packet(const std::string& bytes) : dns_packet(0) {
    int pos = 0;
    int len = bytes.length();
    // if parsing fails
    this->header.rcode = dns::PARSING_ERROR;
    // parse header
    if (len < sizeof(dns::DNS_HEADER)) return;
    std::string h_bytes = bytes.substr(pos,sizeof(dns::DNS_HEADER));
    pos += sizeof(header);
    dns::DNS_HEADER header = *(dns::DNS_HEADER*)(h_bytes.data());
    std::vector<dns::QUERY> questions;
    int q_count = ntohs(header.q_count);
    // parse questions
    for (int i = 0; i < q_count; i++) {
        dns::QUERY q;
        size_t null = bytes.find('\0',pos);
        if (null == std::string::npos || null-pos > 255) return;
        q.name = qname_to_ascii(bytes.substr(pos,null-pos));
        pos = null+1;
        if (len < pos + sizeof(dns::QUESTION)) return;
        q.ques = *(dns::QUESTION*)(bytes.substr(pos,sizeof(dns::QUESTION)).data());
        pos += sizeof(dns::QUESTION);
        questions.push_back(std::move(q));
    }
    // parse answers
    int ans_count = ntohs(header.ans_count);
    for (int i = 0; i < ans_count; i++) {
        pos = this->parse_response(bytes,pos,len,RESPONSE);
        if (pos < 0 || pos > len) {
            this->responses.clear();
            return;
        }
    }
    // parse authorities
    int auth_count = ntohs(header.auth_count);
    for (int i = 0; i < auth_count; i++) {
        pos = this->parse_response(bytes,pos,len,AUTHORITY);
        if (pos < 0 || pos > len) {
            this->authorities.clear();
            return;
        }
    }
    // parse additional entries
    int add_count = ntohs(header.add_count);
    for (int i = 0; i < add_count; i++) {
        pos = this->parse_response(bytes,pos,len,ADDITIONAL);
        if (pos < 0 || pos > len) {
            this->additionals.clear();
            return;
        }
    }
    this->header = header;
    this->questions = std::move(questions);
}
// mainly for debug purposes
void dns_packet::print_header() const {
    dns::DNS_HEADER h = this->header;
    std::string error = "undefined";
    switch (this->header.rcode) {
        case dns::FORMAT_ERROR:
            error = "format error";
            break;
        case dns::SERVER_FAILURE:
            error = "server failure";
            break;
        case dns::NAME_ERROR:
            error = "name error";
            break;
        case dns::NOT_IMPLEMENTED:
            error = "not implemented";
            break;
        case dns::REFUSED:
            error = "refused";
            break;
        case dns::PARSING_ERROR:
            error = "parsing error";
            break;
        case dns::NON_ERROR:
            error = "no error";
            break;
    };
    std::cout << "id = " << ntohs(h.id)
        << "\nrd = " << int(h.rd)
        << "\ntc = " << int(h.tc)
        << "\naa = " << int(h.aa)
        << "\nopcode = " << int(h.opcode)
        << "\nqr = " << int(h.qr)
        << "\ncd = " << int(h.cd)
        << "\nad = " << int(h.ad)
        << "\nz = " << int(h.z)
        << "\nra = " << int(h.ra)
        << "\nq_count = " << ntohs(h.q_count)
        << "\nans_count = " << ntohs(h.ans_count)
        << "\nauth_count = " << ntohs(h.auth_count)
        << "\nadd_count = " << ntohs(h.add_count)
        << "\nrcode = " << error
        << '\n';
}
void dns_packet::print_questions() const {
    for (auto q : this->questions) {
        std::cout << q.name << '\n';
    }
}
void dns_packet::print_responses() const {
    for (auto r : this->responses) {
        std::cout << r.name << ": ";
        if (r.resource.type == htons(dns::A)) {
            in_addr a;
            a.s_addr = *(uint32_t*)r.data.data();
            std::cout << inet_ntoa(a);
        } else if (r.resource.type == htons(dns::AAAA)) {
            struct sockaddr_in6 sa;
            sa.sin6_family = AF_INET6;
            char str[INET6_ADDRSTRLEN];
            if (r.data.length() != 16) {
                std::cout << "invalid";
            } else {
                std::copy(r.data.begin(),r.data.end(),sa.sin6_addr.s6_addr);
                const char* ret = inet_ntop(AF_INET6,&(sa.sin6_addr),str,INET6_ADDRSTRLEN);
                if (!ret)
                    std::cout << "invalid";
                else 
                    std::cout << str;
            }
        }
        std::cout << '\n';
    }
}
void dns_packet::add_question(std::string name, dns::qtype type) {
    dns::QUERY q;
    size_t len = name.length();

    if (name[len-1] != '.')
        q.name = name+'.';
    else
        q.name = name;
    q.ques.qtype = htons(uint16_t(type)); // A records
    q.ques.qclass = htons(1); // internet
    this->questions.push_back(q);
    this->header.q_count = htons(this->questions.size());
}
void dns_packet::add_response(uint32_t data,dns::qtype type, unsigned int ttl, uint16_t q_id) {
    std::string d((char*)&data,sizeof(uint32_t));
    this->add_response(d,type,ttl, q_id);
}
void dns_packet::add_response(std::string data,dns::qtype type, unsigned int ttl, uint16_t i) {
    if (this->header.rcode != dns::NON_ERROR) return;
    this->header.rcode = dns::SERVER_FAILURE;
    if (i >= this->questions.size() || i < 0) {
        return;
    }
    dns::RES_RECORD r;
    dns::QUERY q = this->questions[i];
    r.name = q.name;
    if (q.ques.qtype != htons(uint16_t(dns::ANY)) && q.ques.qtype != htons(uint16_t(type))) {
        return;
    }
    switch (type) {
        case dns::AAAA:
             if (data.length() != 16)
                 return;
            break;
        case dns::A:
             if (data.length() != 4)
                 return;
            break;
        default:
            return;
    };
    r.resource.type = htons(uint16_t(type));
    r.resource._class = htons(1);
    r.resource.ttl = ttl;
    r.resource.data_len = htons(data.length());
    r.data = data;
    this->responses.push_back(std::move(r));
    this->header.ans_count = htons(this->responses.size());
    this->header.rcode = dns::NON_ERROR;
}
void dns_packet::set_response() {
    this->header.qr = 1;
    // TODO remove these when support for auth and add resources are added
    this->header.auth_count = 0;
    this->header.add_count = 0;
}
/* internal static, convert dns domain into ascii form
 * */
std::string dns_packet::qname_to_ascii(const std::string& qname) {
    std::string ret;
    int pos = 0;
    int n = qname[pos];
    while (n) {
        pos++;
        ret.append(qname.substr(pos,n));
        ret.append(1,'.');
        pos += n;
        if (pos > qname.length()) return ret;
        n = qname[pos];
    }
    return ret;
}
/* internal, parse a response from bytes and add it to the responses
 * returns the new pos depending on consumed bytes
 * returns -1 with  an error
 * */
int dns_packet::parse_response(const std::string& bytes,int pos,int len,rtype type) {
    dns::RES_RECORD r;
    if (pos + 2 + sizeof(dns::R_DATA) > len)
        return -1;
    unsigned short pname = ntohs(*(unsigned short*)(bytes.substr(pos,2).data()));
    if (pname & 0xc000) { // pointer format
        pname &= 0x3fff; // remove the flag bits
        if (pname > pos) return -1;
        size_t null = bytes.find('\0',pname);
        if (null == std::string::npos || null-pname > 255) return -1;
        r.name = qname_to_ascii(bytes.substr(pname,null-pname));
        pos += 2;
    } else {    // label format
        size_t null = bytes.find('\0',pos);
        if (null == std::string::npos || null-pos > 255) return -1;
        r.name = qname_to_ascii(bytes.substr(pos,null-pos));
        pos = null+1;
    }
    if (pos + sizeof(dns::R_DATA) > len) return -1;
    r.resource = *(dns::R_DATA*)(bytes.substr(pos,sizeof(dns::R_DATA)).data());
    pos += sizeof(dns::R_DATA);
    int data_len = ntohs(r.resource.data_len);
    if (pos + data_len > len) return -1;
    r.data = bytes.substr(pos,data_len);
    pos += data_len;
    if (type == RESPONSE) {
        this->responses.push_back(std::move(r));
    } else if (type == AUTHORITY) {
        this->authorities.push_back(std::move(r));
    } else {
        this->additionals.push_back(std::move(r));
    }
    return pos;
}
/* internal static, convert ascii domain into dns form
 * */
std::string dns_packet::ascii_to_qname(const std::string& domain) {
    std::string ret;
    size_t pos = 0,dot;
    do {
        dot = domain.find('.',pos);
        size_t len = dot == std::string::npos ? std::string::npos : dot-pos;
        std::string temp = domain.substr(pos,len);
        ret.append(1,char(temp.length()));
        ret.append(temp);
        pos += temp.length()+1;
    } while (dot != std::string::npos);

    // only append a null, if there was a no dot in the end
    dot = domain.rfind('.');
    if (dot != domain.length()-1)
        ret.append(1,'\0');
    return std::move(ret);
}
/* make bytes out of a dns_packet object
 * */
std::string dns_packet::str() {
    std::string ret;
    ret.append((char*)&(this->header),sizeof(this->header));
    for (const auto& q : this->questions) {
        ret.append(ascii_to_qname(q.name));
        ret.append((char*)&(q.ques),sizeof(q.ques));
    }
    for (const auto& r : this->responses) {
        ret.append(ascii_to_qname(r.name));
        ret.append((char*)&(r.resource),sizeof(r.resource));
        ret.append(r.data);
    }
    for (const auto& r : this->authorities) {
        ret.append(ascii_to_qname(r.name));
        ret.append((char*)&(r.resource),sizeof(r.resource));
        ret.append(r.data);
    }
    for (const auto& r : this->additionals) {
        ret.append(ascii_to_qname(r.name));
        ret.append((char*)&(r.resource),sizeof(r.resource));
        ret.append(r.data);
    }
    return std::move(ret);
}
unsigned char dns_packet::rcode() const {
    return header.rcode;
}
