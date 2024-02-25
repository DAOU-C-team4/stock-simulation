#include "client_member.h"
#include "client_stock.h"
#include "client_socket.h"

// 에세스 관리
char access[31] = "NONE";

int main(int argc, char* argv[]) {
	// 0 소켓연결
	SOCKET client_fd = connect_to_server();

	// 1.0 이벤트 객체 생성
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (event == NULL) {
		perror("Event creation failed");
		return 1;
	}

	// 1.1 리슨 스레드
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, listen_thread, client_fd, 0, &dwThreadId);
	if (hThread == NULL) {
		perror("Thread creation failed");
		return 1;
	}
	// 1.2 요청 스레드
	select_task_home(client_fd);

	// end. 소켓종료
	closesocket(client_fd);
	WSACleanup();

	return 0;
}

// 1.1 리슨 스레드
DWORD WINAPI listen_thread(SOCKET client_fd) {
	char received_message[MAX_BUFFER_SIZE];
	while (1) {
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			// 에러 처리 로직 추가
			break;
		}

		// 서버로부터 받은 메시지 처리
		ResponseData* res_data = (ResponseData*)received_message;
		if (strcmp(res_data->session, "NONE") && strcmp(res_data->session, "CLEAR")) {
			strcpy(access, res_data->session);
		}
		printf("\n서버로부터 응답:\n");
		printf("   select: %d\n", res_data->select);
		printf("   session: %s\n", res_data->session);
		printf("   msg: %s\n", res_data->msg);
		printf("현재 저장된 에세스 읽기: %s\n", access);

		// 이벤트 신호 발생
		SetEvent(event);
	}
	return;
}

// 1.2 요청 스레드
select_task_home(SOCKET client_fd) {
	// 기본 세팅
	int run = 1;
	int isLogin = 0; // 1인 경우 로그인 완료
	char message[MAX_BUFFER_SIZE];
	printf("\n반갑습니다. 키울까말까증권입니다.\n");

	// 홈 메뉴 반복
	do {
		// 로그인시 - 주식매매
		if (strcmp(access, "NONE") && strcmp(access, "CLEAR")) {
			stock_home(client_fd);
			logout(client_fd, access);
			WaitForSingleObject(event, INFINITE); // 신호 대기
			printf("========================================================================\n");
			continue;
		}
		// 로그인 아닐시 - 회원관리
		int select = 0;
		printf("\n(1.회원가입 / 2.회원탈퇴 / 3.로그인 / 4.종료)\n");
		printf("원하는 작업을 지정해주세요 : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("회원가입\n\n");
			add_member(client_fd);
			WaitForSingleObject(event, INFINITE); // 신호 대기
			break;
		case 2:
			printf("회원탈퇴\n\n");
			del_member(client_fd);
			WaitForSingleObject(event, INFINITE); // 신호 대기
			break;
		case 3:
			printf("로그인\n\n");
			login(client_fd);
			WaitForSingleObject(event, INFINITE); // 신호 대기
			break;
		case 4:
			printf("\n프로그램을 종료합니다. 좋은 하루 되세요 :)\n");
			exit(1);
			break;
		default:
			printf("\n잘못된 번호입니다.\n");
			continue;
		}
		printf("========================================================================\n");
	} while (run);
}
