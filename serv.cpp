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

void send_file(const int* sock, const string& file_name);

int main(){
    sockaddr_in addr_info;
    memset(&addr_info, 0, sizeof(sockaddr_in));
    
    socklen_t size_addr = sizeof(addr_info);
    addr_info.sin_port = htons(4321);
    addr_info.sin_family = AF_INET;

    int s_listen = socket(AF_INET, SOCK_STREAM, 0);
    bind(s_listen, (sockaddr*)&addr_info, sizeof(addr_info));
    listen(s_listen, SOMAXCONN);
    int s_for_conect = accept(s_listen, (sockaddr*)&addr_info, &size_addr);

    if(s_for_conect != 0) {
        cout << "CONNECT" << endl;
        string path;
        cin >> path;
        send_file(&s_for_conect, path);
    }
    return 0;
}

std::ifstream::pos_type filesize(string file_name){
    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
    return in.tellg();
}

void send_file(const int* sock, const string& file_name){
    fstream file;
    file.open(file_name, ios_base::in | ios_base::binary);

    if(file.is_open()){
        int file_size = filesize(file_name);
        char* bytes = new char[file_size];
        file.read((char*)bytes, file_size);

        cout << "size: " << file_size << endl;
        cout << "name: " << file_name << endl;
        cout << "data: " << bytes << endl;

        send(*sock, to_string(file_size).c_str(), 16, 0);
        send(*sock, file_name.c_str(), 32, 0);
        send(*sock, bytes, file_size, 0);
    }
    else {
        cout << "Error opening file" << endl;
    }
    file.close();
}