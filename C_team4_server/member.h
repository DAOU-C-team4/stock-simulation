#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 200

// 구조체 선언
typedef struct {
	int select;
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
} RequestData;

int add_member(RequestData* req_data);
int del_member(RequestData* req_data);
int login(RequestData* req_data);
int logout(RequestData* req_data);
