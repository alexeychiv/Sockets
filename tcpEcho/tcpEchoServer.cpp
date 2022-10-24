#include <stdio.h>

#include <Socket.h>


int main(int argc, const char** argv)
{
    printf("----------------------- ECHO TCP SERVER START -----------------------\n");
    
    Net::Socket serverSocket("0.0.0.0", 5555, Net::Protocol::TCP);
    
    if (serverSocket.descriptor() == INVALID_SOCKET)
        return -1;
    
    printf("Echo TCP server started on %d port...\n", serverSocket.port());
    
    if (serverSocket.listenSocket(5))
    {
        Net::Socket newConnection;
        if (serverSocket.acceptConnection(newConnection))
        {
            char message[256];
            
            while (true)
            {
                memset(message, 0, 256);
                int receivedBytes = 0;
                if (newConnection.receiveData(message, sizeof(message), receivedBytes) == false)
                    break;
                
                if (receivedBytes > 0)
                {
                    printf("Received message through %d socket (%d bytes): '%s'\n",
                        newConnection.descriptor(),
                        receivedBytes,
                        message
                    );
                    
                    int bytesSent = 0;
                    newConnection.sendData(
                        message,
                        receivedBytes,
                        bytesSent
                    );
                    
                    if (strcmp(message, "exit") == 0)
                    {
                        printf("Received 'exit' command. Shutting down...\n");
                        break;
                    }
                }
            }
            
            newConnection.closeSocket();
        }
    }
    
    serverSocket.closeSocket();
    
    printf("----------------------- ECHO TCP SERVER STOP -----------------------\n");
    return 0;
}