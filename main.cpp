#include <iostream>
#include "functions.h"

int main(int argc, char *argv[])
{
    if (argc == 3){
        if (strcmp(argv[1], "-lu") == 0)
        {
            listeningUDP(atoi(argv[2]), false, -1);
        }
        else if (strcmp(argv[1], "-l") == 0)
        {
            listeningTCP(atoi(argv[2]), false, -1);
        }
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "-lur") == 0)
        {
            listeningUDP(atoi(argv[2]), false, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-lr") == 0)
        {
            listeningTCP(atoi(argv[2]), false, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-cu") == 0)
        {
            connectUDP(std::string(argv[2]), atoi(argv[3]), false);
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            connectTCP(std::string(argv[2]), atoi(argv[3]), false);
        }
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-s") == 0)
        {
            scanPorts(argv[2], atoi(argv[3]), atoi(argv[4]));
        }
    }
    else
    {
        std::cout << "MyNCAT 1.0" << std::endl;
        std::cout << "Use IPv4 only" << std::endl;
        std::cout << "Usage: ./ncat [options] [hostname/port] [port] (port - for redirect)" << std::endl;
        std::cout << "-c,\t\t\t\t\t\tConnect to [hostname]:[port]" << std::endl;
        std::cout << "  --u\t\t\t\t\t\tUse UDP instead of default TCP" << std::endl;
        std::cout << "  --f\t\t\t\t\t\tUse connection for file transfers" << std::endl;
        std::cout << "-l,\t\t\t\t\t\tBind and listen [port] for incoming connections" << std::endl;
        std::cout << "  --u\t\t\t\t\t\tUse UDP instead of default TCP" << std::endl;
        std::cout << "  --f\t\t\t\t\t\tUse connection for file transfers" << std::endl;
        std::cout << "  --r\t\t\t\t\t\tRedirect data to other port" << std::endl;
    }
    return 0;
}