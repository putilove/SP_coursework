#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

using namespace std;

static bool port_is_open(string ip, int port){

	struct sockaddr_in addr_s;
    const char* addr = ip.c_str();

    short int fd=-1;
    fd_set fdset;
    struct timeval tv;
    int rc;
    int so_error;
    socklen_t len;
    struct timespec tstart={0,0}, tend={0,0};
    int seconds = 1;

    addr_s.sin_family = AF_INET; 
    addr_s.sin_addr.s_addr = inet_addr(addr);
    addr_s.sin_port = htons(port);

    clock_gettime(CLOCK_MONOTONIC, &tstart);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    rc = connect(fd, (struct sockaddr *)&addr_s, sizeof(addr_s));

    if ((rc == -1) && (errno != EINPROGRESS)) {
        close(fd);
        return false;
    }

    if (rc == 0) {
        clock_gettime(CLOCK_MONOTONIC, &tend);
        close(fd);
        return true;
    }

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    rc = select(fd + 1, NULL, &fdset, NULL, &tv);

    switch(rc) {
    case 1:
        len = sizeof(so_error);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) { // success
            clock_gettime(CLOCK_MONOTONIC, &tend);
            //printf("MyNC: connection to %s port %d succeeded.\n",
            //     addr, port);
            close(fd);
            return true;
        } else { // error
            clock_gettime(CLOCK_MONOTONIC, &tend);
            //printf("MyNC: connection to %s port %d failed: %s.\n",
            //    addr, port, strerror(so_error));
        }
        break;
    case 0: //timeout
        //printf("MyNC: connection to %s port %d failed. Timeout\n", addr, port);
        break;
    }
    close(fd);
	return false;
}


void task(std::string ip, int port)
{
    if (port_is_open(ip, port))
    {
        cout << "MyNC: connection to " << ip << " port " << port << " succeeded.\n";
    }
}

void scanPorts(std::string ip, int portInit, int portEnd)
{
	std::vector<std::thread *> tasks;
    
    for (int i = portInit; i < portEnd; i++)
    {    
        tasks.push_back(new std::thread(task, ip, i));
    }
    for (int i=0; i< portEnd - portInit ; i++){
        tasks[i]->join();
        delete tasks[i];
    }
}

int main(int argc, char **argv){
    cout << "MyNC: Port scanning for IP 82.179.48.251 1-1000" << endl;            
    scanPorts("82.179.48.251", 1, 1000);
    return 0;
}