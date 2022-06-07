#include <iostream>
#include "functions.h"

int main(int argc, char *argv[])
{
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
        else if (strcmp(argv[1], "-lf") == 0)
        {
            std::cout << "listen file" << std::endl;
            listeningTCP(atoi(argv[2]), true, -1);
        }
        else if (strcmp(argv[1], "-l") == 0)
        {
            std::cout << "listen" << std::endl;
            listeningTCP(atoi(argv[2]), false, -1);
        }
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "-lufr") == 0)
        {
            std::cout << "listen udp file redirect" << std::endl;
            listeningUDP(atoi(argv[2]), true, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-lfr") == 0)
        {
            std::cout << "listen file redirect" << std::endl;
            listeningTCP(atoi(argv[2]), true, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-lur") == 0)
        {
            std::cout << "listen udp redirect" << std::endl;
            listeningUDP(atoi(argv[2]), false, atoi(argv[3]));
        }
        else if (strcmp(argv[1], "-lr") == 0)
        {
            std::cout << "listen tcp redirect" << std::endl;
            listeningTCP(atoi(argv[2]), false, atoi(argv[3]));
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
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-s") == 0)
        {
            std::cout << "scanport" << std::endl;
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