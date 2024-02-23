#pragma once
#include "client_socket.h"

int stock_home(SOCKET client_fd);
int buyStock(SOCKET client_fd);
int sellStock(SOCKET client_fd);