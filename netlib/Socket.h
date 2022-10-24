#pragma once

#include <stdint.h>
#include "Network.h"
#include "Address.h"


/* -------------------------------------------------------------------------- */
/*                              PLATFORM DEFINES                              */
/* -------------------------------------------------------------------------- */

#ifdef _WIN32
    #include <WS2tcpip.h>
    #define GET_SOCKET_ERROR() WSAGetLastError()
    typedef SOCKET SocketDescriptor;
    typedef int SocketLength;
#else
    #define GET_SOCKET_ERROR() errno
    static const int SOCKET_ERROR = -1;
    
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <cerrno>
    
    typedef int SocketDescriptor;
    static const int INVALID_SOCKET = ~0;
    typedef socklen_t SocketLength;
    
    #define closesocket close
#endif

/* -------------------------------------------------------------------------- */


namespace Net
{

    enum class Protocol : uint8_t
    {
        Unsupported,
        UDP,
        TCP,
    };

    class Socket
    {
        private:
            Address m_address;
            Protocol m_protocol;
            SocketDescriptor m_descriptor = INVALID_SOCKET;
        
        public:
            Socket() = default;
            Socket(const IPVersion ipVersion, const Protocol protocol);
            Socket(const char* host, unsigned short port, const Protocol protocol);
            Socket(
                const Address& m_address,
                Protocol m_protocol,
                const SocketDescriptor m_descriptor
            );
        
        public:
            bool createSocket();
            bool bindSocket();
            bool setPort();
            bool closeSocket();
        
        public:
            SocketDescriptor descriptor() const { return m_descriptor; }
            unsigned short port() const { return m_address.getPort(); }
            
            bool listenSocket(const int backlog = 5);
            bool acceptConnection(Socket& newSocket);
            bool connectToAddress(const Address& address);
            
            bool sendDataTo(const Address& address, const char* data, int dataSize, int& bytesSent);
            bool sendData(const char* data, int dataSize, int& bytesSent);
            bool receiveDataFrom(Address& address, char* buf, int bufSize, int& bytesReceived);
            bool receiveData(char* buf, int bufSize, int& bytesReceived);
    };

} // namespace Net

