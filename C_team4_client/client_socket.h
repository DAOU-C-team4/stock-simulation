#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>    // TCP/IP ����� ���� ���
#include <limits.h>
#include <stdbool.h>
#include <conio.h>
#include <ctype.h>
#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER_SIZE 1024
#define MAX_ID_LENGTH 50
#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 50
#define MAX_MESSAGE_LENGTH 200
#define MAX_STOCK_NAME_LENGTH 50
#define MAX_STOCK_RES_LENGTH 10

#define PORT 8080
#define SERVER_IP "127.0.0.1" // ���� pc ���� ip�ּ�
//#define SERVER_IP "172.30.1.93" // ���ϴ� pc ���� ip�ּ�

// �̺�Ʈ ��ü - ��Ƽ������ ��ȣ����
HANDLE event;

/**************** ����ü ���� ****************/
typedef struct {
	int select;       // �ż� �ŵ� �Ҹ���
	int stock_id;     // �ż� �ŵ��� �ֽ� id
	int stock_count;  // �ż� �ŵ��� �ֽ� ����
} STOCK_REQ;
typedef struct {
	int select;  // �޴� ����
	int balance; // �����ܰ�
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
	STOCK_REQ stock_data;
} RequestData;
typedef struct {
	int stock_id;
	int stock_name;
	char stock_company_name[MAX_STOCK_NAME_LENGTH];
	int stock_price;
	int stock_count;
} STOCK_RES;
typedef struct {
	int select;
	int check;
	char session[MAX_SESSION_LENGTH]; // �α��ν� �߱޹޴� ��ū
	char msg[MAX_MESSAGE_LENGTH];     // ���� ���� �޼���
	STOCK_RES stock_arr[MAX_STOCK_RES_LENGTH];          // �ֽ� ����
	STOCK_RES my_stock[MAX_STOCK_RES_LENGTH];           // �� �ֽ� �ܰ�
} ResponseData;

/**************** ���� ���� �Լ� ****************/
SOCKET connect_to_server();
DWORD WINAPI listen_thread(SOCKET client_fd);