#pragma once
#include "client_socket.h"

/**************** 주식 관련 함수 ****************/
int stock_home(SOCKET client_fd);
int buyStock(SOCKET client_fd);
int sellStock(SOCKET client_fd);