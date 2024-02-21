#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

// 함수 선언
SOCKET connect_to_server();
select_task_home();

int main(int argc, char* argv[]) {
	// 0. 소켓연결
	SOCKET client_fd = connect_to_server();

	// 1. 홈화면에서 원하는 작업선택
	select_task_home(client_fd);

	// end. 소켓종료
	closesocket(client_fd);
	WSACleanup();
	return 0;
}

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

	printf("Connected to server\n");

	return client_fd;
}

// 1. 홈화면 작업선택 함수
select_task_home(SOCKET client_fd) {
	int run = 1;
	char message[MAX_BUFFER_SIZE];
	printf("\n반갑습니다. 키울까말까증권입니다.\n");
	do {
		int select = 0;
		printf("\n(1.회원가입 / 2.회원탈퇴 / 3.로그인 / 4.로그아웃 / 5.종료)\n");
		printf("원하는 작업을 지정해주세요 : ");

		scanf("%d%*c", &select);
		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("회원가입\n");
			//SOCKET client_fd = connect_to_server();
			// 메시지 입력
			printf("원하는 메세지를 입력하세요: ");
			fgets(message, MAX_BUFFER_SIZE, stdin);

			// 서버로 메시지 전송
			int bytes_sent = send(client_fd, message, strlen(message), 0);
			if (bytes_sent == SOCKET_ERROR) {
				fprintf(stderr, "Send failed\n");
				return 1;
			}

			// 서버로부터 메시지 수신
			char received_message[MAX_BUFFER_SIZE];
			int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
			if (bytes_received == SOCKET_ERROR) {
				perror("recv failed");
				return 1;
			}
			received_message[bytes_received] = '\0'; // 문자열 끝에 NULL 추가
			// 서버로부터 받은 메시지 출력
			printf("서버로부터 온 메세지: %s\n", received_message);
			break;
		case 2:
			printf("회원탈퇴\n");
			break;
		case 3:
			printf("로그인\n");
			break;
		case 4:
			printf("로그아웃\n");
			break;
		case 5:
			printf("\n");
			printf("프로그램을 종료합니다. 좋은 하루 되세요 :)\n");
			exit(1);
			break;
		}
		printf("========================================================================\n");
	} while (run);
}