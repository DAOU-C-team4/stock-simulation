#include <openssl/sha.h> // OpenSSL ���̺귯�� ���
#include <WinSock2.h>    // ��Ʈ��ũ ���� ���� ���
#include <WS2tcpip.h>    // TCP/IP ����� ���� ���
#pragma comment(lib, "Ws2_32.lib") // Windows ���� ���̺귯�� ��ũ

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
//#define FD_SETSIZE 100

// �Լ� ����
int open_socket();
DWORD WINAPI handle_client(SOCKET client_socket);

// ���� ���� ����
extern SOCKET client_sockets[FD_SETSIZE];