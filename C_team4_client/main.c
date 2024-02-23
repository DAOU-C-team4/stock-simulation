#include <stdio.h>
#include <stdlib.h>
#include "client_socket.h"
#include "client_member.h"

#define MAX_BUFFER_SIZE 1024

void clear_input_buffer();

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

// 1. 홈화면 작업선택 함수
select_task_home(SOCKET client_fd) {
	// 기본 세팅
	int run = 1;
	int isLogin = 0; // 1인 경우 로그인 완료
	char message[MAX_BUFFER_SIZE];
	printf("\n반갑습니다. 키울까말까증권입니다.\n");

	// 홈 메뉴 반복
	do {
		int select = 0;
		printf("\n(1.회원가입 / 2.회원탈퇴 / 3.로그인 / 4.로그아웃 / 5.종료)\n");
		printf("원하는 작업을 지정해주세요 : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("회원가입\n\n");
			add_member(client_fd);
			break;
		case 2:
			printf("회원탈퇴\n\n");
			del_member(client_fd);
			break;
		case 3:
			printf("로그인\n\n");
			//로그인 함수 결과를 isLogin에 저장 (0: 실패, 1: 성공)
			isLogin = login(client_fd);
			//로그인 성공 시, 주식 매매 함수 시작
			if (isLogin) {
				stock_home(client_fd);
			}
			break;
		case 4:
			printf("로그아웃\n\n");
			logout(client_fd);
			isLogin = 0;
			break;
		case 5:
			printf("\n프로그램을 종료합니다. 좋은 하루 되세요 :)\n");
			exit(1);
			break;
		}

		// 서버로부터 수신
		char received_message[MAX_BUFFER_SIZE];
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return 1;
		}
		received_message[bytes_received] = '\0'; // 문자열 끝에 NULL 추가
		// 서버로부터 받은 메시지 출력
		printf("%s\n", received_message);

		printf("========================================================================\n");
	} while (run);
}

// 입력 버퍼 비우기 함수
void clear_input_buffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}