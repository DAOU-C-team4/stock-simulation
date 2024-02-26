#pragma once
#include "client_socket.h"

/**************** �ֽ� Ȩ ****************/
stock_home(SOCKET client_fd, char* access);

/**************** �ֽ� ���� ��û �Լ� ****************/
req_buyStock(SOCKET client_fd, char* access);
req_sellStock(SOCKET client_fd, char* access);

/**************** �ֽ� ���� ���� �Լ� ****************/
res_buyStock(ResponseData* res_data);
res_sellStock(ResponseData* res_data);
res_allStock(ResponseData* res_data);