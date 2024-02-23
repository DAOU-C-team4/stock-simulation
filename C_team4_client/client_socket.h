#pragma once
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER_SIZE 1024
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 50
#define MAX_MESSAGE_LENGTH 200

/**************** 구조체 선언 ****************/
typedef struct {
	int select;       // 매수 매도 불린값
	int stock_id;     // 매수 매도할 주식 id
	int stock_count;  // 매수 매도할 주식 갯수
} STOCK_REQ;
typedef struct {
	int select;  // 메뉴 선택
	int balance; // 현금잔고
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
	STOCK_REQ stock_data;
} RequestData;
typedef struct {
	int stock_id;
	int stock_name;
	int stock_price;
	int stock_count;
} STOCK_RES;
typedef struct {
	int select;
	char session[MAX_SESSION_LENGTH]; // 로그인시 발급받는 토큰
	char msg[MAX_MESSAGE_LENGTH];     // 서버 전달 메세지
	STOCK_RES stock_arr[10];          // 주식 정보
	STOCK_RES my_stock[10];           // 내 주식 잔고
} ResponseData;

#define PORT 8080
#define SERVER_IP "127.0.0.1" // 로컬 pc 서버 ip주소
//#define SERVER_IP "172.30.1.93" // 준하님 pc 서버 ip주소

/**************** 소켓 관련 함수 ****************/
SOCKET connect_to_server();