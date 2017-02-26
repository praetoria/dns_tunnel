#ifndef __DNS_H
#define __DNS_H
#include <vector>
#include <string>
#include "vec.h"

namespace dns {
    // this is used when crerating a new dns_packet object
    enum dns_type { query_t, response_t, none };
    enum qtype : uint16_t { A = 1, NS = 2, CNAME = 5, SOA = 6,
        WKS = 11, PTR = 12, MX = 15, SRV = 33,
        AAAA = 28, ANY = 255 };
    enum rcode : uint8_t { FORMAT_ERROR = 1, SERVER_FAILURE = 2, NAME_ERROR = 3,
        NOT_IMPLEMENTED = 4, REFUSED = 5, NON_ERROR = 0, PARSING_ERROR = 8 };
    //DNS header structure
    typedef struct DNS_HEADER
    {
        unsigned short id; // identification number

        unsigned char rd :1; // recursion desired
        unsigned char tc :1; // truncated message
        unsigned char aa :1; // authoritive answer
        unsigned char opcode :4; // purpose of message
        unsigned char qr :1; // query/response flag

        unsigned char rcode :4; // response code
        unsigned char cd :1; // checking disabled
        unsigned char ad :1; // authenticated data
        unsigned char z :1; // its z! reserved
        unsigned char ra :1; // recursion available

        unsigned short q_count; // number of question entries
        unsigned short ans_count; // number of answer entries
        unsigned short auth_count; // number of authority entries
        unsigned short add_count; // number of resource entries
    } DNS_HEADER;


    //Constant sized fields of query structure
    typedef struct QUESTION
    {
        unsigned short qtype;
        unsigned short qclass;
    } QUESTION;

    //Constant sized fields of the resource record structure
#pragma pack(push, 1)
    typedef struct R_DATA
    {
        unsigned short type;
        unsigned short _class;
        unsigned int ttl;
        unsigned short data_len;
    } R_DATA;
#pragma pack(pop)

    //Structure of a response record
    //the name is not in dns format, just ascii
    typedef struct RES_RECORD
    {
        std::string name;
        struct R_DATA resource;
        std::string data;
        RES_RECORD(const RES_RECORD& obj) {
            name = obj.name;
            resource = obj.resource;
            data = obj.data;
        }
        RES_RECORD() {};
    } RES_RECORD;

    //Structure of a Query
    //the name is not in dns format, just ascii
    typedef struct
    {
        std::string name;
        struct QUESTION ques;
    } QUERY;
    
}

// for comparing the DNS_HEADER
bool operator==(const dns::DNS_HEADER& lhs, const dns::DNS_HEADER& rhs);
bool operator!=(const dns::DNS_HEADER& lhs, const dns::DNS_HEADER& rhs);
bool operator==(const dns::QUERY& lhs, const dns::QUERY& rhs);
bool operator!=(const dns::QUERY& lhs, const dns::QUERY& rhs);
bool operator!=(const dns::RES_RECORD& lhs, const dns::RES_RECORD& rhs);
bool operator==(const dns::RES_RECORD& lhs, const dns::RES_RECORD& rhs);

/* the actual dns_packet class */
class dns_packet {
    static std::string ascii_to_qname(const std::string&);
    static std::string qname_to_ascii(const std::string&);
    enum rtype { RESPONSE, AUTHORITY, ADDITIONAL };
    int parse_response(const std::string&,int,int,rtype);
    protected:
        dns::DNS_HEADER header;
    public:
        // dns_type has effect on only the query flag for the header,
        // query flag defaults to 0, with dns::none
        dns_packet(unsigned short id, dns::dns_type type = dns::none);
        // this creates a dns_packet from bytes
        dns_packet(const std::string&);
        dns_packet(const dns_packet&);

        vec<dns::QUERY> questions;
        vec<dns::RES_RECORD> responses;
        vec<dns::RES_RECORD> authorities;
        vec<dns::RES_RECORD> additionals;

        void add_question(std::string name, dns::qtype type = dns::ANY);
        void add_response(uint32_t data,dns::qtype type,unsigned int ttl = 0, uint16_t q_id = 0);
        void add_response(std::string data,dns::qtype type,unsigned int ttl = 0, uint16_t q_id = 0);
        void print_header() const;
        void print_questions() const;
        void print_responses() const;
        unsigned char rcode() const;
        // sets the response flag
        void set_response();
        // returns the query in byte form
        std::string str();
};
#endif
