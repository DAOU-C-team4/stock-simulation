#pragma once
#include "client_socket.h"

/**************** 회원 관련 함수 ****************/
add_member(SOCKET client_fd);
del_member(SOCKET client_fd);
login(SOCKET client_fd);
logout(SOCKET client_fd);