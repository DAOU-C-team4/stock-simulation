#pragma once
#include "client_socket.h"

/**************** 주식 홈 ****************/
stock_home(SOCKET client_fd, char* access);

/**************** 주식 관련 요청 함수 ****************/
req_buyStock(SOCKET client_fd, char* access);
req_sellStock(SOCKET client_fd, char* access);

/**************** 주식 관련 리슨 함수 ****************/
res_buyStock(ResponseData* res_data);
res_sellStock(ResponseData* res_data);
res_allStock(ResponseData* res_data);