#include <stdio.h>

#include <Socket.h>


int main(int argc, const char** argv)
{
    printf("----------------------- ECHO UDP SERVER START -----------------------\n");
    
    Net::Socket serverSocket("0.0.0.0", 5555, Net::Protocol::UDP);
    
    if (serverSocket.descriptor() == INVALID_SOCKET)
        return -1;
    
    printf("Echo UDP server started on %d port...\n", serverSocket.port());
    
    char datagram[256];
    
    while (true)
    {
        memset(datagram, 0, 256);
        Net::Address clientAddress;
        int receivedBytes = 0;
        if (serverSocket.receiveDataFrom(clientAddress, datagram, sizeof(datagram), receivedBytes) == false)
            break;
        
        if (receivedBytes > 0)
        {
            printf("%s [%s:%d] sent datagram (%d bytes): '%s'\n",
                clientAddress.getHostName().c_str(),
                clientAddress.getIPString().c_str(),
                clientAddress.getPort(),
                receivedBytes,
                datagram
            );
            
            int bytesSent = 0;
            serverSocket.sendDataTo(
                clientAddress,
                datagram,
                receivedBytes,
                bytesSent
            );
            
            if (strcmp(datagram, "exit") == 0)
            {
                printf("Received 'exit' command. Shutting down...\n");
                break;
            }
        }
    }
    
    serverSocket.closeSocket();
    
    printf("----------------------- ECHO UDP SERVER STOP -----------------------\n");
    return 0;
}