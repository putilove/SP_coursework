#pragma once
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

static bool port_is_open(std::string ip, int port);

void task(std::string ip, int port);

void scanPorts(std::string ip, int portInit, int portEnd);

void scanPort(std::string ip, int port);