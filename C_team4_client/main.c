#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

// �Լ� ����
SOCKET connect_to_server();
select_task_home();

int main(int argc, char* argv[]) {
	// 0. ���Ͽ���
	SOCKET client_fd = connect_to_server();

	// 1. Ȩȭ�鿡�� ���ϴ� �۾�����
	select_task_home(client_fd);

	// end. ��������
	closesocket(client_fd);
	WSACleanup();
	return 0;
}

// 0. ���Ͽ���
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

	printf("Connected to server\n");

	return client_fd;
}

// 1. Ȩȭ�� �۾����� �Լ�
select_task_home(SOCKET client_fd) {
	int run = 1;
	char message[MAX_BUFFER_SIZE];
	printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");
	do {
		int select = 0;
		printf("\n(1.ȸ������ / 2.ȸ��Ż�� / 3.�α��� / 4.�α׾ƿ� / 5.����)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");

		scanf("%d%*c", &select);
		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("ȸ������\n");
			//SOCKET client_fd = connect_to_server();
			// �޽��� �Է�
			printf("���ϴ� �޼����� �Է��ϼ���: ");
			fgets(message, MAX_BUFFER_SIZE, stdin);

			// ������ �޽��� ����
			int bytes_sent = send(client_fd, message, strlen(message), 0);
			if (bytes_sent == SOCKET_ERROR) {
				fprintf(stderr, "Send failed\n");
				return 1;
			}

			// �����κ��� �޽��� ����
			char received_message[MAX_BUFFER_SIZE];
			int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
			if (bytes_received == SOCKET_ERROR) {
				perror("recv failed");
				return 1;
			}
			received_message[bytes_received] = '\0'; // ���ڿ� ���� NULL �߰�
			// �����κ��� ���� �޽��� ���
			printf("�����κ��� �� �޼���: %s\n", received_message);
			break;
		case 2:
			printf("ȸ��Ż��\n");
			break;
		case 3:
			printf("�α���\n");
			break;
		case 4:
			printf("�α׾ƿ�\n");
			break;
		case 5:
			printf("\n");
			printf("���α׷��� �����մϴ�. ���� �Ϸ� �Ǽ��� :)\n");
			exit(1);
			break;
		}
		printf("========================================================================\n");
	} while (run);
}