#pragma once
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fstream>

#pragma region scanner

bool port_is_open(std::string ip, int port);

void task(std::string ip, int port);

void scanPorts(std::string ip, int portInit, int portEnd);

void scanPort(std::string ip, int port);

#pragma endregion scanner

#pragma region TCP

void listeningTCP(int srcPort, bool isFileMessage, int dstPort);

void connectTCP(std::string ip, int host, bool isFileMessage);

#pragma endregion TCP

#pragma region UDP

void listeningUDP(int srcPort, bool isFileMessage, int dstPort);

void connectUDP(std::string ip, int port, bool isFileMessage);

#pragma endregion UDP


std::ifstream::pos_type filesize(std::string file_name);
