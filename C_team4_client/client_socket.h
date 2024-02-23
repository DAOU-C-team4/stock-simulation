#pragma once
#include <WinSock2.h>
//#define SERVER_IP "172.30.1.93" // 준하님 pc 서버 ip주소
#define PORT 8080
#define SERVER_IP "127.0.0.1" // 로컬 pc 서버 ip주소
#define MAX_SESSION_LENGTH 200
#pragma comment(lib, "Ws2_32.lib")

SOCKET connect_to_server();