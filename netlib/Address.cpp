#include "Address.h"

#include <algorithm> 

#include "Logger.h"
#include "Network.h"


namespace Net
{

    static void trimStr(std::string &str)
    {
        str.erase(
            str.begin(),
            std::find_if(
                str.begin(),
                str.end(),
                [](int ch)
                {
                    return !(std::isspace(ch) || ch == '\0');
                }
            )
        );
        
        str.erase(
            std::find_if(
                str.rbegin(),
                str.rend(),
                [](int ch)
                {
                    return !(std::isspace(ch) || ch == '\0');
                }
            ).base(),
            str.end()
        );
    }

    const char* Address::versionToStr(const IPVersion version)
    {
        switch (version)
        {
            case IPVersion::Unsupported: return "Unsupported";
            case IPVersion::IPv4: return "IPv4";
            case IPVersion::IPv6: return "IPv6";
        }
        return "Unsupported";
    }

    Address::Address(const IPVersion ipVersion)
        :
            m_ipVersion(ipVersion),
            m_hostName("NONE"),
            m_ipString("NONE")
    {}

    Address::Address(const char* addressStr, unsigned short port)
        :
            m_port(port)
    {
        if (isNetStarted() == false)
        {
            LOGE("Cant resolve address. Network is not started.");
            return;
        }
        
        addrinfo hints =
        {
            .ai_flags = AI_CANONNAME,
            .ai_family = AF_UNSPEC,
            .ai_socktype = SOCK_STREAM,
            .ai_protocol = 0
        };
        addrinfo *servinfo = nullptr;
        int status = 0;
        if ((status = getaddrinfo(addressStr, nullptr, &hints, &servinfo)) != 0)
        {
            LOGE("getaddrinfo error: %s", gai_strerror(status));
            return;
        }
        
        for (auto const *s = servinfo; s != nullptr; s = s->ai_next)
        {
            m_hostName = s->ai_canonname;
            
            if (s->ai_family == AF_INET)
            {
                m_ipVersion = IPVersion::IPv4;
                
                sockaddr_in* addrV4 = (sockaddr_in*)s->ai_addr;
                
                m_ipString.resize(16);
                inet_ntop(AF_INET, &addrV4->sin_addr, &m_ipString[0], 16);
                
                uint32_t ipLong = addrV4->sin_addr.s_addr;
                m_ipBytes.resize(sizeof(addrV4->sin_addr));
                memcpy(&m_ipBytes[0], &ipLong, sizeof(addrV4->sin_addr));
                
                break;
            }
            else if (s->ai_family == AF_INET6)
            {
                m_ipVersion = IPVersion::IPv6;
                
                sockaddr_in6 * addrV6 = reinterpret_cast<sockaddr_in6*>(s->ai_addr);
                
                m_ipString.resize(46);
                inet_ntop(AF_INET6, &addrV6->sin6_addr, &m_ipString[0], 46);
                
                m_ipBytes.resize(sizeof(addrV6->sin6_addr));
                memcpy(&m_ipBytes[0], &addrV6->sin6_addr, sizeof(addrV6->sin6_addr));
                
                break;
            }
        }
        
        trimStr(m_hostName);
        trimStr(m_ipString);
        
        freeaddrinfo(servinfo);
        
        if (m_hostName.empty())
            LOGE("Unable to resolve '%s' host!", addressStr);
    }

    Address::Address(sockaddr* addr)
    {
        if (addr->sa_family == AF_INET)
        {
            m_ipVersion = IPVersion::IPv4;
            sockaddr_in* addrV4 = (sockaddr_in*)addr;
            m_port = ntohs(addrV4->sin_port); //network to host short
            m_ipBytes.resize(sizeof(uint32_t));
            memcpy(&m_ipBytes[0], &addrV4->sin_addr, sizeof(uint32_t));
            
            m_ipString.resize(16);
            inet_ntop(AF_INET, &addrV4->sin_addr, &m_ipString[0], 16); //network to presentation format
            m_hostName = m_ipString;
        }
        else
        {
            m_ipVersion = IPVersion::IPv6;
            sockaddr_in6* addrV6 = (sockaddr_in6*)addr;
            m_port = ntohs(addrV6->sin6_port); //network to host short
            m_ipBytes.resize(16);
            memcpy(&m_ipBytes[0], &addrV6->sin6_addr, 16);
            
            m_ipString.resize(46);
            inet_ntop(AF_INET6, &addrV6->sin6_addr, &m_ipString[0], 46);
            m_hostName = m_ipString;
        }
        
        trimStr(m_ipString);
        trimStr(m_hostName);
    }

    sockaddr_in Address::getSockaddrIPv4() const
    {
        sockaddr_in addr {};
        addr.sin_family = AF_INET;
        memcpy(&addr.sin_addr, &m_ipBytes[0], sizeof(uint32_t));
        addr.sin_port = htons(m_port);
        return addr;
    }

    sockaddr_in6 Address::getSockaddrIPv6() const
    {
        sockaddr_in6 addr6 {};
        addr6.sin6_family = AF_INET6;
        memcpy(&addr6.sin6_addr, &m_ipBytes[0], 16);
        addr6.sin6_port = htons(m_port);
        return addr6;
    }

    int Address::sockaddrLen() const
    {
        switch(m_ipVersion)
        {
            case IPVersion::IPv4: return sizeof(sockaddr_in);
            case IPVersion::IPv6: return sizeof(sockaddr_in6);
            case IPVersion::Unsupported: return 0;
        }
    }

    std::string Address::str() const
    {
        char result[512] {""};
        
        sprintf(result, "host = '%s' ipVersion = %s ip = '%s' port = %d ipBytes = { ",
            m_hostName.c_str(),
            versionToStr(m_ipVersion),
            m_ipString.c_str(),
            m_port
        );
        
        for (size_t i = 0; i < m_ipBytes.size(); ++i)
        {
            char num[5] = "";
            sprintf(num, "%d ", m_ipBytes[i]);
            strcat(result, num);
        }
        
        strcat(result, "}");
        
        return result;
    }

} // namespace Net