#include <openssl/sha.h> // OpenSSL 라이브러리 헤더
#include <WinSock2.h>    // 네트워크 소켓 관련 헤더
#include <WS2tcpip.h>    // TCP/IP 통신을 위한 헤더
#pragma comment(lib, "Ws2_32.lib") // Windows 소켓 라이브러리 링크

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
//#define FD_SETSIZE 100

// 함수 선언
int open_socket();
DWORD WINAPI handle_client(SOCKET client_socket);

// 전역 소켓 관리
extern SOCKET client_sockets[FD_SETSIZE];