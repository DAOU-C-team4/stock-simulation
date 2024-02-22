#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member.h"
#include "socket_connect.h"
#include "db_connect.h"

// 2.0 소켓 초기화 함수
int initialize_winsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		fprintf(stderr, "WSAStartup failed with error code: %d\n", result);
		return -1;
	}
	return 0;
}

// 2.1 소켓 열기 함수
int open_socket() {
	// Winsock 초기화
	if (initialize_winsock() != 0) {
		return -1;
	}

	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// 서버 소켓 생성
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		return -1;
	}
	printf("socket create success\n");

	// 소켓 옵션 설정
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// 소켓에 주소 바인딩
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		int error_code = WSAGetLastError();
		fprintf(stderr, "bind failed with error code: %d\n", error_code);
		perror("bind failed");
		return -1;
	}

	// 서버가 듣기 시작
	if (listen(server_fd, SOMAXCONN) < 0) {
		perror("listen");
		return -1;
	}
	printf("socket ready\n");

	return server_fd;
}

// 2.2 클라이언트 요청 처리 함수
DWORD WINAPI handle_client(int client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received;
	int run = 1;

	// 클라이언트로부터 요청 대기
	do {
		printf("%d번 클라이언트 요청대기\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return;
		}
		//buffer[bytes_received] = '\0'; // 문자열 끝에 NULL 추가
		//printf("%d번 클라이언트 요청 : %s\n", client_socket, buffer);

		// 클라이언트로부터 받은 데이터를 req_data 구조체로 형변환
		RequestData* req_data = (RequestData*)buffer;

		// 요청에 따라 다른 작업 실행
		switch (req_data->select)
		{
		case 1:
			add_member(req_data);
			break;
		case 2:
			del_member(req_data);
			break;
		case 3:
			login(req_data);
			break;
		case 4:
			logout(req_data);
			break;
		}

		// 클라이언트로 결과 전송
		int bytes_sent = send(client_socket, buffer, bytes_received, 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return;
		}
	} while (run);
}
