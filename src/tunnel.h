#ifndef _TUNNEL_H
#define _TUNNEL_H
#include <string>

class tunnel {
    public:
    enum tunnel_type { INCOMING, OUTGOING };
    virtual tunnel& operator<<(const std::string&) = 0;
    virtual tunnel& operator>>(std::string&) = 0;
    protected:
    tunnel_type t_type;
    tunnel(tunnel_type t_type) : t_type(t_type)  {};
};
#endif
