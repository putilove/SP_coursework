#include "connect.h"

void connect(std::string ip, int port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    int s_client = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(s_client, (sockaddr *)&addr, sizeof(addr)) == 0)
    {
        std::cout << "CONNECT" << std::endl;
        auto pidId = fork();
        if(!pidId){
            std::string msg;
            while(true){
                std::cin >> msg;
                char message[1024];
                strcpy(message, msg.c_str());
                if(send(s_client, (char*)&message, sizeof(message), 0) > 0){
                    //std::cout << "Sended" << std::endl;
                }
            }
        }
        else{
            while(true){
                char message[1024];
                int sizeOfRecv = recv(s_client, (char*)&message, sizeof(message), 0);
                if(sizeOfRecv > 0){
                    std::cout << message << std::endl;
                }
            }
        }
        
    }
    else
    {
        std::cout << "NOT CONNECT" << std::endl;
    }
}

int main(){
    connect("127.0.0.1", 60012);
}