#pragma once
#include "client_socket.h"

#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50

// 구조체 선언
typedef struct {
	int stock_id;
	int stock_price;
	int stock_count;
} STOCK_DATA;
typedef struct {
	int select;
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
	STOCK_DATA stock_data;
} RequestData;

add_member(SOCKET client_fd);
del_member(SOCKET client_fd);
login(SOCKET client_fd);
logout(SOCKET client_fd);