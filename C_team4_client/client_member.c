#include <stdio.h>
#include <stdlib.h>
#include "client_socket.h"

// 1.1 회원가입요청
add_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 1;

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	printf("이름을 입력하세요: ");
	fgets(req_data.name, MAX_NAME_LENGTH, stdin);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.2 회원탈퇴요청
del_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 2;

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.3 로그인
login(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 3;

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 0;
	}

	return 1;
}

// 1.4 로그아웃
logout(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 4;
	strcpy(req_data.session, access);
	strcpy(access, "NONE");

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}