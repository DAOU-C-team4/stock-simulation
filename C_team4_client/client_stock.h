#pragma once
#include "client_socket.h"
#include <time.h>
#include <Windows.h>
#include <ctype.h>

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

delay(clock_t delay_time);

/**************** 주식 홈 ****************/
stock_home(SOCKET client_fd, char* access);

/**************** 주식 관련 요청 함수 ****************/
req_buyStock(SOCKET client_fd, char* access);
req_sellStock(SOCKET client_fd, char* access);

/**************** 주식 관련 리슨 함수 ****************/
req_allStock(SOCKET client_fd, char* access);
res_buyStock(ResponseData* res_data);
res_sellStock(ResponseData* res_data);
res_allStock(ResponseData* res_data);