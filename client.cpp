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
#include <cstring>
#include <fstream>
#include <experimental/filesystem>   

using namespace std;

void recv_file(const int* sock);

int main(){
    sockaddr_in addr_info;
    memset(&addr_info, 0, sizeof(sockaddr_in));
    
    //socklen_t size_addr = sizeof(addr_info);
    addr_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_info.sin_port = htons(4321);
    addr_info.sin_family = AF_INET;

    int s_client = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(s_client, (sockaddr*)&addr_info, sizeof(addr_info)) == 0) {
        cout << "CONNECT" << endl;
        recv_file(&s_client);
    }
    else {
        cout << "NOT CONNECT" << endl;
    }
    return 0;
}

void recv_file(const int* sock){
    char file_size_str[16];
    char file_name[32];

    recv(*sock, file_size_str, 16, 0);
    int file_size = atoi(file_size_str);
    char* bytes = new char[file_size];

    recv(*sock, file_name, 32, 0);
    ofstream ofs("accepted/"+string(file_name)); //создать
    ofs.close();
    fstream file;
    file.open("accepted/"+string(file_name), ios_base::out | ios_base::binary);

    cout << "size: " << file_size << endl;
    cout << "name: " << file_name << endl;

    if(file.is_open()) {
        recv(*sock, bytes, file_size, 0);
        cout << "data: " << bytes << endl;
        file.write(bytes, file_size);
        cout << "ok.save" << endl;
    }
    else{
        cout << "error opening" << endl;
    }

    delete[] bytes;
    file.close();
}