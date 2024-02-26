#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 50
#define MAX_MESSAGE_LENGTH 200
#define MAX_STOCK_NAME_LENGTH 50

#define MAX_STOCK_RES_LENGTH 50

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
	char stock_company_name[50];
	int stock_price;
	int stock_count;
} STOCK_RES;
typedef struct {
	int select;
	int check;
	char session[MAX_SESSION_LENGTH]; // 로그인시 발급받는 토큰
	char msg[MAX_MESSAGE_LENGTH];     // 서버 전달 메세지
	STOCK_RES stock_arr[10];          // 주식 정보
	STOCK_RES my_stock[10];           // 내 주식 잔고
} ResponseData;

/**************** 회원 관련 함수 ****************/
int add_member(RequestData* req_data, ResponseData* res_data_ptr);
int del_member(RequestData* req_data, ResponseData* res_data_ptr);
int login(RequestData* req_data, ResponseData* res_data_ptr);
int logout(RequestData* req_data, ResponseData* res_data_ptr);

/**************** 주식 관련 함수 ****************/
int checkSession(char* session);
int allStock(RequestData* req_data, ResponseData* res_data_ptr);
int buyStock(RequestData* req_data, ResponseData* res_data_ptr);
int sellStock(RequestData* req_data, ResponseData* res_data_ptr);
