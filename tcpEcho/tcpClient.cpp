#include <stdio.h>

#include <Socket.h>


int main(int argc, char const *argv[])
{
    printf("----------------------- UDP CLIENT START -----------------------\n");
    
    Net::Socket clientSocket(Net::IPVersion::IPv4, Net::Protocol::TCP);
    
    if (clientSocket.descriptor() == INVALID_SOCKET)
        return -1;
    
    if (clientSocket.connectToAddress(Net::Address("127.0.0.1", 5555)))
    {
        printf("Connected to server!\n");
        
        while (true)
        {
            char message[1024];
            scanf("%s", message);
            size_t messageLen = strlen(message) + 1;
            int sentBytes = 0;
            if (clientSocket.sendData(message, messageLen, sentBytes) == false)
                break;
            
            if (sentBytes)
                printf("Message '%s' sent! Size %d bytes.\n", message, messageLen);
            
            char buf[1024];
            memset(buf, 0, 1024);
            
            int receivedBytes = 0;
            if (clientSocket.receiveData(buf, 1024, receivedBytes) == false)
                break;
            
            printf("Message received (%d bytes): '%s'\n", receivedBytes, buf);
        }
    }
    
    printf("----------------------- UDP CLIENT STOP -----------------------\n");
    return 0;
}
