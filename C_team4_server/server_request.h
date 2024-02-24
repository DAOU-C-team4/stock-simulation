#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 50
#define MAX_MESSAGE_LENGTH 200

/**************** ����ü ���� ****************/
//Ŭ���̾�Ʈ���� ���� ��û
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

//Ŭ���̾�Ʈ���� ���� ����
typedef struct {
	int stock_id;
	int stock_name;
	int stock_price;
	int stock_count;
} STOCK_RES;
typedef struct {
	int select;
	char session[MAX_SESSION_LENGTH]; // �α��ν� �߱޹޴� ��ū
	char msg[MAX_MESSAGE_LENGTH];     // ���� ���� �޼���
	//STOCK_RES stock_trade;			  // �Ÿ� ����
	STOCK_RES stock_arr[10];          // �ֽ� ����
	STOCK_RES my_stock[10];           // �� �ֽ� �ܰ�
} ResponseData;

/**************** ȸ�� ���� DB �Լ� ****************/
int add_member(RequestData* req_data, ResponseData* res_data_ptr);
int del_member(RequestData* req_data, ResponseData* res_data_ptr);
int login(RequestData* req_data, ResponseData* res_data_ptr);
int logout(RequestData* req_data, ResponseData* res_data_ptr);