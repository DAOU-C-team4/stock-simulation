#pragma once
#include <WinSock2.h>
//#define SERVER_IP "172.30.1.93" // ���ϴ� pc ���� ip�ּ�
#define PORT 8080
#define SERVER_IP "127.0.0.1" // ���� pc ���� ip�ּ�
#define MAX_SESSION_LENGTH 200
#pragma comment(lib, "Ws2_32.lib")

SOCKET connect_to_server();