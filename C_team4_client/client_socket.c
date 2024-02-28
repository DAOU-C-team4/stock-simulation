#include "client_socket.h"


// 0. 소켓연결
SOCKET connect_to_server() {
	WSADATA wsaData;
	SOCKET client_fd;
	struct sockaddr_in server_addr;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		perror("WSAStartup failed");
		exit(EXIT_FAILURE);
	}

	// Create socket
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	// Connect to server
	if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect failed");
		exit(EXIT_FAILURE);
	}

	//printf("Connected to server\n");

	return client_fd;
}