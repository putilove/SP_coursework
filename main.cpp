#include <iostream>
#include "functions.h"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }
    if (argc == 4)
    {
        if (strcmp(argv[1], "-cu") == 0)
        {
            std::cout << "connect udp" << std::endl;
            connectUDP(std::string(argv[2]), atoi(argv[3]), false);
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            std::cout << "connect" << std::endl;
            connectTCP(std::string(argv[2]), atoi(argv[3]), false);
        }
    }
    return 0;
}