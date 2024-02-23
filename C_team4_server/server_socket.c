#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_socket.h"

/**************** ��� ���� �⺻ ****************/
// 1 ���� �ʱ�ȭ �Լ�
int initialize_winsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		fprintf(stderr, "WSAStartup failed with error code: %d\n", result);
		return -1;
	}
	return 0;
}

// 2 ���� ���� �Լ�
int open_socket() {
	// Winsock �ʱ�ȭ
	if (initialize_winsock() != 0) {
		return -1;
	}

	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// ���� ���� ����
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		return -1;
	}
	printf("socket create success\n");

	// ���� �ɼ� ����
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// ���Ͽ� �ּ� ���ε�
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		int error_code = WSAGetLastError();
		fprintf(stderr, "bind failed with error code: %d\n", error_code);
		perror("bind failed");
		return -1;
	}

	// ������ ��� ����
	if (listen(server_fd, SOMAXCONN) < 0) {
		perror("listen");
		return -1;
	}
	printf("socket ready\n");

	return server_fd;
}

