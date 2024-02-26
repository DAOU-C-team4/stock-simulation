#pragma once
#include "client_socket.h"

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

clearConsoleArea(int left, int top, int width, int height);

/**************** 회원 관련 요청 함수 ****************/
req_add_member(SOCKET client_fd);
req_del_member(SOCKET client_fd);
req_login(SOCKET client_fd);
req_logout(SOCKET client_fd, char* access);

/**************** 회원 관련 리슨 함수 ****************/
res_add_member(ResponseData* res_data);
res_del_member(ResponseData* res_data);
res_login(ResponseData* res_data);
res_logout(ResponseData* res_data, char* access);
res_memberInfo(ResponseData* res_data, char* access);