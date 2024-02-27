#include "client_member.h"
#include "client_stock.h"
#include "client_socket.h"

// 에세스 관리
char access[31] = "NONE";

int main(int argc, char* argv[]) {
	// 0.1 소켓연결
	SOCKET client_fd = connect_to_server();
	// 0.2 이벤트 객체 생성
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (event == NULL) {
		perror("Event creation failed");
		return 1;
	}

	// 1. 리슨 스레드
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, listen_thread, client_fd, 0, &dwThreadId);
	if (hThread == NULL) {
		perror("Thread creation failed");
		return 1;
	}

	// 2. 요청 스레드
	select_task_home(client_fd);

	// end. 소켓종료
	closesocket(client_fd);
	WSACleanup();
	return 0;
}

// 1.1 리슨 스레드
DWORD WINAPI listen_thread(SOCKET client_fd) {
	char received_message[MAX_BUFFER_SIZE];
	int select;

	while (1) {
		// 서버 전송메세지 리슨
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			// 에러 처리 로직 추가
			break;
		}
		// 서버로부터 받은 메시지 처리
		ResponseData* res_data = (ResponseData*)received_message;

		if (strcmp(res_data->session, "NONE") && strcmp(res_data->session, "CLEAR")
			&& strcmp(res_data->session, "") && strcmp(res_data->session, "\0")
			&& res_data->session == NULL) {
			strcpy(access, res_data->session);
			printf("\nlisten_thread내부 session (%s): ", access);
		}
		//printf("\n서버로부터 응답 (%d): \n", res_data->select);
		//printf("\nlisten_thread session (%s): ", access);

		// 요청별 분기처리
		select = res_data->select;
		switch (select) {
		case 1: // 회원가입
			res_add_member(res_data);
			break;
		case 2: // 회원탈퇴
			res_del_member(res_data);
			break;
		case 3: // 로그인
			res_login(res_data, access);
			break;
		case 4: // 로그아웃
			res_logout(res_data, access);
			break;
		case 5: // 회원 정보 조회
			res_memberInfo(res_data, access);
			continue;
		case 200: // 주가 실시간 조회
			res_allStock(res_data);
			break;
		case 201: // 주식 매수
			res_buyStock(res_data);
			break;
		case 202: // 주식 매도
			res_sellStock(res_data);
			break;

		default:
			//printf("\n알 수 없는 요청입니다.\n");
			continue;
		}

		// 이벤트 신호 발생
		if (select != 200 && select != 0)
			SetEvent(event);
	}
	return;
}

// 1.2 요청 스레드
select_task_home(SOCKET client_fd) {
	// 기본 세팅
	int run = 1;
	char message[MAX_BUFFER_SIZE];
	/*system("cls");
	printf("\n반갑습니다. 키울까말까증권입니다.\n");*/

	// 홈 메뉴 반복
	do {
		// 로그인시 - 주식매매
		if (strcmp(access, "NONE") && strcmp(access, "CLEAR")) {
			//printf("%s\n", res_data->msg);
			//printf("   session: %s\n", access);
			req_allStock(client_fd, access);
			stock_home(client_fd, access);
			// 주식매매 홈에서 나갈시 로그아웃
			req_logout(client_fd, access);
			printf("\n========================================================================\n");
			continue;
		}
		// 로그인 아닐시 - 회원관리
		int select = 0;
		//system("cls");
		printf("\n반갑습니다. 키울까말까증권입니다.\n");
		printf("\n(1.회원가입 / 2.회원탈퇴 / 3.로그인 / 4.종료)\n");
		select = getInputInteger("원하는 작업을 지정해주세요 : ");
		printf("\n====================================================\n\n");
		switch (select)
		{
		case 1: // 회원가입
			req_add_member(client_fd);
			break;
		case 2: // 회원탈퇴
			req_del_member(client_fd);
			break;
		case 3: // 로그인
			req_login(client_fd);
			break;
		case 4: // 로그아웃
			printf("\n프로그램을 종료합니다. 좋은 하루 되세요 :)\n");
			run = 0;
			break;
		default:
			printf("\n잘못된 번호입니다.\n");
			continue;
		}
		printf("\n====================================\n");
	} while (run);
}
