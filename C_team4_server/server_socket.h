#include <openssl/sha.h> // OpenSSL ���̺귯�� ���
#include <WinSock2.h>    // ��Ʈ��ũ ���� ���� ���
#include <WS2tcpip.h>    // TCP/IP ����� ���� ���
#pragma comment(lib, "Ws2_32.lib") // Windows ���� ���̺귯�� ��ũ
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
//#define FD_SETSIZE 100

// �̺�Ʈ ��ü - ��Ƽ������ ��ȣ����
HANDLE event;

// �Լ� ����
int open_socket();
DWORD WINAPI handle_client(SOCKET client_socket);

// ���� ���� ����
extern int num_clients;
extern SOCKET client_sockets[FD_SETSIZE];
extern int client_tf[FD_SETSIZE]; //Ŭ���̾�Ʈ ���� �޸� ����(Ŭ���̾�Ʈ ���翩�� ����)
extern int client_login[FD_SETSIZE]; //�α��� ���� Ȯ��