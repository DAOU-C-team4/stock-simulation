#pragma once
#include "client_socket.h"

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

clearConsoleArea(int left, int top, int width, int height);

/**************** ȸ�� ���� ��û �Լ� ****************/
req_add_member(SOCKET client_fd);
req_del_member(SOCKET client_fd);
req_login(SOCKET client_fd);
req_logout(SOCKET client_fd, char* access);

/**************** ȸ�� ���� ���� �Լ� ****************/
res_add_member(ResponseData* res_data);
res_del_member(ResponseData* res_data);
res_login(ResponseData* res_data);
res_logout(ResponseData* res_data, char* access);
res_memberInfo(ResponseData* res_data, char* access);