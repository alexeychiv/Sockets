#include "Network.h"

#include "Logger.h"


#ifdef _WIN32

    #include <WS2tcpip.h>
    #include <string>
    #include <cstring>
    
    namespace Net
    {
    
        class Network
        {
            private:
                bool m_isStarted = false;
            
            private:
                Network()
                {
                    WSADATA wsaData;
                    WORD version = MAKEWORD(2, 2);
                    int result = WSAStartup(version,&wsaData);
                    if (result != 0)
                    {
                        LOGE("NETWORK ERROR: failed to startup winsock API! Error code %d", WSAGetLastError());
                        return;
                    }
                    
                    if (LOBYTE(wsaData.wVersion) != LOBYTE(version) || HIBYTE(wsaData.wVersion) != HIBYTE(version))
                    {
                        LOGE("NETWORK ERROR: winsock API dll was not found! Version %d.%d expected (%d.%d provided)",
                            HIBYTE(version), LOBYTE(version),
                            HIBYTE(wsaData.wVersion), LOBYTE(wsaData.wVersion)
                        );
                        return;
                    }
                    
                    LOGI("NETWORK: Winsock API started successfully!");
                    m_isStarted = true;
                }
                ~Network()
                {
                    if (WSACleanup() == SOCKET_ERROR)
                    {
                        std::string errorStr(256, '\0');
                        #if (_POSIX_C_SOURCE >= 200112L) && ! _GNU_SOURCE
                                ::strerror_r(get_last_error_code(), &buffer[0], buffer.size());
                                return buffer;
                        #else
                                errorStr = ::strerror(WSAGetLastError());
                        #endif
                        
                        LOGE("NETWORK ERROR: failed to cleanup WSA! Error code %d (%s)", WSAGetLastError(), errorStr.c_str());
                        return;
                    }
                    LOGI("NETWORK: WSA cleaned up!");
                    m_isStarted = false;
                }
                static Network& instance()
                {
                    static Network networkInstance;
                    return networkInstance;
                }
            
            public:
                static bool isStarted()
                {
                    return instance().m_isStarted;
                }
        };
    
    } // namespace Net

#endif

namespace Net
{

    bool isNetStarted()
    {
#ifdef _WIN32
        return Network::isStarted();
#else
        return true;
#endif
    }

} // namespace Net

