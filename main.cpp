#include <iostream>
#include "functions.h"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }
    if (argc == 3){
        if (strcmp(argv[1], "-luf") == 0)
        {
            std::cout << "listen udp file" << std::endl;
            listeningUDP(atoi(argv[2]), true, -1);
        }
        else if (strcmp(argv[1], "-lu") == 0)
        {
            std::cout << "listen udp" << std::endl;
            listeningUDP(atoi(argv[2]), false, -1);
        }
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "-lufr") == 0)
        {
            std::cout << "listen udp file redirect" << std::endl;
            listeningUDP(atoi(argv[2]), true, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-lur") == 0)
        {
            std::cout << "listen udp redirect" << std::endl;
            listeningUDP(atoi(argv[2]), false, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-cuf") == 0)
        {
            std::cout << "connect udp file" << std::endl;
            connectUDP(std::string(argv[2]), atoi(argv[3]), true);
        }
        else if (strcmp(argv[1], "-cu") == 0)
        {
            std::cout << "connect udp" << std::endl;
            connectUDP(std::string(argv[2]), atoi(argv[3]), false);
        }
        else if (strcmp(argv[1], "-cf") == 0)
        {
            std::cout << "connect file" << std::endl;
            connectTCP(std::string(argv[2]), atoi(argv[3]), true);
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            std::cout << "connect" << std::endl;
            connectTCP(std::string(argv[2]), atoi(argv[3]), false);
        }
    }
    return 0;
}