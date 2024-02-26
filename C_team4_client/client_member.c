#include <stdio.h>
#include <stdlib.h>
#include "client_socket.h"
#include "client_member.h"
#include "client_stock.h"

clearConsoleArea(int left, int top, int width, int height) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 현재 콘솔 창의 버퍼 정보 가져오기
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	DWORD dwConSize = width * height;
	COORD upperLeft = { (SHORT)left, (SHORT)top };
	DWORD dwCharsWritten;

	// 특정 영역을 공백으로 채우기
	FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, upperLeft, &dwCharsWritten);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, upperLeft, &dwCharsWritten);

	// 커서 위치 조정
	SetConsoleCursorPosition(hConsole, upperLeft);
}

/**************** 회원 관련 요청 함수 ****************/
// 1.1 회원가입 요청
req_add_member(SOCKET client_fd) {

	system("cls");
	printf("\n반갑습니다. 키울까말까증권입니다.\n");
	printf(">> 회원가입 <<\n\n");
	RequestData req_data;
	req_data.select = 1;
	char password_check[MAX_PASSWORD_LENGTH];

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	printf("비밀번호를 한번더 입력하세요: ");
	fgets(password_check, MAX_PASSWORD_LENGTH, stdin);
	// 비밀번호 확인
	if (strcmp(req_data.password, password_check) != 0) {
		printf("비밀번호가 일치하지 않습니다. 다시 시도해주세요.\n");
		return 1;
	}
	printf("이름을 입력하세요: ");
	fgets(req_data.name, MAX_NAME_LENGTH, stdin);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.2 회원탈퇴 요청
req_del_member(SOCKET client_fd) {
	system("cls");
	printf("\n안녕하세요. 키울까말까증권입니다.\n");
	printf(">> 회원탈퇴 <<\n\n");
	RequestData req_data;
	req_data.select = 2;
	char confirmation = 'y';

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	printf("정말 탈퇴하시겠습니까? (y/n): ");
	confirmation = fgetc(stdin);
	if (confirmation != 'y' && confirmation != 'Y') {
		printf("회원탈퇴가 취소되었습니다.\n");
		return 0; // 탈퇴 취소
	}

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.3 로그인 요청
req_login(SOCKET client_fd) {
	system("cls");
	printf("\n반갑습니다. 키울까말까증권입니다.\n");
	printf(">> 로그인 <<\n\n");
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
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.4 로그아웃 요청
req_logout(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 4;
	strcpy(req_data.session, access);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

/**************** 회원 관련 리슨 함수 ****************/
// 2.1 회원가입 리슨
res_add_member(ResponseData* res_data) {
	//system("cls");
	//printf(">> 회원가입 <<\n\n");
	printf("%s\n", res_data->msg);
	return 0;
}

// 2.2 회원탈퇴 리슨
res_del_member(ResponseData* res_data) {
	//system("cls");
	//printf(">> 회원탈퇴 <<\n\n");
	printf("%s\n", res_data->msg);
	return 0;
}

// 2.3 로그인 리슨
res_login(ResponseData* res_data, char* access) {
	//system("cls");
	//printf(">> 로그인 <<\n\n");
	printf("%s\n", res_data->msg);
	printf("   session: %s\n", res_data->session);
	strcpy(access, res_data->session);
	res_allStock(res_data);
	return 0;
}

// 2.4 로그아웃 리슨
res_logout(ResponseData* res_data, char* access) {
	printf(">> 로그아웃 <<\n\n");
	printf("   session: %s\n", res_data->session);
	strcpy(access, "NONE");
	return 0;
}

// 2.5 회원정보 리슨
res_memberInfo(ResponseData* res_data, char* access) {
	printf(">> 회원정보 <<\n\n");

	return 0;
}