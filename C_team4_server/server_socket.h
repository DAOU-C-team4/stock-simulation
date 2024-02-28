#include <openssl/sha.h> // OpenSSL 라이브러리 헤더
#include <WinSock2.h>    // 네트워크 소켓 관련 헤더
#include <WS2tcpip.h>    // TCP/IP 통신을 위한 헤더
#pragma comment(lib, "Ws2_32.lib") // Windows 소켓 라이브러리 링크
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
//#define FD_SETSIZE 100

// 이벤트 객체 - 멀티스레드 신호제어
HANDLE event;

// 함수 선언
int open_socket();
DWORD WINAPI handle_client(SOCKET client_socket);

// 전역 소켓 관리
extern int num_clients;
extern SOCKET client_sockets[FD_SETSIZE];
extern int client_tf[FD_SETSIZE]; //클라이언트 소켓 메모리 관리(클라이언트 존재여부 저장)
extern int client_login[FD_SETSIZE]; //로그인 여부 확인