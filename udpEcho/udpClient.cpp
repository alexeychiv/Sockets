#include <stdio.h>

#include <Socket.h>


int main(int argc, char const *argv[])
{
    printf("----------------------- UDP CLIENT START -----------------------\n");
    
    Net::Socket clientSocket(Net::IPVersion::IPv4, Net::Protocol::UDP);
    
    if (clientSocket.descriptor() == INVALID_SOCKET)
        return -1;
    
    //Sending datagrams
    while (true)
    {
        char message[1024];
        scanf("%s", message);
        size_t messageLen = strlen(message) + 1;
        
        Net::Address serverAddress("127.0.0.1", 5555);
        int sentBytes = 0;
        if (clientSocket.sendDataTo(serverAddress, message, messageLen, sentBytes) == false)
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
    
    clientSocket.closeSocket();
    
    printf("----------------------- UDP CLIENT STOP -----------------------\n");
    return 0;
}
