#include "functions.h"

std::ifstream::pos_type filesize(std::string file_name)
{
    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
    return in.tellg();
}

void listeningUDP(int srcPort, bool isFileMessage, int dstPort)
{
    int listening_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(srcPort);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(listening_socket, (sockaddr *)&address, sizeof(address));

    socklen_t len;
    std::cout << "MyNC: Start listening UDP " << srcPort << std::endl;
    int sock_fd = 0;

    while (true)
    {
        if (isFileMessage)
        {
            char file_size_str[16];
            char file_name[32];
            int file_size;
            recvfrom(listening_socket, (char *)file_size_str, 16, MSG_WAITALL, 0, &len);
            file_size = atoi(file_size_str);
            recvfrom(listening_socket, (char *)file_name, 32, MSG_WAITALL, 0, &len);
            char *bytes = new char[file_size];
            recvfrom(listening_socket, (char *)bytes, file_size, MSG_WAITALL, 0, &len);

            std::cout << "size of file: " << file_size << std::endl;
            std::cout << "name of file: " << file_name << std::endl;
            if (dstPort != -1)
            {
                int s_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                sockaddr_in dstAddress;
                memset(&dstAddress, 0, sizeof(sockaddr_in));
                dstAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                dstAddress.sin_port = htons(dstPort);
                dstAddress.sin_family = AF_INET;

                sendto(s_client, (char *)std::to_string(file_size).c_str(), 16, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
                sendto(s_client, (char *)file_name, 32, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
                sendto(s_client, (char *)bytes, file_size, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
            }
            else
            {
                std::ofstream ofs("accepted/" + std::string(file_name)); //создать
                ofs.close();
                std::fstream file;
                file.open("accepted/" + std::string(file_name), std::ios_base::out | std::ios_base::binary);
                if (file.is_open())
                {
                    std::cout << "data: " << bytes << std::endl;
                    file.write(bytes, file_size);
                    std::cout << "ok.save" << std::endl;
                }
            }
        }
        else
        {
            char message[1024] = "\0";
            socklen_t len;
            int sizeOfRecv = recvfrom(listening_socket, (char *)message, 1024, MSG_WAITALL, 0, &len);
            if (sizeOfRecv > 0)
            {
                if (dstPort != -1)
                {
                    int s_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                    sockaddr_in dstAddress;
                    memset(&dstAddress, 0, sizeof(sockaddr_in));
                    dstAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                    dstAddress.sin_port = htons(dstPort);
                    dstAddress.sin_family = AF_INET;

                    sendto(s_client, (char *)message, sizeOfRecv, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
                }
                else
                {
                    std::cout << message << std::endl;
                }
            }
        }
    }
}

void connectUDP(std::string ip, int port, bool isFileMessage)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    int s_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    while (true)
    {
        if (isFileMessage)
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

                sendto(s_client, (char *)std::to_string(file_size).c_str(), 16, 0, (const sockaddr *)&addr, sizeof(addr));
                sendto(s_client, (char *)path.c_str(), 32, 0, (const sockaddr *)&addr, sizeof(addr));
                sendto(s_client, (char *)bytes, file_size, 0, (const sockaddr *)&addr, sizeof(addr));
            }
            else
            {
                std::cout << "Error opening file" << std::endl;
            }
            file.close();
        }
        else
        {
            std::string msg;
            std::cin >> msg;
            size_t msglen = msg.length();
            char message[msglen] = "\0";
            strcpy(message, msg.c_str());
            sendto(s_client, (char *)message, msglen, 0, (const sockaddr *)&addr, sizeof(addr));
        }
    }
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