#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

//#define SERVER_IP "172.30.1.93" // ���ϴ� pc ���� ip�ּ�
#define PORT 8080
#define SERVER_IP "127.0.0.1" // ���� pc ���� ip�ּ�
#define MAX_BUFFER_SIZE 1024
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 200

// �Լ� ����
SOCKET connect_to_server();
select_task_home(SOCKET client_fd);
buyStock(SOCKET client_fd);  // �ֽ� �ż� �Լ�
sellStock(SOCKET client_fd); // �ֽ� �ŵ� �Լ�
void clear_input_buffer();

// ����ü ����
typedef struct {
	int stock_id;
	int stock_price;
	int stock_count;
} STOCK_DATA;
typedef struct {
	int select;
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
	STOCK_DATA stock_data;
} RequestData;


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
	// �⺻ ����
	int run = 1;
	int isLogin = 0; // 1�� ��� �α��� �Ϸ�
	char message[MAX_BUFFER_SIZE];
	printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");

	// Ȩ �޴� �ݺ�
	do {
		int select = 0;
		printf("\n(1.ȸ������ / 2.ȸ��Ż�� / 3.�α��� / 4.�α׾ƿ� / 5.����)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("ȸ������\n\n");
			add_member(client_fd);
			break;
		case 2:
			printf("ȸ��Ż��\n\n");
			del_member(client_fd);
			break;
		case 3:
			printf("�α���\n\n");
			//�α��� �Լ� ����� isLogin�� ���� (0: ����, 1: ����)
			isLogin = login(client_fd);
			//�α��� ���� ��, �ֽ� �Ÿ� �Լ� ����
			if (isLogin) {
				stock_home(client_fd);
			}
			break;
		case 4:
			printf("�α׾ƿ�\n\n");
			logout(client_fd);
			isLogin = 0;
			break;
		case 5:
			printf("\n���α׷��� �����մϴ�. ���� �Ϸ� �Ǽ��� :)\n");
			exit(1);
			break;
		}

		// �����κ��� ����
		char received_message[MAX_BUFFER_SIZE];
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return 1;
		}
		received_message[bytes_received] = '\0'; // ���ڿ� ���� NULL �߰�
		// �����κ��� ���� �޽��� ���
		printf("%s\n", received_message);

		printf("========================================================================\n");
	} while (run);
}

// 1.1 ȸ�����Կ�û
add_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 1;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	//clear_input_buffer();
	printf("��й�ȣ�� �Է��ϼ���: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	//clear_input_buffer();
	printf("�̸��� �Է��ϼ���: ");
	fgets(req_data.name, MAX_NAME_LENGTH, stdin);
	//clear_input_buffer();

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.2 ȸ��Ż���û
del_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 2;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	clear_input_buffer();
	printf("��й�ȣ�� �Է��ϼ���: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	clear_input_buffer();

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.3 �α���
login(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 3;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	clear_input_buffer();
	printf("��й�ȣ�� �Է��ϼ���: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	clear_input_buffer();

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 0;
	}

	return 1;
}

// 1.4 �α׾ƿ�
logout(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 4;

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 2 �α����� �ֽ� ���� Ȩ
stock_home(SOCKET client_fd) {
	do {
		int select = 0;
		printf("\n(1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�Ÿ� ����)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			buyStock(client_fd);
			break;
		case 2:
			sellStock(client_fd);
			break;
		case 3:
			printf("�Ÿ� ����\n\n");
			break;
		default:
			printf("\n1, 2, 3�� �� �ϳ��� �Է��ϼ���\n");
			break;
		}
	} while (1 != 0);
}

// �Է� ���� ���� �Լ�
void clear_input_buffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

//�ֽ� �ż� �Լ�
buyStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 6;
	
	printf("�ż��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("�ż��� �ݾ��� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_price);
	printf("�ż��� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	// ������ ����
	int bytes_sent = send(client_fd, (RequestData*) &req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

//�ֽ� �ŵ� �Լ�
sellStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 7;

	printf("�ŵ��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("�ŵ��� �ݾ��� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_price);
	printf("�ŵ��� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	int bytes_sent = send(client_fd, (RequestData*) &req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}