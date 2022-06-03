#include "listen.h"

bool isLogged = true;

void listening(int port)
{    
    sockaddr_in address{};                                              // create and set sockaddr_in  
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);     
    int listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   // create socket for listen

    printLog("listening socket create FINE");

    bind(listening_socket, (sockaddr *)&address, sizeof(address));      // bind port

    printLog("listening socket bind FINE");

    listen(listening_socket, SOMAXCONN);                                // listen

    printLog("listen FINE");

    sockaddr_in addr;                                                   // sockaddr_in for connected socket
    socklen_t len;                                                      // socklen for connected socket
    std::cout << "MyNC: Start listening " << port << std::endl;
    int sock_fd = 0;                                                    // sock of connected socket

    while (true)
    {
        sock_fd = accept(listening_socket, (sockaddr *)&addr, &len);    // accept connected socket
        std::cout << "Connected from " << inet_ntoa(address.sin_addr) << std::endl;

        if (sock_fd != 0)
        {
            pid_t pidId = fork();
            if (!pidId)
            {
                while (true)
                {
                    pid_t priem = fork();
                    if (!priem)
                    {
                        std::string msg;
                        while (true)
                        {
                            std::cin >> msg;
                            char message[1024];
                            strcpy(message, msg.c_str());
                            if (send(sock_fd, (char *)&message, sizeof(message), 0) > 0)
                            {
                                printLog("sended");
                            }
                        }
                    }
                    else
                    {
                        while (true)
                        {
                            char message[1024];
                            if (recv(sock_fd, (char *)&message, sizeof(message), 0) > 0)
                            {
                                printLog("received");
                                std::cout << message << std::endl;
                            }
                            else
                            {
                                printLog("not received");
                                close(sock_fd);

                            }
                        }
                    }
                }
            }
        }
    }
}

void printLog(std::string data){
    if(isLogged){
        std::cout << "\t\tLOG: " << data << std::endl;
    }
}