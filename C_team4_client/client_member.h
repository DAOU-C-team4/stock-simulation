#pragma once
#include "client_socket.h"

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

clearConsoleArea(int left, int top, int width, int height);
enterPassword(char* password);

/**************** 회원 관련 요청 함수 ****************/
req_add_member(SOCKET client_fd);
req_del_member(SOCKET client_fd);
req_login(SOCKET client_fd);
req_logout(SOCKET client_fd, char* access);

/**************** 회원 관련 리슨 함수 ****************/
res_add_member(ResponseData* res_data);
res_del_member(ResponseData* res_data);
res_login(ResponseData* res_data, char* access);
res_logout(ResponseData* res_data, char* access);
res_memberInfo(ResponseData* res_data, char* access);

/**************** 기타 처리함수 ****************/
clearConsoleArea(int left, int top, int width, int height);
enterPassword(char* password);
enterPasswordCheck(char* password);
enterPasswordLogin(char* password);
int getInputInteger(char* prompt);
char* getInputStringLimited(char* prompt, int max_length);
bool hasSpecialChar(const char* password, int i);
bool hasDigit(const char* password, int i);
bool hasAlpha(const char* password, int i);