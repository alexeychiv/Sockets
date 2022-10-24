#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
    #include <WS2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
#endif


namespace Net
{

    enum class IPVersion : uint8_t
    {
        Unsupported,
        IPv4,
        IPv6,
    };

    class Address
    {
        static const char* versionToStr(const IPVersion version);
        
        private:
            IPVersion m_ipVersion = IPVersion::Unsupported;
            std::string m_hostName = "";
            std::string m_ipString;
            std::vector<uint8_t> m_ipBytes;
            
            unsigned short m_port = 0;
        
        public:
            Address() = default;
            Address(const IPVersion ipVersion);
            Address(const char* addressStr, unsigned short port);
            Address(sockaddr* addr);
        
        public:
            void setPort(unsigned short port) { m_port = port; }
            
            IPVersion getIPVersion() const { return m_ipVersion; }
            std::vector<uint8_t> getIPBytes() const { return m_ipBytes; }
            std::string getHostName() const { return m_hostName; }
            std::string getIPString() const { return m_ipString; }
            unsigned short getPort() const { return m_port; }
            
            sockaddr_in getSockaddrIPv4() const;
            sockaddr_in6 getSockaddrIPv6() const;
            int sockaddrLen() const;
            
            std::string str() const;
    };

} // namespace Net