#include "Socket.h"

#include "Logger.h"


namespace Net
{

    Socket::Socket(const IPVersion ipVersion, const Protocol protocol)
        :
            m_address(ipVersion),
            m_protocol(protocol),
            m_descriptor(INVALID_SOCKET)
    {
        if (createSocket() == false)
            return;
    }

    Socket::Socket(const char* host, unsigned short port, const Protocol protocol)
        :
            m_address(host, port),
            m_protocol(protocol),
            m_descriptor(INVALID_SOCKET)
    {
        if (createSocket() == false)
            return;
        
        if (bindSocket() == false)
            return;
    }

    Socket::Socket(
        const Address& address,
        Protocol protocol,
        const SocketDescriptor descriptor
    )
        :
            m_address(address),
            m_protocol(protocol),
            m_descriptor(descriptor)
    {}

    bool Socket::closeSocket()
    {
        if (closesocket(m_descriptor) != 0)
        {
            LOGE("Unable to close %d socket! Error code %d", m_descriptor, GET_SOCKET_ERROR());
            return false;
        }
        LOGI("Socket %d closed!", m_descriptor);
        return true;
    }

    bool Socket::createSocket()
    {
        m_descriptor = INVALID_SOCKET;
        
        if (isNetStarted() == false)
        {
            LOGE("Unable to create socket - network is not started!");
            return false;
        }
        
        int sockAddressFamily = AF_UNSPEC;
        switch(m_address.getIPVersion())
        {
            case IPVersion::IPv4:
                sockAddressFamily = AF_INET;
                break;
            case IPVersion::IPv6:
                sockAddressFamily = AF_INET6;
                break;
            case IPVersion::Unsupported:
                {
                    LOGE("Unable to create socket - unsupported IP version!");
                    return false;
                }
        }
        
        int sockType = AF_UNSPEC;
        int sockProtocol = SOCK_RAW;
        switch(m_protocol)
        {
            case Protocol::UDP:
                sockType = SOCK_DGRAM;
                sockProtocol = IPPROTO_UDP;
                break;
            case Protocol::TCP:
                sockType = SOCK_STREAM;
                sockProtocol = IPPROTO_TCP;
                break;
            case Protocol::Unsupported:
                {
                    LOGE("Unable to create socket - unsupported protocol!");
                    return false;
                }
        }
        
        m_descriptor = socket(sockAddressFamily, sockType, sockProtocol);
        
        if (m_descriptor == INVALID_SOCKET)
        {
            LOGE("Failed to create socket! Error code %d", GET_SOCKET_ERROR());
            return false;
        }
        
        LOGI("Socket %d created!", m_descriptor);
        return true;
    }

    bool Socket::bindSocket()
    {
        int result = SOCKET_ERROR;
        switch (m_address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                sockaddr_in addr = m_address.getSockaddrIPv4();
                result = bind(m_descriptor, (sockaddr*)&addr, sizeof(sockaddr_in));
                break;
            }
            case IPVersion::IPv6:
            {
                sockaddr_in6 addr = m_address.getSockaddrIPv6();
                result = bind(m_descriptor, (sockaddr*)&addr, sizeof(sockaddr_in6));
                break;
            }
            case IPVersion::Unsupported:
            {
                LOGE("Unable to bind socket %d to address: %s! Unsupported IP version!",
                    m_descriptor,
                    m_address.str().c_str(),
                    GET_SOCKET_ERROR()
                );
                return false;
            }
        }
        
        if (result == SOCKET_ERROR)
        {
            LOGE("Unable to bind socket %d to address: %s! Error code %d",
                m_descriptor,
                m_address.str().c_str(),
                GET_SOCKET_ERROR()
            );
            return false;
        }
        
        if (m_address.getPort() == 0 && setPort() == false)
        {
            LOGE("Unable to bind socket %d to address: %s! Port error!",
                m_descriptor,
                m_address.str().c_str()
            );
            return false;
        }
        
        LOGI("Socket %d bound! Address: %s", m_descriptor, m_address.str().c_str());
        
        return true;
    }

    bool Socket::setPort()
    {
        switch(m_address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                sockaddr_in addr;
                SocketLength addrLen = sizeof(addr);
                if (getsockname(m_descriptor, (sockaddr*)&addr, &addrLen) == SOCKET_ERROR)
                {
                    LOGE("Unable to get socket name for %d socket! Error code %d", m_descriptor, GET_SOCKET_ERROR());
                    return false;
                }
                m_address.setPort(ntohs(addr.sin_port));
                break;
            }
            case IPVersion::IPv6:
            {
                sockaddr_in6 addr;
                SocketLength addrLen = sizeof(addr);
                if (getsockname(m_descriptor, (sockaddr*)&addr, &addrLen) == SOCKET_ERROR)
                {
                    LOGE("Unable to get socket name for %d socket! Error code %d", m_descriptor, GET_SOCKET_ERROR());
                    return false;
                }
                m_address.setPort(ntohs(addr.sin6_port));
                break;
            }
            case IPVersion::Unsupported:
                LOGE("Unable to set port for %d socket! Unsupported IP version!", m_descriptor);
                return false;
        }
        return true;
    }

    bool Socket::listenSocket(const int backlog)
    {
        int result = listen(m_descriptor, backlog);
        if (listen(m_descriptor, backlog) == SOCKET_ERROR)
        {
            LOGE("Listen socket %d error! Error code %d", m_descriptor, GET_SOCKET_ERROR());
            return false;
        }
        return true;
    }

    bool Socket::acceptConnection(Socket& newSocket)
    {
        switch(m_address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                sockaddr_in addr;
                SocketLength len = sizeof(addr);
                SocketDescriptor acceptedConnectionDescriptor = accept(
                    m_descriptor,
                    (sockaddr*)&addr,
                    &len
                );
                if (acceptedConnectionDescriptor == INVALID_SOCKET)
                {
                    LOGE("Unable to accept connection on socket %d! Error code %d", m_descriptor, GET_SOCKET_ERROR());
                    return false;
                }
                Address acceptedAddress((sockaddr*)&addr);
                LOGI("Socket %d accepted connection from address: %s", m_descriptor, acceptedAddress.str().c_str());
                newSocket = Socket(acceptedAddress, m_protocol, acceptedConnectionDescriptor);
                break;
            }
            case IPVersion::IPv6:
            {
                sockaddr_in6 addr;
                SocketLength len = sizeof(addr);
                SocketDescriptor acceptedConnectionDescriptor = accept(
                    m_descriptor,
                    (sockaddr*)&addr,
                    &len
                );
                if (acceptedConnectionDescriptor == INVALID_SOCKET)
                {
                    LOGE("Unable to accept connection on socket %d! Error code %d", m_descriptor, GET_SOCKET_ERROR());
                    return false;
                }
                Address acceptedAddress((sockaddr*)&addr);
                LOGI("Socket %d accepted connection from address: %s", m_descriptor, acceptedAddress.str().c_str());
                newSocket = Socket(acceptedAddress, m_protocol, acceptedConnectionDescriptor);
                break;
            }
            case IPVersion::Unsupported:
                LOGE("Unable to accept connection on %d socket! Unsupported IP version!", m_descriptor);
                return false;
        }
        return true;
    }

    bool Socket::connectToAddress(const Address& address)
    {
        int result = 0;
        
        switch (address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                sockaddr_in addr4 = address.getSockaddrIPv4();
                result = connect(m_descriptor, (sockaddr*)&addr4, sizeof(sockaddr_in));
                break;
            }
            case IPVersion::IPv6:
            {
                sockaddr_in6 addr6 = address.getSockaddrIPv6();
                result = connect(m_descriptor, (sockaddr*)&addr6, sizeof(sockaddr_in6));
                break;
            }
            case IPVersion::Unsupported:
                LOGE("Unable to connect socket %d to address with unsupported IP version: %s",
                    m_descriptor,
                    address.str().c_str()
                );
        }
        if (result == SOCKET_ERROR)
        {
            LOGE("Unable to connect socket %d to address: %s. Error code %d",
                    m_descriptor,
                    address.str().c_str(),
                    GET_SOCKET_ERROR()
                );
            return false;
        }
        LOGI("Socket %d connected to address: %s", m_descriptor, address.str().c_str());
        return true;
    }

    bool Socket::sendDataTo(const Address& address, const char* data, int dataSize, int& bytesSent)
    {
        switch (address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                sockaddr_in addr = address.getSockaddrIPv4();
                bytesSent = sendto(m_descriptor, data, dataSize, 0, (sockaddr*)&addr, sizeof(addr));
                break;
            }
            case IPVersion::IPv6:
            {
                sockaddr_in6 addr = address.getSockaddrIPv6();
                bytesSent = sendto(m_descriptor, data, dataSize, 0, (sockaddr*)&addr, sizeof(addr));
                break;
            }
            case IPVersion::Unsupported:
                LOGE("Unable to send data from socket %d to address: %s! IP version is unsupported!\n",
                    m_descriptor, address.str().c_str()
                );
                return false;
        }
        
        if (bytesSent == SOCKET_ERROR)
        {
            LOGE("Unable to send data from socket %d to address: %s! Error code %d\n",
                m_descriptor, address.str().c_str(), GET_SOCKET_ERROR()
            );
            return false;
        }
        return true;
    }

    bool Socket::sendData(const char* data, int dataSize, int& bytesSent)
    {
        switch (m_address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                bytesSent = send(m_descriptor, data, dataSize, 0);
                break;
            }
            case IPVersion::IPv6:
            {
                bytesSent = send(m_descriptor, data, dataSize, 0);
                break;
            }
            case IPVersion::Unsupported:
                LOGE("Unable to send data from socket %d! IP version is unsupported!\n",
                    m_descriptor
                );
                return false;
        }
        
        if (bytesSent == SOCKET_ERROR)
        {
            LOGE("Unable to send data from socket %d! Error code %d\n",
                m_descriptor, GET_SOCKET_ERROR()
            );
            return false;
        }
        return true;
    }

    bool Socket::receiveDataFrom(Address& address, char* buf, int bufSize, int& bytesReceived)
    {
        switch (m_address.getIPVersion())
        {
            case IPVersion::IPv4:
            {
                sockaddr_in addr;
                SocketLength addrLen = sizeof(addr);
                bytesReceived = recvfrom(m_descriptor, buf, bufSize, 0, (sockaddr*)&addr, &addrLen);
                address = Address((sockaddr*)&addr);
                break;
            }
            case IPVersion::IPv6:
            {
                sockaddr_in6 addr;
                SocketLength addrLen = sizeof(addr);
                bytesReceived = recvfrom(m_descriptor, buf, bufSize, 0, (sockaddr*)&addr, &addrLen);
                address = Address((sockaddr*)&addr);
                break;
            }
            case IPVersion::Unsupported:
                LOGE("Unable to receive data from socket %d! IP version is unsupported!\n",
                    m_descriptor
                );
                return false;
        }
        
        if (bytesReceived == SOCKET_ERROR)
        {
            LOGE("Unable to receive data from socket %d from address: %s! Error code %d\n",
                m_descriptor, address.str().c_str(), GET_SOCKET_ERROR()
            );
            return false;
        }
        return true;
    }

    bool Socket::receiveData(char* buf, int bufSize, int& bytesReceived)
    {
        bytesReceived = recv(m_descriptor, (char*)buf, bufSize, 0);
        
        if (bytesReceived == 0)
            return false;
        
        if (bytesReceived == SOCKET_ERROR)
        {
            LOGE("Unable to receive data from socket %d! Error code %d\n", m_descriptor);
            return false;
        }
        
        return true;
    }

} // namespace Net





