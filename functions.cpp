#include "functions.h"

std::ifstream::pos_type filesize(std::string file_name)
{
    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
    return in.tellg();
}

#pragma region scanner

bool port_is_open(std::string ip, int port)
{
    struct sockaddr_in addr_s;
    const char *addr = ip.c_str();

    short int fd = -1;
    fd_set fdset;
    struct timeval tv;
    int rc;
    int so_error;
    socklen_t len;
    struct timespec tstart = {0, 0}, tend = {0, 0};
    int seconds = 1;

    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr(addr);
    addr_s.sin_port = htons(port);

    clock_gettime(CLOCK_MONOTONIC, &tstart);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    rc = connect(fd, (struct sockaddr *)&addr_s, sizeof(addr_s));

    if ((rc == -1) && (errno != EINPROGRESS))
    {
        close(fd);
        return false;
    }

    if (rc == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &tend);
        close(fd);
        return true;
    }

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    rc = select(fd + 1, NULL, &fdset, NULL, &tv);

    switch (rc)
    {
    case 1:
        len = sizeof(so_error);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0)
        { // success
            clock_gettime(CLOCK_MONOTONIC, &tend);
            close(fd);
            return true;
        }
        else
        { // error
            clock_gettime(CLOCK_MONOTONIC, &tend);
        }
        break;
    case 0: // timeout
        break;
    }

    close(fd);
    return false;
}

void task(std::string ip, int port)
{
    if (port_is_open(ip, port))
    {
        std::cout << "MyNC: connection to " 
        << ip << " port " 
        << port << " succeeded.\n";
    }
}

void scanPorts(std::string ip, int portInit, int portEnd)
{
    std::cout << "MyNC: Port scanning for IP " 
                << ip << " ports: " 
                << portInit << "-" 
                << portEnd << std::endl;
    std::vector<std::thread *> tasks;

    for (int i = portInit; i < portEnd; i++)
    {
        tasks.push_back(new std::thread(task, ip, i));
    }

    for (int i = 0; i < portEnd - portInit; i++)
    {
        tasks[i]->join();
        delete tasks[i];
    }
}

void scanPort(std::string ip, int port)
{
    std::cout << "MyNC: Port scanning for " << ip << ":" << port << std::endl;
    auto scanTask = new std::thread(task, ip, port);
    scanTask->join();
    return;
}

#pragma endregion scanner

#pragma region UDP

void listeningUDP(int srcPort, bool isFileMessage, int dstPort)
{
    int listeningSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(srcPort);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(listeningSock, (sockaddr *)&address, sizeof(address));

    socklen_t len;
    std::cout << "MyNC: Start listening UDP " << srcPort << std::endl;

    while (true)
    {
        if (isFileMessage)
        {
            char sizeOfFileStr[16];
            char fileName[32];
            int sizeOfFile;

            recvfrom(listeningSock, (char *)sizeOfFileStr, 16, MSG_WAITALL, 0, &len);
            sizeOfFile = atoi(sizeOfFileStr);
            recvfrom(listeningSock, (char *)fileName, 32, MSG_WAITALL, 0, &len);
            char *bytes = new char[sizeOfFile];
            recvfrom(listeningSock, (char *)bytes, sizeOfFile, MSG_WAITALL, 0, &len);

            std::cout << "size of file: " << sizeOfFile << std::endl;
            std::cout << "name of file: " << fileName << std::endl;

            if (dstPort != -1)
            {
                int dstSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                sockaddr_in dstAddress;
                memset(&dstAddress, 0, sizeof(sockaddr_in));
                dstAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                dstAddress.sin_port = htons(dstPort);
                dstAddress.sin_family = AF_INET;

                sendto(dstSock, (char *)std::to_string(sizeOfFile).c_str(), 16, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
                sendto(dstSock, (char *)fileName, 32, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
                sendto(dstSock, (char *)bytes, sizeOfFile, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
            }
            else
            {
                std::ofstream ofs("accepted/" + std::string(fileName));
                ofs.close();
                std::fstream file;
                file.open("accepted/" + std::string(fileName), std::ios_base::out | std::ios_base::binary);

                if (file.is_open())
                {
                    std::cout << "data: " << bytes << std::endl;
                    file.write(bytes, sizeOfFile);
                    std::cout << "ok.save" << std::endl;
                }
            }
        }
        else
        {
            char message[1024] = "\0";
            socklen_t len;
            int sizeOfRecv = recvfrom(listeningSock, (char *)message, 1024, MSG_WAITALL, 0, &len);
            if (sizeOfRecv > 0)
            {
                if (dstPort != -1)
                {
                    int dstSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                    sockaddr_in dstAddress;
                    memset(&dstAddress, 0, sizeof(sockaddr_in));
                    dstAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                    dstAddress.sin_port = htons(dstPort);
                    dstAddress.sin_family = AF_INET;

                    sendto(dstSock, (char *)message, sizeOfRecv, 0, (const sockaddr *)&dstAddress, sizeof(dstAddress));
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

    int connectedSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    while (true)
    {
        if (isFileMessage)
        {
            std::string fileName;
            std::cout << "Enter file name: ";
            std::cin >> fileName;
            std::cout << "filename: " << fileName << std::endl;

            std::fstream file;
            file.open(fileName, std::ios_base::in | std::ios_base::binary);

            if (file.is_open())
            {
                int sizeOfFile = filesize(fileName);
                char *bytes = new char[sizeOfFile];
                file.read((char *)bytes, sizeOfFile);

                std::cout << "size: " << sizeOfFile << std::endl;
                std::cout << "name: " << fileName << std::endl;
                std::cout << "data: " << bytes << std::endl;

                sendto(connectedSock, (char *)std::to_string(sizeOfFile).c_str(), 16, 0, (const sockaddr *)&addr, sizeof(addr));
                sendto(connectedSock, (char *)fileName.c_str(), 32, 0, (const sockaddr *)&addr, sizeof(addr));
                sendto(connectedSock, (char *)bytes, sizeOfFile, 0, (const sockaddr *)&addr, sizeof(addr));
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
            sendto(connectedSock, (char *)message, msglen, 0, (const sockaddr *)&addr, sizeof(addr));
        }
    }
}

#pragma endregion UDP

#pragma region TCP

void listeningTCP(int port, bool isFileMessage, int dstPort)
{
    int listeningSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int dstSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in dstAddress;
    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(listeningSock, (sockaddr *)&address, sizeof(address));
    listen(listeningSock, SOMAXCONN);

    sockaddr_in addr;
    socklen_t len;
    std::cout << "MyNC: Start listening TCP " << port << std::endl;
    int connection;

    if (dstPort != -1)
    {
        memset(&dstAddress, 0, sizeof(sockaddr_in));
        dstAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        dstAddress.sin_port = htons(dstPort);
        dstAddress.sin_family = AF_INET;
        connection = connect(dstSock, (sockaddr *)&dstAddress, sizeof(dstAddress));

        if (connection == 0)
        {
            std::cout << "Connection to address for redirection succeeded" << std::endl;
        }
        else
        {
            std::cout << "Connection to address for redirection refused" << std::endl;
            close(listeningSock);
            close(dstSock);
            exit(EXIT_FAILURE);
        }
    }

    int connectedSock = 0;

    while (true)
    {
        connectedSock = accept(listeningSock, (sockaddr *)&addr, &len);
        std::cout << "Connected from " << inet_ntoa(address.sin_addr) << std::endl;

        if (connectedSock != 0)
        {
            pid_t pidAccept = fork();
            if (pidAccept)
            {
                if (isFileMessage)
                {
                    char sizeOfFileStr[16];
                    char fileName[32];
                    int sizeOfFile;

                    recv(connectedSock, (char *)&sizeOfFileStr, 16, 0);
                    sizeOfFile = atoi(sizeOfFileStr);
                    recv(connectedSock, (char *)&fileName, 32, 0);
                    char *bytes = new char[sizeOfFile];
                    recvfrom(connectedSock, (char *)bytes, sizeOfFile, MSG_WAITALL, 0, &len);

                    std::cout << "size of file: " << sizeOfFile << std::endl;
                    std::cout << "name of file: " << fileName << std::endl;

                    if (dstPort != -1)
                    {
                        send(dstSock, (char *)std::to_string(sizeOfFile).c_str(), 16, 0);
                        send(dstSock, (char *)fileName, 32, 0);
                        send(dstSock, (char *)bytes, sizeOfFile, 0);
                    }
                    else
                    {
                        std::ofstream ofs("accepted/" + std::string(fileName));
                        ofs.close();
                        std::fstream file;
                        file.open("accepted/" + std::string(fileName), std::ios_base::out | std::ios_base::binary);

                        if (file.is_open())
                        {
                            std::cout << "data: " << bytes << std::endl;
                            file.write(bytes, sizeOfFile);
                            std::cout << "ok.save" << std::endl;
                        }
                    }
                }
                else
                {
                    pid_t pidIn = fork();
                    if (pidIn)
                    {
                        while (true)
                        {
                            std::string msg;
                            std::cin >> msg;
                            size_t msglen = msg.length();
                            char message[msglen] = "\0";
                            strcpy(message, msg.c_str());
                            send(connectedSock, (char *)&message, sizeof(message), 0);
                        }
                    }
                    else
                    {
                        while (true)
                        {
                            char message[1024] = "\0";
                            int sizeOfRecv = recv(connectedSock, (char *)&message, sizeof(message), 0);
                            if (sizeOfRecv > 0)
                            {
                                if (dstPort != -1)
                                {
                                    send(dstSock, (char *)message, sizeOfRecv, 0);
                                }
                                else
                                {
                                    std::cout << message << std::endl;
                                }
                            }
                        }
                    }
                }
            }
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

    int connectedSock = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(connectedSock, (sockaddr *)&addr, sizeof(addr)) == 0)
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

                    send(connectedSock, (char *)std::to_string(file_size).c_str(), 16, 0);
                    send(connectedSock, (char *)path.c_str(), 32, 0);
                    send(connectedSock, (char *)bytes, file_size, 0);
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
            pid_t pidIn = fork();
            if (pidIn)
            {
                while (true)
                {
                    std::string msg;
                    std::cin >> msg;
                    size_t msglen = 1;
                    msglen = msg.length();
                    char message[msglen] = "\0";
                    strcpy(message, msg.c_str());
                    send(connectedSock, (char *)&message, sizeof(message), 0);
                }
            }
            else
            {
                while (true)
                {
                    char message[1024] = "\0";
                    int sizeOfRecv = recv(connectedSock, (char *)&message, sizeof(message), 0);
                    
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

#pragma endregion TCP