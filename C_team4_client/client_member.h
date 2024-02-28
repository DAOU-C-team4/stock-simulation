#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "client_socket.h"

/**************** ȸ�� ���� ��û �Լ� ****************/
req_add_member(SOCKET client_fd);
req_del_member(SOCKET client_fd);
req_login(SOCKET client_fd);
req_logout(SOCKET client_fd, char* access);

/**************** ȸ�� ���� ���� �Լ� ****************/
res_add_member(ResponseData* res_data);
res_del_member(ResponseData* res_data);
res_login(ResponseData* res_data, char* access);
res_logout(ResponseData* res_data, char* access);
res_memberInfo(ResponseData* res_data, char* access);

/**************** ��Ÿ ó���Լ� ****************/
clearConsoleArea(int left, int top, int width, int height);
enterPassword(char* password);
enterPasswordCheck(char* password);
enterPasswordLogin(char* password);
int getInputInteger(char* prompt);
char* getInputStringLimited(char* prompt, int max_length);
bool hasSpecialChar(const char* password, int i);
bool hasDigit(const char* password, int i);
bool hasAlpha(const char* password, int i);