#include "functions.h"

std::ifstream::pos_type filesize(std::string file_name)
{
    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
    return in.tellg();
}

void connectTCP(std::string ip, int port, bool isFileMessage)
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
        if (isFileMessage)
        {
            while (true)
            {
                std::string path;
                std::cout << "Enter file name: ";
                std::cin >> path;
                std::cout << "filename: " << path << std::endl;
                std::fstream file;
                file.open(path, std::ios_base::in | std::ios_base::binary);

                if (file.is_open())
                {
                    int file_size = filesize(path);
                    char *bytes = new char[file_size];
                    file.read((char *)bytes, file_size);

                    std::cout << "size: " << file_size << std::endl;
                    std::cout << "name: " << path << std::endl;
                    std::cout << "data: " << bytes << std::endl;

                    send(s_client, (char *)std::to_string(file_size).c_str(), 16, 0);
                    send(s_client, (char *)path.c_str(), 32, 0);
                    send(s_client, (char *)bytes, file_size, 0);
                }
                else
                {
                    std::cout << "Error opening file" << std::endl;
                }
                file.close();
            }
        }
        else
        {
            auto pidId = fork();
            if (pidId)
            {
                while (true)
                {
                    std::string msg;
                    std::cin >> msg;
                    size_t msglen = 1;
                    msglen = msg.length();
                    char message[msglen] = "\0";
                    strcpy(message, msg.c_str());
                    if (send(s_client, (char *)&message, sizeof(message), 0) > 0)
                    {
                        // std::cout << "Sended"<< std::endl;
                    }
                }
            }
            else
            {
                while (true)
                {
                    // Message message = Message();
                    char message[1024] = "\0";
                    int sizeOfRecv = recv(s_client, (char *)&message, sizeof(message), 0);
                    if (sizeOfRecv > 0)
                    {
                        std::cout << message << std::endl;
                    }
                }
            }
        }
    }
    else
    {
        std::cout << "NOT CONNECT" << std::endl;
    }
}